/** \file
*/

#include "Measurements.h"
#include "device/data.h"
#include "Log.h"
#include <math.h>
#include <algorithm>
#include <limits>

//#define LOCAL_DEBUG
#ifdef LOCAL_DEBUG
#	define TRACE(args) LOG args
static int TODO__DISABLE_DEBUG;
#else
#	define TRACE(args)
#endif

void Measurements::Process(const Data &data, int channel)
{
	fMin = std::numeric_limits<float>::max();
	fMax = (-1.0) * fMin;
	fAvg = 0;

    float fSquareSum = 0;
	std::vector<float>::const_iterator it;
	std::vector<float> &samples = const_cast<Data&>(data).getChannel(channel).dataOsc;
	for (it = samples.begin(); it != samples.end(); ++it)
	{
		float val = *it;
		if (val < fMin)
			fMin = val;
		if (val > fMax)
			fMax = val;
		fAvg += val;
		fSquareSum += (val*val);
	}
	fAvg /= samples.size();
	fRms = sqrt(fSquareSum / samples.size());

	// try to estimate signal frequency
	// locate & count signal crossing average value
	float fPrev = std::numeric_limits<float>::max();
	int counter = 0;
	std::vector<int> periods;
	bool started = false;   // used to ignore time from start to first crossing point
	bool armed = false;		// used to implement kind of hysteresis when detecting crossing
	bFreqValid = false;
	if (fMax - fAvg < fAvg - fMin)
	{
		float Vth = fAvg - (fAvg - fMin)/2;
		TRACE((E_LOG_TRACE, "Vth = %f", Vth));
		for (it = samples.begin(); it != samples.end(); ++it)
		{
			counter++;
			if (armed == false)
			{
				if (fPrev <= Vth && *it > Vth)
				{
					armed = true;
					TRACE((E_LOG_TRACE, "Armed after %d samples", counter));
				}
			}

			if (armed)
			{
				if (fPrev <= fAvg && *it > fAvg)	// rising slope
				{
					// transition moment found
					TRACE((E_LOG_TRACE, "Transition after %d samples", counter));
					armed = false;
					if (started)
						periods.push_back(counter);
					else
						started = true;
					counter = 0;
				}
			}
			fPrev = *it;
			TRACE((E_LOG_TRACE, "Val = %f", fPrev));
		}
	}
	else
	{
        float Vth = fAvg + ((fMax - fAvg)/2);
		for (it = samples.begin(); it != samples.end(); ++it)
		{
			counter++;
			if (armed == false)
			{
				if (fPrev > Vth && *it <= Vth)
					armed = true;
			}

			if (armed)
			{
				if (fPrev > fAvg && *it <= fAvg)
				{
					// transition moment found
					armed = false;
					if (started)
						periods.push_back(counter);
					else
						started = true;
					counter = 0;
				}
			}
			fPrev = *it;
		}
	}

	if (periods.size() >= 3)	// do not take too fast assumption about periodic nature
	{
		float fPeriodSum = 0;
		for (unsigned int i=0; i<periods.size(); ++i) {
			fPeriodSum += periods[i];
			//LOG(E_LOG_TRACE, "period[%d]: %d", i, periods[i]);
		}
		float fAvgPeriod = fPeriodSum / periods.size();
#if 0
		// try to exclude false positives,
		// i.e. "periods" that are much smaller than average
		// that are caused by noise
		int falsePeriods = 0;
		for (unsigned int i=0; i<periods.size(); ++i) {
			if (periods[i] < 0.2 * fAvgPeriod) {
				//LOG(E_LOG_TRACE, "Excluded period[%d]: %d", i, periods[i]);
				falsePeriods++;
			}
		}
		// we are not changing total sum of periods
		if (periods.size() - falsePeriods >= 3)
		{
			fAvgPeriod = fPeriodSum / (periods.size() - falsePeriods);
			fFreq = 1.0 / (fAvgPeriod * fDivX);
			bFreqValid = true;
		}
#else
		float fDivX;
		if (data.GetMode() == MODE_RECORDER)
			fDivX = data.fRecorderInterval;
		else
			fDivX = data.fOscDivX;
		fFreq = 1.0 / (fAvgPeriod * fDivX);
		bFreqValid = true;
#endif
	}
}
