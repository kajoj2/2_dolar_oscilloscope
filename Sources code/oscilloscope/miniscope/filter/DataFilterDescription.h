/** \file
*/

#ifndef DataFilterDescriptionH
#define DataFilterDescriptionH

#include <string>

/** \brief Description/ID of data processing filter
*/
class DataFilterDescription
{
public:
	DataFilterDescription(std::string name, std::string desc):
		name(name), desc(desc)
	{}
	std::string name;	///< displayed name; used as class identifier - must be
						///< unique
	std::string desc;	///< full description
};

#endif
