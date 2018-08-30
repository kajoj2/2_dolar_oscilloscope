//---------------------------------------------------------------------------


#pragma hdrstop

#include "Hsv.h"
#include <assert.h>
#include <algorithm>

//---------------------------------------------------------------------------

#pragma package(smart_init)

TColor Hsv::ToRGB(void)
{
	assert(h>=0 && h<360);

	int sextant = (h % 360) / 60;
	int frac = h % 60;

	int p1 = 255 - s;
	int p2 = 255 - (s * frac) / 59;
	int p3 = 255 - (s * (59 - frac)) / 59;

	p1 = p1 * v / 255;
	p2 = p2 * v / 255;
	p3 = p3 * v / 255;

	unsigned char r, g, b;

	switch (sextant)
	{
		case 0: r = v,  g = p3, b = p1; break;
		case 1: r = p2, g = v,  b = p1; break;
		case 2: r = p1, g = v,  b = p3; break;
		case 3: r = p1, g = p2, b = v;  break;
		case 4: r = p3, g = p1, b = v;  break;
		default: r = v,  g = p1, b = p2; break;
	}

	return (TColor)RGB(r, g, b);
}

void Hsv::FromRGB(TColor color)
{
    int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);
	int m = std::min(r, std::min(b, g));

	v = std::max(r, std::max(b, g));

	s = (v == 0) ? 0 : (v - m) * 255 / v;

	if (v <= m)
	{
		h = 0;
		return;
	}

	int r1 = (v - r) * 59 / (v - m);
	int g1 = (v - g) * 59 / (v - m);
	int b1 = (v - b) * 59 / (v - m);

	if (v == r && m == g)
		h = 300 + b1;
	else if (v == r)
		h = 60 - g1;
	else if (v == g && m == b)
		h = 60 + r1;
	else if (v == g)
		h = 180 - b1;
	else if (m == r)
		h = 180 + g1;
	else
		h = 300 - r1;
}

TColor Hsv::GetUniformColor(int count, int start_hue, int id)
{
	assert(id < count);
	int delta = 360/count;
	h = start_hue + (id * delta);
	s = 255;
	v = 255;
	return ToRGB();
}
