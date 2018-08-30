/** \file
	\brief Common description of FFT window
*/

#ifndef FFTWindowDescriptionH
#define FFTWindowDescriptionH

#include <string>

/** \brief Description/ID of FFT window
*/
class FFTWindowDescription
{
public:
	FFTWindowDescription(std::string name, std::string desc, float fLobeWidth):
		name(name), desc(desc), fLobeWidth(fLobeWidth)
	{}
	std::string name;	///< displayed name; used as class identifier - must be
						///< unique
	std::string desc;	///< full description
	float fLobeWidth;	///< main lobe width (-3dB, unit = FFT bin), used to sort windows

	bool operator<(const FFTWindowDescription &other) const
	{
	   return name < other.name;
	}
};

#endif
