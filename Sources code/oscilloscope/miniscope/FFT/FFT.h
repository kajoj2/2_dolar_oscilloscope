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

#ifndef FftH
#define FftH

#include <System.hpp>

/** \brief Spectrum calculation
 */
class FFT
{
public:
	/** \brief Get nearest value of power of two LOWER than N or same to N
	*/
	static unsigned int NearestPowerOfTwo(unsigned int count);
	static void Transform(float *pRealIn, float *pRealOut, float *pImagOut);
	static void SetWindowType(AnsiString asName);
	static void SetSamplesCount(int samples);
private:
	static bool IsPowerOfTwo(unsigned int number);
	static unsigned int NeededBits(unsigned int iSamples);
	static unsigned int ReverseBits(unsigned int idx, unsigned int bits);
};

#endif
