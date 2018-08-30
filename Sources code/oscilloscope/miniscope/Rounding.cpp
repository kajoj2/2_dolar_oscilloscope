//---------------------------------------------------------------------------


#pragma hdrstop

#include "Rounding.h"
#include <assert.h>
#include <math.h>


#pragma package(smart_init)

float GetNextRoundHigherValue(float val)
{
	assert (val > 0.0f);
	float order = log10(val);
	if (order - floor(order) > 0.9999f)	// helps with precision problem (or not?)
		order = ceil(order);
	else
		order = floor(order);
	float fNewVal = val / pow(10.0f, order);
	if (fNewVal < 1.0f)
		fNewVal = 1.0f;
	else if (fNewVal < 2.0f)
		fNewVal = 2.0f;
	else if (fNewVal < 5.0f)
		fNewVal = 5.0f;
	else
	{
		fNewVal = 1.0f;
		order += 1.0f;
	}
	fNewVal *= pow(10.0f, order);
	return fNewVal;
}

float GetNextRoundLowerValue(float val)
{
	assert (val > 0.0f);
	float order = log10(val);
	if (order - floor(order) > 0.9999f)	// helps with precision problem (or not?)
		order = ceil(order);
	else
		order = floor(order);
	float fNewVal = val / pow(10.0f, order);
	if (fNewVal > 5.0f)
		fNewVal = 5.0f;
	else if (fNewVal > 2.0f)
		fNewVal = 2.0f;
	else if (fNewVal > 1.0f)
		fNewVal = 1.0f;
	else
	{
		fNewVal = 5.0f;
		order -= 1.0f;
	}
	fNewVal *= pow(10.0f, order);
	return fNewVal;
}

bool IsRoundingNeeded(float val)
{
	assert (val > 0.0f);
	float order = log10(val);
	if (order - floor(order) > 0.9999f)	// helps with precision problem (or not?)
		order = ceil(order);
	else
		order = floor(order);
	float fNewVal = val / pow(10.0f, order);
	if (
		fabs(fNewVal/1.0f - 1.0f) < 0.001 ||
		fabs(fNewVal/2.0f - 1.0f) < 0.001 ||
		fabs(fNewVal/5.0f - 1.0f) < 0.001
		)
	{
		return false;
	}
	return true;
}
