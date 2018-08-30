/** \file Data.h
    \brief Acquired oscilloscope data
*/

#ifndef DataH
#define DataH

#include "Calibration.h"
#include "Mode.h"
#include <sys\timeb.h>
#include <vector>

class Data
{
public:
	Data();
	int GetBitsPerSample (void) {
		return iBitsPerSample;
	}
    unsigned int GetChannelCount (void) const {
        return iChannelCount;
	}
	unsigned int GetEnabledChannelCount(void) const;
	void SetBitsPerSample(int bits)
	{
    	iBitsPerSample = bits;
	}
	/** \brief Allocate/re-initialize all channels */
	void SetChannelCount(int ch_count);

	struct Channel {
		Channel():
			id(0),
			bEnabled(true),
			fDivY(0),
			bDivYChanged(false),
			iCouplingType(0)
		{
		}
		int id;
		bool bEnabled;							///< de-/activating channel
		float fDivY;
		bool bDivYChanged;
		struct DEV_CALIBRATION calibration;
		int iCouplingType;						///< coupling type
		std::vector<float> dataOsc;             ///< data container used in oscilloscope and slow signal recorder modes
		std::vector<float> dataFFT;             ///< data container used in spectrum analyzer mode
		std::vector<float> dataRec;             ///< data container used in slow changing signal recorder mode
		Channel& operator=(const Channel& src)
		{
			// copies everything but dataRec and dataFFT
			this->id = src.id;
			this->bEnabled = src.bEnabled;
			this->fDivY = src.fDivY;
			this->bDivYChanged = src.bDivYChanged;
			this->calibration = src.calibration;
			this->iCouplingType = src.iCouplingType;
			this->dataOsc = src.dataOsc;
			return *this;
		}
	};
	float fOscDivX;								///< sampling interval (oscilloscope mode)
	float fRecorderInterval;
	struct timeb timestamp;
	int trigger_mode;
	int trigger_source;
	int trigger_type;
	unsigned int buffer_size;
	
	bool bFrameFilled;

private:
	int iBitsPerSample;
	int iChannelCount;
	std::vector<Channel> channels;
	enum E_MODE mode;

public:
	struct Channel& getChannel(int id)
	{
		return channels[id];
	}
	struct Channel& getEnabledChannel(void);
	const struct Channel& getEnabledChannelConst(void) const;
	const struct Channel& getChannelConst(int id) const
	{
		return channels[id];
	}
	void SetMode(enum E_MODE mode)
	{
		this->mode = mode;
	}
	enum E_MODE GetMode(void) const
    {
    	return mode;
	}
};

#endif // DataH
