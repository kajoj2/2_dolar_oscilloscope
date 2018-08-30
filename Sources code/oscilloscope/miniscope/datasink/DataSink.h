/** \file
	\brief Abstract data receiver interface
*/

#ifndef DataSinkH
#define DataSinkH

#include <vector>

class Data;

/** \brief Abstract data receiver
*/
class DataSink
{
public:
	virtual ~DataSink() {};
	/** Called on device trigger
	*/
	virtual void Trigger(void);
	/** Process data frame (acquisition buffer or part of if) received from device
		\param data input data
	*/
	virtual void Process(const Data &data, int channel);
};

#endif