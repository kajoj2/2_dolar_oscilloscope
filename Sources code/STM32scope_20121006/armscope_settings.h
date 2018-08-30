/** \file
	\brief Armscope settings structires
*/

#ifndef ARMSCOPE_SETTINGS_H
#define ARMSCOPE_SETTINGS_H

#include <inttypes.h>

enum 
{
	/** Maximum sampling frequency available */
	E_ARMSCOPE_SAMPLING_MAX = 500000
};

enum E_ARMSCOPE_TRIGGER_TYPE
{
	ARMSCOPE_TRIGGER_MANUAL = 0,	///< triggered by receiving special command only (no internal trigger)
	ARMSCOPE_TRIGGER_SLOPE_RAISING,	///< triggered on raising slope of signal
	ARMSCOPE_TRIGGER_SLOPE_FALLING	///< triggered on falling slope of signal
};

enum E_ARMSCOPE_COUPLING_TYPE
{
	ARMSCOPE_COUPLING_DC = 0,		///< direct current analog front-end coupling
	ARMSCOPE_COUPLING_AC			///< alternative current analog front-end coupling
};

/** \brief Armscope settings */
struct S_ARMSCOPE_SETTINGS
{
	/** \brief Used trigger type. */
	enum E_ARMSCOPE_TRIGGER_TYPE trigger_type;
	/** \brief Coupling type for analog front-end. */
	enum E_ARMSCOPE_COUPLING_TYPE coupling_type;
	/** \brief Sampling frequency. */
	uint32_t sampling_freq;
	/** \brief Voltage value for trigger */
	uint8_t trigger_level;
	/** \brief Sensitivity, depends on analog front-end. */
	uint8_t sensitivity;
	/** \brief Number of samples that are captured every trigger */
	uint16_t capture_buf_size;
};

#endif
