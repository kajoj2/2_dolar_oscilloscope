/** \file
 *  \brief Routines for physical values string formatting
 *
 *  Strings are automatically converted to multiplications of base
 *  unit (mV, kHz) to make them shorter.
 */

//---------------------------------------------------------------------------

#ifndef ValueStringFormatterH
#define ValueStringFormatterH
//---------------------------------------------------------------------------
#include <string>

/** \brief Utility class - value to string conversion
*/
class ValueStringFormatter
{
public:
    /** \brief Convert float value to string representing voltage
    */
	static std::string VoltageToStr(float val, int precision);
	/** \brief Convert float value to string representing time
	*/
	static std::string TimeToStr(float val, int precision, bool precision_absolute = true);
	/** \brief Convert float value to string representing frequency
	*/
	static std::string FreqToStr(float val, int precision);
	/** \brief Convert float value to string
		\param val input value
		\param precision requested conversion precision
		\param precision_absolute if set to true precision means number of decimal fraction
		part digits, alse precision means requested number of digits to round input value
	*/
	static std::string ValToStr(float val, int precision, bool precision_absolute = true);
private:
};

#endif
