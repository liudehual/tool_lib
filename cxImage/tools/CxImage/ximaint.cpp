//// xImaInt.cpp : interpolation functions
///* 02/2004 - Branko Brevensek
// * CxImage version 5.99c 17/Oct/2004 - Davide Pizzolato - www.xdp.it
// */

#include "ximage.h"


//////////////////////////////////////////////////////////////////////////////////
float CxImage::KernelBSpline(const float x)
{
	if (x>2.0f) return 0.0f;
	// thanks to Kristian Kratzenstein
	float a, b, c, d;
	float xm1 = x - 1.0f; // Was calculatet anyway cause the "if((x-1.0f) < 0)"
	float xp1 = x + 1.0f;
	float xp2 = x + 2.0f;

	if ((xp2) <= 0.0f) a = 0.0f; else a = xp2*xp2*xp2; // Only float, not float -> double -> float
	if ((xp1) <= 0.0f) b = 0.0f; else b = xp1*xp1*xp1;
	if (x <= 0) c = 0.0f; else c = x*x*x;
	if ((xm1) <= 0.0f) d = 0.0f; else d = xm1*xm1*xm1;

	return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));

	/* equivalent <Vladim\EDr Kloucek>
	if (x < -2.0)
		return(0.0f);
	if (x < -1.0)
		return((2.0f+x)*(2.0f+x)*(2.0f+x)*0.16666666666666666667f);
	if (x < 0.0)
		return((4.0f+x*x*(-6.0f-3.0f*x))*0.16666666666666666667f);
	if (x < 1.0)
		return((4.0f+x*x*(-6.0f+3.0f*x))*0.16666666666666666667f);
	if (x < 2.0)
		return((2.0f-x)*(2.0f-x)*(2.0f-x)*0.16666666666666666667f);
	return(0.0f);
	*/
}

//////////////////////////////////////////////////////////////////////////////////
///**
