/** \file
*/

#include "DataFilterFactory.h"

bool DataFilterFactory::Register(const std::string &name, Callback cb)
{
	MapFilter::const_iterator i = filters.find(name);
	if (i != filters.end())
	{
		throw std::runtime_error("Duplicated data filter name!");
	}
	return filters.insert(MapFilter::value_type(name, cb)).second;
}

void DataFilterFactory::GetRegisteredList(std::vector<std::string> &names)
{
	names.clear();
	names.resize(filters.size());
	MapFilter::iterator it;
	int i=0;
	for (it = filters.begin(); it != filters.end(); ++it)
	{
    	names[i++] = it->first;
	}
}

bool DataFilterFactory::Unregister(const std::string &name)
{
	return filters.erase(name) == 1;
}

DataFilter* DataFilterFactory::Create(const std::string &name)
{
	MapFilter::const_iterator i = filters.find(name);
	if (i == filters.end())
		return NULL;
	// invoke object create callback
	return (i->second)();
}



