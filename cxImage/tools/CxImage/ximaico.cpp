/*
 * File:	ximaico.cpp
 * Purpose:	Platform Independent ICON Image Class Loader and Writer (MS version)
 * 07/Aug/2001 Davide Pizzolato - www.xdp.it
 * CxImage version 5.99c 17/Oct/2004
 */

#include "ximaico.h"

#if CXIMAGE_SUPPORT_ICO

////////////////////////////////////////////////////////////////////////////////
bool CxImageICO::Decode(CxFile *hFile)
{
	if (hFile==NULL) return false;

	DWORD off = hFile->Tell(); //<yuandi>
	int	page=info.nFrame;	//internal icon structure indexes

	// read the first part of the header
	ICONHEADER icon_header;
	hFile->Read(&icon_header,sizeof(ICONHEADER),1);
	// check if it's an icon or a cursor
	if ((icon_header.idReserved == 0) && ((icon_header.idType == 1)||(icon_header.idType == 2))) {

		info.nNumFrames = icon_header.idCount;

		// load the icon descriptions
		ICONDIRENTRY *icon_list = (ICONDIRENTRY *)malloc(icon_header.idCount * sizeof(ICONDIRENTRY));
		int c;
		for (c = 0; c < icon_header.idCount; c++)
			hFile->Read(icon_list + c, sizeof(ICONDIRENTRY), 1);

		if ((page>=0)&&(page<icon_header.idCount)){

			// get the bit count for the colors in the icon <CoreyRLucier>
			BITMAPINFOHEADER bih;
			hFile->Seek(off + icon_list[page].dwImageOffset, SEEK_SET);
			hFile->Read(&bih,sizeof(BITMAPINFOHEADER),1);
			c = bih.biBitCount;

			// allocate memory for one icon
			Create(icon_list[page].bWidth,icon_list[page].bHeight, c, CXIMAGE_FORMAT_ICO);	//image creation

			// read the palette
			RGBQUAD pal[256];
			hFile->Read(pal,head.biClrUsed*sizeof(RGBQUAD), 1);
			SetPalette(pal,head.biClrUsed);	//palette assign

			//read the icon
			if (c<=24){
				hFile->Read(info.pImage, head.biSizeImage, 1);
			} else { // 32 bit icon
				BYTE* dst = info.pImage;
				BYTE* buf=(BYTE*)malloc(4*head.biHeight*head.biWidth);
				BYTE* src = buf;
				hFile->Read(buf, 4*head.biHeight*head.biWidth, 1);
				for (long y = 0; y < head.biHeight; y++) {
					for(long x=0;x<head.biWidth;x++){
						*dst++=src[0];
						*dst++=src[1];
						*dst++=src[2];
						src+=4;
					}
				}
				free(buf);
			}
			// apply the AND and XOR masks
			int maskwdt = ((head.biWidth+31) / 32) * 4;	//line width of AND mask (always 1 Bpp)
			int masksize = head.biHeight * maskwdt;				//size of mask
			BYTE *mask = (BYTE *)malloc(masksize);
			if (hFile->Read(mask, masksize, 1)){

				bool bGoodMask=false;
				for (int im=0;im<masksize;im++){
					if (mask[im]!=255){
						bGoodMask=true;
						break;
					}
				}

				if (bGoodMask){

					if (c==24){ //check if there is only one transparent color
						RGBQUAD cc,ct;
						long* pcc = (long*)&cc;
						long* pct = (long*)&ct;
						int nTransColors=0;
						for (int y = 0; y < head.biHeight; y++){
							for (int x = 0; x < head.biWidth; x++){
								if (((mask[y*maskwdt+(x>>3)] >> (7-x%8)) & 0x01)){
									cc = GetPixelColor(x,y,false);
									if (nTransColors==0){
										nTransColors++;
										ct = cc;
									} else {
										if (*pct!=*pcc){
											nTransColors++;
										}
									}
								}
							}
						}
						if (nTransColors==1){
							SetTransColor(ct);
							SetTransIndex(0);
						}
					}

					// <vho> - Transparency support w/o Alpha support
					if (c <= 8){ // only for icons with less than 256 colors (XP icons need alpha).

						// find a color index, which is not used in the image
						// it is almost sure to find one, bcs. nobody uses all possible colors for an icon

						BYTE colorsUsed[256];
						memset(colorsUsed, 0, sizeof(colorsUsed));

						for (int y = 0; y < head.biHeight; y++){
							for (int x = 0; x < head.biWidth; x++){
								colorsUsed[GetPixelIndex(x,y)] = 1;
							}
						}

						int iTransIdx = -1;
						for (int x = 0; x < (int)head.biClrUsed; x++){
							if (colorsUsed[x] == 0){
								iTransIdx = x; // this one is not in use. we may use it as transparent color
								break;
							}
						}

						// Go thru image and set unused color as transparent index if needed
						if (iTransIdx >= 0){
							bool bNeedTrans = false;
							for (int y = 0; y < head.biHeight; y++){
								for (int x = 0; x < head.biWidth; x++){
									// AND mask (Each Byte represents 8 Pixels)
									if (((mask[y*maskwdt+(x>>3)] >> (7-x%8)) & 0x01)){
										// AND mask is set (!=0). This is a transparent part
										SetPixelIndex(x, y, iTransIdx);
										bNeedTrans = true;
									}
								}
							}
							// set transparent index if needed
							if (bNeedTrans)	SetTransIndex(iTransIdx);
						}
					}
				} else {
					SetTransIndex(0); //empty mask, set black as transparent color
					Negative();
				}
			}
			free(mask);

			free(icon_list);
			// icon has been loaded successfully!
			return true;
		}
		free(icon_list);
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
bool CxImageICO::Encode(CxFile * hFile)
{
	if (EncodeSafeCheck(hFile)) return false;

	//check format limits
	if ((head.biWidth>255)||(head.biHeight>255)){
		strcpy(info.szLastError,"Can't save this image as icon");
		return false;
	}

	//prepare the palette struct
	RGBQUAD* pal=GetPalette();
	if (head.biBitCount<=8 && pal==NULL) return false;

	int maskwdt=((head.biWidth+31)/32)*4; //mask line width
	int masksize=head.biHeight * maskwdt; //size of mask
	int bitcount=head.biBitCount;
	int imagesize=head.biSizeImage;

	//fill the icon headers
	ICONHEADER icon_header={0,1,1};
	ICONDIRENTRY icon_list={(BYTE)head.biWidth,(BYTE)head.biHeight,(BYTE)head.biClrUsed ,0,0,(WORD)bitcount,
							sizeof(BITMAPINFOHEADER)+head.biClrUsed*sizeof(RGBQUAD)+
							imagesize+masksize,
							sizeof(ICONHEADER)+sizeof(ICONDIRENTRY)};
	BITMAPINFOHEADER bi={sizeof(BITMAPINFOHEADER),head.biWidth,2*head.biHeight,1,(WORD)bitcount,
						0,imagesize,0,0,0,0};

	hFile->Write(&icon_header,sizeof(ICONHEADER),1);			//write the headers
	hFile->Write(&icon_list,sizeof(ICONDIRENTRY),1);
	hFile->Write(&bi,sizeof(BITMAPINFOHEADER),1);
	if (pal) hFile->Write(pal,head.biClrUsed*sizeof(RGBQUAD),1); //write palette


	hFile->Write(info.pImage,imagesize,1);	//write image


	//save transparency mask
	BYTE* mask=(BYTE*)calloc(masksize,1);	//create empty AND/XOR masks
	if (!mask) return false;

	//prepare the variables to build the mask
	BYTE* iDst;
	int pos,i;
	RGBQUAD c={0,0,0,0};
	RGBQUAD ct = GetTransColor();
	long* pc = (long*)&c;
	long* pct= (long*)&ct;
	bool bTransparent = info.nBkgndIndex != -1;

	//build the mask
	for (int y = 0; y < head.biHeight; y++) {
		for (int x = 0; x < head.biWidth; x++) {
			i=0;
			c=GetPixelColor(x,y,false);
			if (bTransparent && *pc==*pct) i=1;
			iDst = mask + y*maskwdt + (x>>3);
			pos = 7-x%8;
			*iDst &= ~(0x01<<pos);
			*iDst |= ((i & 0x01)<<pos);
		}
	}
	//write AND/XOR masks
	hFile->Write(mask,masksize,1);
	free(mask);
	return true;
}
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_ICO

