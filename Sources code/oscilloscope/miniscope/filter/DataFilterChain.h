/** \file
	\brief Cascade filter chain
*/

#ifndef DataFilterChainH
#define DataFilterChainH

#include <vector>
#include <string>

class DataFilter;

class DataFilterChain
{
private:
	std::vector<DataFilter*> chain;
public:
	void Configure(const std::vector<std::string> &names);
	void Process(std::vector<float> &data);
	void Release(void);
};

#endif
