/** \file
	\brief Data filter: absolute value of signal
*/

#include "filter/DataFilter.h"
#include "filter/DataFilterFactory.h"
#include "filter/DataFilterDescription.h"
#include <math.h>
#include "gnugettext.hpp"
#include <System.hpp>


namespace
{
	DataFilterDescription desc("Abs", ((AnsiString)_("Absolute value of signal")).c_str());

	class DataFilterAbs: public DataFilter
	{
	private:
		friend DataFilter* CreateFilter(void);
		DataFilterAbs(const DataFilterDescription &desc):
			DataFilter(desc)
		{};
	public:
		virtual void Process(std::vector<float> &data);
	};

	DataFilter* CreateFilter(void)
	{
		return new DataFilterAbs(desc);
	}

	const bool registered =
		DataFilterFactory::Instance().Register(desc.name, CreateFilter);
}

void DataFilterAbs::Process(std::vector<float> &data)
{
	for (unsigned int i=0; i<data.size(); i++)
	{
    	data[i] = fabs(data[i]);
	}
}


