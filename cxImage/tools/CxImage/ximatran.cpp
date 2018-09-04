// xImaTran.cpp : Transformation functions
/* 07/08/2001 v1.00 - Davide Pizzolato - www.xdp.it
 * CxImage version 5.99c 17/Oct/2004
 */

#include "ximage.h"

#if CXIMAGE_SUPPORT_BASICTRANSFORMATIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool CxImage::Flip()
{
	if (!pDib) return false;
	CxImage* imatmp = new CxImage(*this,false,false,true);
	if (!imatmp) return false;
	if (!imatmp->IsValid()) return false;
	BYTE *iSrc,*iDst;
	iSrc=info.pImage + (head.biHeight-1)*info.dwEffWidth;
	iDst=imatmp->info.pImage;
    for(long y=0; y < head.biHeight; y++){
                memcpy(iDst,iSrc,info.dwEffWidth);
                iSrc-=info.dwEffWidth;
                iDst+=info.dwEffWidth;
        }



	Transfer(*imatmp);
	delete imatmp;
	return true;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool CxImage::Negative()
{
	if (!pDib) return false;

	if (head.biBitCount<=8){
		if (IsGrayScale()){ //GRAYSCALE, selection
			if (pSelection){
				for(long y=info.rSelectionBox.bottom; y<info.rSelectionBox.top; y++){
					for(long x=info.rSelectionBox.left; x<info.rSelectionBox.right; x++){
#if CXIMAGE_SUPPORT_SELECTION
						if (SelectionIsInside(x,y))
#endif //CXIMAGE_SUPPORT_SELECTION
						{
							SetPixelIndex(x,y,(BYTE)(255-GetPixelIndex(x,y)));
						}
					}
				}
			} else {
				for(long y=0; y<head.biHeight; y++){
					for(long x=0; x<head.biWidth; x++){
						SetPixelIndex(x,y,(BYTE)(255-GetPixelIndex(x,y)));
					}
				}
			}
		} else { //PALETTE, full image
			RGBQUAD* ppal=GetPalette();
			for(DWORD i=0;i<head.biClrUsed;i++){
				ppal[i].rgbBlue =(BYTE)(255-ppal[i].rgbBlue);
				ppal[i].rgbGreen =(BYTE)(255-ppal[i].rgbGreen);
				ppal[i].rgbRed =(BYTE)(255-ppal[i].rgbRed);
			}
		}
	} else {
		if (pSelection==NULL){ //RGB, full image
			BYTE *iSrc=info.pImage;
			for(unsigned long i=0; i < head.biSizeImage; i++){
				*iSrc=(BYTE)~(*(iSrc));
				iSrc++;
			}
		} else { // RGB with selection
			RGBQUAD color;
			for(long y=info.rSelectionBox.bottom; y<info.rSelectionBox.top; y++){
				for(long x=info.rSelectionBox.left; x<info.rSelectionBox.right; x++){
#if CXIMAGE_SUPPORT_SELECTION
					if (SelectionIsInside(x,y))
#endif //CXIMAGE_SUPPORT_SELECTION
					{
						color = GetPixelColor(x,y);
						color.rgbRed = (BYTE)(255-color.rgbRed);
						color.rgbGreen = (BYTE)(255-color.rgbGreen);
						color.rgbBlue = (BYTE)(255-color.rgbBlue);
						SetPixelColor(x,y,color);
					}
				}
			}
		}
		//<DP> invert transparent color too
		info.nBkgndColor.rgbBlue = (BYTE)(255-info.nBkgndColor.rgbBlue);
		info.nBkgndColor.rgbGreen = (BYTE)(255-info.nBkgndColor.rgbGreen);
		info.nBkgndColor.rgbRed = (BYTE)(255-info.nBkgndColor.rgbRed);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
#endif //CXIMAGE_SUPPORT_BASICTRANSFORMATIONS
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_TRANSFORMATION
////////////////////////////////////////////////////////////////////////////////


/**
 * Rotates image around it's center.
 * Method can use interpolation with paletted images, but does not change pallete, so results vary.
 * (If you have only four colours in a palette, there's not much room for interpolation.)
 *
 * \param  angle - angle in degrees (positive values rotate clockwise)
 * \param  *iDst - destination image (if null, this image is changed)
 * \param  inMethod - interpolation method used
 *              (IM_NEAREST_NEIGHBOUR produces aliasing (fast), IM_BILINEAR softens picture a bit (slower)
 *               IM_SHARPBICUBIC is slower and produces some halos...)
 * \param  ofMethod - overflow method (how to choose colour of pixels that have no source)
 * \param  replColor - replacement colour to use (OM_COLOR, OM_BACKGROUND with no background colour...)
 * \param  optimizeRightAngles - call faster methods for 90, 180, and 270 degree rotations. Faster methods
 *                         are called for angles, where error (in location of corner pixels) is less
 *                         than 0.25 pixels.
 * \param  bKeepOriginalSize - rotates the image without resizing.
 *
 * \author ***bd*** 2.2004
 */

////////////////////////////////////////////////////////////////////////////////
/**
 * Resizes the image. mode can be 0 for slow (bilinear) method ,
 * 1 for fast (nearest pixel) method, or 2 for accurate (bicubic spline interpolation) method.
 * The function is faster with 24 and 1 bpp images, slow for 4 bpp images and slowest for 8 bpp images.
 */
bool CxImage::Resample(long newx, long newy, int mode, CxImage* iDst)
{
	if (newx==0 || newy==0) return false;

	if (head.biWidth==newx && head.biHeight==newy){
		if (iDst) iDst->Copy(*this);
		return true;
	}

	float xScale, yScale, fX, fY;
	xScale = (float)head.biWidth  / (float)newx;
	yScale = (float)head.biHeight / (float)newy;

	CxImage newImage;
	newImage.CopyInfo(*this);
	newImage.Create(newx,newy,head.biBitCount,GetType());
	newImage.SetPalette(GetPalette());
	if (!newImage.IsValid()) return false;

	switch (mode) {
	case 1: // nearest pixel
	{
		for(long y=0; y<newy; y++){
			info.nProgress = (long)(100*y/newy);
			if (info.nEscape) break;
			fY = y * yScale;
			for(long x=0; x<newx; x++){
				fX = x * xScale;
				newImage.SetPixelColor(x,y,GetPixelColor((long)fX,(long)fY));
			}
		}
		break;
	}
	case 2: // bicubic interpolation by Blake L. Carlson <blake-carlson(at)uiowa(dot)edu
	{
		float f_x, f_y, a, b, rr, gg, bb, r1, r2;
		int   i_x, i_y, xx, yy;
		RGBQUAD rgb;
		BYTE* iDst;
		for(long y=0; y<newy; y++){
			info.nProgress = (long)(100*y/newy);
			if (info.nEscape) break;
			f_y = (float) y * yScale - 0.5f;
			i_y = (int) floor(f_y);
			a   = f_y - (float)floor(f_y);
			for(long x=0; x<newx; x++){
				f_x = (float) x * xScale - 0.5f;
				i_x = (int) floor(f_x);
				b   = f_x - (float)floor(f_x);

				rr = gg = bb = 0.0f;
				for(int m=-1; m<3; m++) {
					r1 = KernelBSpline((float) m - a);
					yy = i_y+m;
					if (yy<0) yy=0;
					if (yy>=head.biHeight) yy = head.biHeight-1;
					for(int n=-1; n<3; n++) {
						r2 = r1 * KernelBSpline(b - (float)n);
						xx = i_x+n;
						if (xx<0) xx=0;
						if (xx>=head.biWidth) xx=head.biWidth-1;

						if (head.biClrUsed){
							rgb = GetPixelColor(xx,yy);
						} else {
							iDst  = info.pImage + yy*info.dwEffWidth + xx*3;
							rgb.rgbBlue = *iDst++;
							rgb.rgbGreen= *iDst++;
							rgb.rgbRed  = *iDst;
						}

						rr += rgb.rgbRed * r2;
						gg += rgb.rgbGreen * r2;
						bb += rgb.rgbBlue * r2;
					}
				}

				if (head.biClrUsed)
					newImage.SetPixelColor(x,y,RGB(rr,gg,bb));
				else {
					iDst = newImage.info.pImage + y*newImage.info.dwEffWidth + x*3;
					*iDst++ = (BYTE)bb;
					*iDst++ = (BYTE)gg;
					*iDst   = (BYTE)rr;
				}

			}
		}
		break;
	}
	default: // bilinear interpolation
		if (!(head.biWidth>newx && head.biHeight>newy && head.biBitCount==24)) {
			//\A9 1999 Steve McMahon (steve@dogma.demon.co.uk)
			long ifX, ifY, ifX1, ifY1, xmax, ymax;
			float ir1, ir2, ig1, ig2, ib1, ib2, dx, dy;
			BYTE r,g,b;
			RGBQUAD rgb1, rgb2, rgb3, rgb4;
			xmax = head.biWidth-1;
			ymax = head.biHeight-1;
			for(long y=0; y<newy; y++){
				info.nProgress = (long)(100*y/newy);
				if (info.nEscape) break;
				fY = y * yScale;
				ifY = (int)fY;
				ifY1 = min(ymax, ifY+1);
				dy = fY - ifY;
				for(long x=0; x<newx; x++){
					fX = x * xScale;
					ifX = (int)fX;
					ifX1 = min(xmax, ifX+1);
					dx = fX - ifX;
					// Interpolate using the four nearest pixels in the source
					if (head.biClrUsed){
						rgb1=GetPaletteColor(GetPixelIndex(ifX,ifY));
						rgb2=GetPaletteColor(GetPixelIndex(ifX1,ifY));
						rgb3=GetPaletteColor(GetPixelIndex(ifX,ifY1));
						rgb4=GetPaletteColor(GetPixelIndex(ifX1,ifY1));
					}
					else {
						BYTE* iDst;
						iDst = info.pImage + ifY*info.dwEffWidth + ifX*3;
						rgb1.rgbBlue = *iDst++;	rgb1.rgbGreen= *iDst++;	rgb1.rgbRed =*iDst;
						iDst = info.pImage + ifY*info.dwEffWidth + ifX1*3;
						rgb2.rgbBlue = *iDst++;	rgb2.rgbGreen= *iDst++;	rgb2.rgbRed =*iDst;
						iDst = info.pImage + ifY1*info.dwEffWidth + ifX*3;
						rgb3.rgbBlue = *iDst++;	rgb3.rgbGreen= *iDst++;	rgb3.rgbRed =*iDst;
						iDst = info.pImage + ifY1*info.dwEffWidth + ifX1*3;
						rgb4.rgbBlue = *iDst++;	rgb4.rgbGreen= *iDst++;	rgb4.rgbRed =*iDst;
					}
					// Interplate in x direction:
					ir1 = rgb1.rgbRed   * (1 - dy) + rgb3.rgbRed   * dy;
					ig1 = rgb1.rgbGreen * (1 - dy) + rgb3.rgbGreen * dy;
					ib1 = rgb1.rgbBlue  * (1 - dy) + rgb3.rgbBlue  * dy;
					ir2 = rgb2.rgbRed   * (1 - dy) + rgb4.rgbRed   * dy;
					ig2 = rgb2.rgbGreen * (1 - dy) + rgb4.rgbGreen * dy;
					ib2 = rgb2.rgbBlue  * (1 - dy) + rgb4.rgbBlue  * dy;
					// Interpolate in y:
					r = (BYTE)(ir1 * (1 - dx) + ir2 * dx);
					g = (BYTE)(ig1 * (1 - dx) + ig2 * dx);
					b = (BYTE)(ib1 * (1 - dx) + ib2 * dx);
					// Set output
					newImage.SetPixelColor(x,y,RGB(r,g,b));
				}
			}
		} else {
			//high resolution shrink, thanks to Henrik Stellmann <henrik.stellmann@volleynet.de>
			const long ACCURACY = 1000;
			long i,j; // index for faValue
			long x,y; // coordinates in  source image
			BYTE* pSource;
			BYTE* pDest = newImage.info.pImage;
			long* naAccu  = new long[3 * newx + 3];
			long* naCarry = new long[3 * newx + 3];
			long* naTemp;
			long  nWeightX,nWeightY;
			float fEndX;
			long nScale = (long)(ACCURACY * xScale * yScale);

			memset(naAccu,  0, sizeof(long) * 3 * newx);
			memset(naCarry, 0, sizeof(long) * 3 * newx);

			int u, v = 0; // coordinates in dest image
			float fEndY = yScale - 1.0f;
			for (y = 0; y < head.biHeight; y++){
				info.nProgress = (long)(100*y/head.biHeight); //<Anatoly Ivasyuk>
				if (info.nEscape) break;
				pSource = info.pImage + y * info.dwEffWidth;
				u = i = 0;
				fEndX = xScale - 1.0f;
				if ((float)y < fEndY) {       // complete source row goes into dest row
					for (x = 0; x < head.biWidth; x++){
						if ((float)x < fEndX){       // complete source pixel goes into dest pixel
							for (j = 0; j < 3; j++)	naAccu[i + j] += (*pSource++) * ACCURACY;
						} else {       // source pixel is splitted for 2 dest pixels
							nWeightX = (long)(((float)x - fEndX) * ACCURACY);
							for (j = 0; j < 3; j++){
								naAccu[i] += (ACCURACY - nWeightX) * (*pSource);
								naAccu[3 + i++] += nWeightX * (*pSource++);
							}
							fEndX += xScale;
							u++;
						}
					}
				} else {       // source row is splitted for 2 dest rows
					nWeightY = (long)(((float)y - fEndY) * ACCURACY);
					for (x = 0; x < head.biWidth; x++){
						if ((float)x < fEndX){       // complete source pixel goes into 2 pixel
							for (j = 0; j < 3; j++){
								naAccu[i + j] += ((ACCURACY - nWeightY) * (*pSource));
								naCarry[i + j] += nWeightY * (*pSource++);
							}
						} else {       // source pixel is splitted for 4 dest pixels
							nWeightX = (int)(((float)x - fEndX) * ACCURACY);
							for (j = 0; j < 3; j++) {
								naAccu[i] += ((ACCURACY - nWeightY) * (ACCURACY - nWeightX)) * (*pSource) / ACCURACY;
								*pDest++ = (BYTE)(naAccu[i] / nScale);
								naCarry[i] += (nWeightY * (ACCURACY - nWeightX) * (*pSource)) / ACCURACY;
								naAccu[i + 3] += ((ACCURACY - nWeightY) * nWeightX * (*pSource)) / ACCURACY;
								naCarry[i + 3] = (nWeightY * nWeightX * (*pSource)) / ACCURACY;
								i++;
								pSource++;
							}
							fEndX += xScale;
							u++;
						}
					}
					if (u < newx){ // possibly not completed due to rounding errors
						for (j = 0; j < 3; j++) *pDest++ = (BYTE)(naAccu[i++] / nScale);
					}
					naTemp = naCarry;
					naCarry = naAccu;
					naAccu = naTemp;
					memset(naCarry, 0, sizeof(int) * 3);    // need only to set first pixel zero
					pDest = newImage.info.pImage + (++v * newImage.info.dwEffWidth);
					fEndY += yScale;
				}
			}
			if (v < newy){	// possibly not completed due to rounding errors
				for (i = 0; i < 3 * newx; i++) *pDest++ = (BYTE)(naAccu[i] / nScale);
			}
			delete [] naAccu;
			delete [] naCarry;
		}
	}



	//select the destination
	if (iDst) iDst->Transfer(newImage);
	else Transfer(newImage);

	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * New simpler resample. Adds new interpolation methods and simplifies code (using GetPixelColorInterpolated
 * and GetAreaColorInterpolated). It also (unlike old method) interpolates alpha layer.
 *
 * \param  newx, newy - size of resampled image
 * \param  inMethod - interpolation method to use (see comments at GetPixelColorInterpolated)
 *              If image size is being reduced, averaging is used instead (or simultaneously with) inMethod.
 * \param  ofMethod - what to replace outside pixels by (only significant for bordering pixels of enlarged image)
 * \param  iDst - pointer to destination CxImage or NULL.
 * \param  disableAveraging - force no averaging when shrinking images (Produces aliasing.
 *                      You probably just want to leave this off...)
 *
 * \author ***bd*** 2.2004
 */
#endif
