/** \file
	\brief Data processing filter interface
*/

#ifndef DataFilterH
#define DataFilterH

#include <vector>
#include "filter/DataFilterDescription.h"

/** \brief Interface for data filters
*/
class DataFilter
{
protected:
	DataFilter(const DataFilterDescription &desc):
		desc(desc)
	{};
	const DataFilterDescription &desc;
	
public:
	/** Process samples
	*/
	virtual void Process(std::vector<float> &data) = 0;
	virtual ~DataFilter() {};

	/** Tell about yourself
	*/
	virtual const DataFilterDescription& GetDescription() {
		return desc;
	}
};

#endif
