/** \file
	\brief Data filter: multiplying signal values by itself
*/

#include "filter/DataFilter.h"
#include "filter/DataFilterFactory.h"
#include "filter/DataFilterDescription.h"
#include <math.h>
#include "gnugettext.hpp"


namespace
{
	DataFilterDescription desc("Square", ((AnsiString)_("Signal value square")).c_str());

	class DataFilterSquare: public DataFilter
	{
	private:
		friend DataFilter* CreateFilter(void);
		DataFilterSquare(const DataFilterDescription &desc):
			DataFilter(desc)
		{};
	public:
		virtual void Process(std::vector<float> &data);
	};

	DataFilter* CreateFilter(void)
	{
		return new DataFilterSquare(desc);
	}

	const bool registered =
		DataFilterFactory::Instance().Register(desc.name, CreateFilter);
}

void DataFilterSquare::Process(std::vector<float> &data)
{
	for (unsigned int i=0; i<data.size(); i++)
	{
    	data[i] = data[i] * data[i];
	}
}


