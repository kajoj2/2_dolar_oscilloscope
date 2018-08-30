/** \file
	\brief Data filter: inverting value of signal
*/

#include "filter/DataFilter.h"
#include "filter/DataFilterFactory.h"
#include "filter/DataFilterDescription.h"
#include <math.h>
#include "gnugettext.hpp"


namespace
{
	DataFilterDescription desc("Invert", ((AnsiString)_("Inverted polarity of signal")).c_str());

	class DataFilterInvert: public DataFilter
	{
	private:
		friend DataFilter* CreateFilter(void);
		DataFilterInvert(const DataFilterDescription &desc):
			DataFilter(desc)
		{};
	public:
		virtual void Process(std::vector<float> &data);
	};

	DataFilter* CreateFilter(void)
	{
		return new DataFilterInvert(desc);
	}

	const bool registered =
		DataFilterFactory::Instance().Register(desc.name, CreateFilter);
}

void DataFilterInvert::Process(std::vector<float> &data)
{
	for (unsigned int i=0; i<data.size(); i++)
	{
    	data[i] = -data[i];
	}
}


