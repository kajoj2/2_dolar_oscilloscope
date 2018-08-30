/** \file
 *  \brief Device capabilities specification. This file is shared by miniscope and device dll projects.
 */

#ifndef MiniscopeDeviceCapabilitiesH
#define MiniscopeDeviceCapabilitiesH

#include <string.h>

/** \note By default BCC is using 1-byte for small enums.
	Other compilers (gcc and msvc) seems to prefer 4 bytes size.
	For compatibility with MinGW/gcc use -fshort-enums flag when compiling.
	\todo Change enum size to 4 bytes if dll interface will change.
*/
#ifdef _MSC_VER
enum E_TRIGGER_MODE : unsigned char
#else
enum E_TRIGGER_MODE
#endif
{
	TRIGGER_MODE_SINGLE = 0,
	TRIGGER_MODE_NORMAL,
	TRIGGER_MODE_AUTO,
	TRIGGER_MODE_CONTINUOUS
};

/** \brief Supported triggering modes
 */
#ifdef _MSC_VER
enum E_TRIGGER_TYPE : unsigned char
#else
enum E_TRIGGER_TYPE
#endif
{
	TRIGGER_SLOPE_UP = 0,       ///< raising slope
	TRIGGER_SLOPE_DOWN			///< falling slope
};

/** \brief Supported input coupling modes
 */
#ifdef _MSC_VER
enum E_COUPLING_TYPE : unsigned char
#else
enum E_COUPLING_TYPE
#endif
{
    COUPLING_GND = 0,       ///< input tied to ground
    COUPLING_DC,            ///< direct current coupling
    COUPLING_AC             ///< DC path blocked
};

/** \brief Device capabilities structure
*/
struct S_SCOPE_CAPABILITIES
{
    unsigned int iChannelCount;			///< total channels count
	unsigned int iBitsPerSample;        ///< number of bits per data sample
    unsigned int iSensitivityLevelsCount;   ///< number of available sensitivity level settings
	float *fSensitivityLevels;          ///< ptr to array of sensitivity per LSB values (array size defined by iSensitivityLevelsCount)
										///< values must be ascending
    unsigned int iSamplingPeriodCount;  ///< number of available sampling period time settings
    float *fSamplingPeriod;      		///< ptr to sampling period array [s]

	unsigned int iTriggerModeCount;
	E_TRIGGER_MODE *iTriggerModes;

	unsigned int iTriggerTypeCount;     ///< number of available trigger types
	E_TRIGGER_TYPE *iTriggerTypes; ///< ptr to trigger types array

	unsigned int iCouplingTypeCount;   	///< number of available coupling modes
	E_COUPLING_TYPE *iCouplingType;///< ptr to available coupling modes array

	unsigned int iBufferSizeCount;      ///< number of selectable acquisition buffer sizes
	unsigned int *iBufferSizes;         ///< ptr to acquisition buffer sizes array
};

/** \brief Device settings structure
*/
struct S_SCOPE_SETTINGS
{
	unsigned int iChannelCount;			///< required by copy operator
	bool* pChannelEnable;				///< channel state (active/inactive) array
	unsigned int* pSensitivity;			///< sensitivity array (sensitivity for each channel)
	unsigned int iSamplingPeriod;
	E_TRIGGER_MODE eTriggerMode;
	int iTriggerSrcChannel;
	E_TRIGGER_TYPE eTriggerType;
	int iTriggerLevel;
	int iTriggerPosition;
	E_COUPLING_TYPE* eCouplingType;
	unsigned int iBufferSize;

	S_SCOPE_SETTINGS& operator=(const S_SCOPE_SETTINGS& src)
	{
		this->iChannelCount = src.iChannelCount;
		memcpy(this->pChannelEnable, src.pChannelEnable, this->iChannelCount * sizeof(this->pChannelEnable[0]));
		memcpy(this->pSensitivity, src.pSensitivity, this->iChannelCount * sizeof(this->pSensitivity[0]));
		this->iSamplingPeriod = src.iSamplingPeriod;
        this->eTriggerMode = src.eTriggerMode;
		this->iTriggerSrcChannel = src.iTriggerSrcChannel;
		this->eTriggerType = src.eTriggerType;
		this->iTriggerLevel = src.iTriggerLevel;
		this->iTriggerPosition = src.iTriggerPosition;
		memcpy(this->eCouplingType, src.eCouplingType, this->iChannelCount * sizeof(this->eCouplingType[0]));
		this->iBufferSize = src.iBufferSize;
		return *this;
	}
};

#endif // MiniscopeDeviceCapabilitiesH
