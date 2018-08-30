/** \file
	\brief Information calculator for input data
*/

/** \page frequencymeasure Frequency measuring

Frequency measuring is done by counting moments when signal is crossing its
average value.\n
Algorithm requires capturing at least 3 periods of signal. If less is captured
flag bFreqValid will not be set and fFreq value will be undetermined.\n
When noise level in input signal is high there is a risk of detecting more
crossing points than expected. To avoid this simple hysteresis scheme is used:
	- difference between maximum (Vmax) and average (Vavg) and difference between
	average and minimum (Vmin) value is computed
	- if Vmax - Vavg < Vavg - Vmin:
		- hysteresis threshold (Vth) is calculated as Vavg - (Vavg - Vmin)/2
		- moments when signal is raising through Vavg will be counted
		- each raising through Vavg is taken into account only if signal falled
		earlier through Vth
	- if Vmax - Vavg >= Vavg - Vmin:
		- hysteresis threshold (Vth) is calculated as Vavg + (Vmax - Vavg)/2
		- moments when signal is falling through Vavg will be counted
		- each falling through Vavg is taken into account only if signal falleded
		earlier through Vth


    \image html measure_freq.gif  	
*/

#ifndef MeasurementsH
#define MeasurementsH

#include "DataSink.h"

class Data;

/** \brief Statistical information calculator
*/
class Measurements: public DataSink
{
public:
	Measurements():
		fMin(0),
		fMax(0),
		fAvg(0),
		fRms(0),
		fFreq(0),
		bFreqValid(false)
	{
	};
	virtual ~Measurements() {};
	//virtual void Trigger(void);
	virtual void Process(const Data &data, int channel);

	float fMin;	///< minimal value from data vector
	float fMax;	///< maximal value from data vector
	float fAvg;	///< averaged value from data vector samples
	float fRms;	///< RMS value
	float fFreq;///< estimated frequency of signal; may be difficult to determine
	bool bFreqValid;	///< is frequency value valid?
};

#endif
