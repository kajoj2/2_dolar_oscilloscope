/** \file FFT.h
*/

/* Copyright (C) 2008-2009 Tomasz Ostrowski <tomasz.o.ostrowski at gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.

 * Miniscope is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include "FFT.h"
#include "FFTWindowFactory.h";
#include "FFTWindow.h";
#include <math.h>
#include <assert.h>

namespace
{
	FFTWindow* fftwindow = NULL;
	unsigned int iFftSamples = 1024;
}

unsigned int FFT::NearestPowerOfTwo(unsigned int count)
{
	unsigned int i = 1;
	while (i < count) {
		i = i << 1;
	}
	return i >> 1;
}

void FFT::Transform(float *pRealIn, float *pRealOut, float *pImagOut)
{
	if(!pRealIn || !pRealOut || !pImagOut)
		return;
	if(!IsPowerOfTwo(iFftSamples))
		return;

	if (fftwindow)
		fftwindow->Process(pRealIn);

	for(unsigned int i=0; i<iFftSamples; i++)
	{
		unsigned int j = ReverseBits (i, NeededBits (iFftSamples));
		pRealOut[j] = pRealIn[i];
		pImagOut[j] = 0.0f;
	}
 
	unsigned int iBEnd = 1;	// end of block
	for(unsigned int iBSize=2; iBSize<=iFftSamples; iBSize<<=1)
	{
		float delta_angle = 2.0f * M_PI / (float)iBSize;
		float sm2 = sin(-2 * delta_angle);
		float sm1 = sin(-delta_angle);
		float cm2 = cos(-2 * delta_angle);
		float cm1 = cos(-delta_angle);
		float w = 2 * cm1;
		float ar[3], ai[3];

		for(unsigned int i=0; i<iFftSamples; i+=iBSize)
		{
			ar[2] = cm2;
			ar[1] = cm1;
			ai[2] = sm2;
			ai[1] = sm1;
			for (unsigned int j=i, n=0; n<iBEnd; j++, n++)
			{
				ar[0] = w*ar[1] - ar[2];
				ar[2] = ar[1];
				ar[1] = ar[0];

				ai[0] = w*ai[1] - ai[2];
				ai[2] = ai[1];
				ai[1] = ai[0];

				unsigned int k = j + iBEnd;
				float tmpReal = ar[0]*pRealOut[k] - ai[0]*pImagOut[k];
				float tmpImag = ar[0]*pImagOut[k] + ai[0]*pRealOut[k];

				pRealOut[k] = pRealOut[j] - tmpReal;
				pImagOut[k] = pImagOut[j] - tmpImag;
				pRealOut[j] += tmpReal;
				pImagOut[j] += tmpImag;
			}
		}
		iBEnd = iBSize;
	}
}

bool FFT::IsPowerOfTwo(unsigned int number)
{
	if(number < 2)
		return false;
	if(number & (number-1))
		return false;
    return true;
}

unsigned int FFT::NeededBits(unsigned int iSamples)
{
	if(iSamples < 2)
		return 0;
	for (unsigned int i=0; ; i++)
	{
		if(iSamples & ((unsigned int)1 << i))
			return i;
    }
}

unsigned int FFT::ReverseBits(unsigned int idx, unsigned int bits)
{
	unsigned int rev = 0;
	for(unsigned int i=0; i<bits; i++)
	{
		rev = (rev << 1) | (idx & 1);
		idx >>= 1;
	}
	return rev;
}

void FFT::SetWindowType(AnsiString asName)
{
	if (asName == "" && fftwindow)
	{
		// cleanup to make CG happy
		delete fftwindow;
		fftwindow = NULL;
		return;
	}
	else if (fftwindow)
	{
		if (fftwindow->GetDescription().name != asName.c_str())
		{
			delete fftwindow;
			fftwindow = NULL;
		}
		else
		{
            return;
        }
	}

	fftwindow = FFTWindowFactory::Instance().Create(asName.c_str());
}

void FFT::SetSamplesCount(int samples)
{
	iFftSamples = samples;
	if (fftwindow)
		fftwindow->Resize(iFftSamples);	
}

