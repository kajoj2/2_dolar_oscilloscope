/** \file
 *  \brief Routines for physical values string formatting
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
 
//---------------------------------------------------------------------------


#pragma hdrstop

#include "ValueStringFormatter.h"
#include <iomanip>
#include <sstream>
#include <math.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)


std::string ValueStringFormatter::VoltageToStr(float val, int precision)
{
	std::string sUnit;
	if (fabs(val) < 0.1e-3f)
	{
		val *= 1e6;
		sUnit = "uV";
	}
	else if (fabs(val) < 0.1f)
	{
		val *= 1000;
		sUnit = "mV";
	}
	else if (fabs(val) < 100)
	{
		sUnit = "V";
	}
	else
	{
		val /= 1000;
		sUnit = "kV";
	}
	std::ostringstream buffer;
	buffer << std::fixed << std::setprecision(precision) << val;
	std::string ret = buffer.str();
	return (ret + sUnit);
}

std::string ValueStringFormatter::TimeToStr(float val, int precision, bool precision_absolute)
{
	std::string sUnit;
	if (fabs(val) < 0.1e-9f)
	{
		val *= 1.0e12f;
		sUnit = "ps";
	}
	else if (fabs(val) < 0.1e-6f)
	{
		val *= 1.0e9f;
		sUnit = "ns";
	}
	else if (fabs(val) < 0.1e-3f)
	{
		val *= 1.0e6f;
		sUnit = "us";
	}
	else if (fabs(val) < 0.1f)
	{
		val *= 1000;
		sUnit = "ms";
	}
	else
	{
		sUnit = "s";
	}
	std::ostringstream buffer;
	buffer << std::fixed << std::setprecision(precision) << val;
	std::string ret = buffer.str();
	return (ret + sUnit);
}

std::string ValueStringFormatter::FreqToStr(float val, int precision)
{
	std::string sUnit;
	if (fabs(val)>1000000)
	{
		val /=1000000;
		sUnit = "MHz";
	}
	else if (fabs(val)>1000)
	{
		val /=1000;
		sUnit = "kHz";
	}
	else
	{
		sUnit = "Hz";
	}
	std::ostringstream buffer;
	buffer << std::fixed << std::setprecision(precision) << val;
	std::string ret = buffer.str();
	return (ret + sUnit);
}

std::string ValueStringFormatter::ValToStr(float val, int precision, bool precision_absolute)
{
	if (!precision_absolute)
		throw("ValueStringFormatter::ValToStr - not implemented precision_absolute");
	std::ostringstream buffer;
	if (fabs(val) > 0.001f && fabs(val) < 1000)
		buffer << std::fixed << std::setprecision(precision) << val;
	else
		buffer << std::scientific << std::setprecision(precision) << val;
	return buffer.str();
}


