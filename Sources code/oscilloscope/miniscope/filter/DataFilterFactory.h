/** \file
	\brief DataFilter object factory
*/

#ifndef DataFilterFactoryH
#define DataFilterFactoryH

#include <string>
#include <map>
#include <vector>

class DataFilter;

/** Factory for data filters, singleton.
*/
class DataFilterFactory
{
private:
	/** This factory is a singleton
	*/
	DataFilterFactory() {};
	//virtual ~DataFilterFactory() {};
	DataFilterFactory(const DataFilterFactory& source) {};
	DataFilterFactory& operator=(const DataFilterFactory&);

	/** \brief Callback type definition for creating concrete object
	*/
	typedef DataFilter* (*Callback)();
	/** \brief Map type associating filter name with callback to create specific
		filter
	*/
	typedef std::map<std::string, Callback> MapFilter;

	/** \brief Actual map with filter names and creation callbacks
	*/
	MapFilter filters;
public:
	/** Get reference to factory (singleton) instance
	*/
	static DataFilterFactory& Instance(void)
	{
		static DataFilterFactory dataFilterFactory;
		return dataFilterFactory;
	}
	/** Register new filter type
		\param name Name of the filter, must be unique or exception is thrown
		\param cb Callback to create specific filter
		\return True on success
	*/
	bool Register(const std::string &name, Callback cb);
	/** Unregister filter type with specified name from factory. We'll probably
		won't use this.
		\param name Filter name
		\return True on success
	*/
	bool Unregister(const std::string &name);
	/** Create filter specified with name
		\name Name of filter to create
		\return Pointer to DataFilter class object or NULL
	*/
	DataFilter* Create(const std::string &name);
	/** Get list of names of the registered filters
	*/
	void GetRegisteredList(std::vector<std::string> &names);
};

#endif
