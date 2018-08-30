/** \file
*/

#include "filter/DataFilterChain.h"
#include "filter/DataFilter.h"
#include "filter/DataFilterFactory.h"

void DataFilterChain::Configure(const std::vector<std::string> &names)
{
	Release();
	for (unsigned int i=0; i<names.size(); i++)
	{
		chain.push_back( DataFilterFactory::Instance().Create(names[i]) );
	}
}

void DataFilterChain::Process(std::vector<float> &data)
{
	for (unsigned int i=0; i<chain.size(); i++)
	{
		chain[i]->Process(data);
	}
}

void DataFilterChain::Release(void)
{
	for (unsigned int i=0; i<chain.size(); i++)
	{
		delete chain[i];
	}
	chain.clear();
}
