/** \file
	\brief Interface for FFT window classes
*/

#ifndef FFTWindowH
#define FFTWindowH

#include "FFT/FFTWindowDescription.h"
#include <vector>

/** \brief Interface for FFT window classes
*/
class FFTWindow
{
protected:
	FFTWindow(const FFTWindowDescription &desc):
		desc(desc)
	{};
	const FFTWindowDescription &desc;
	std::vector<float> weights;
	
public:
	/** Prepare for processing - set samples count
		\param size samples count to be processed
	*/
	virtual void Resize(unsigned int size) = 0;
	/** Process samples
	*/
	virtual void Process(float *data) = 0;
	virtual ~FFTWindow() {};

	/** Tell about yourself
	*/
	virtual const FFTWindowDescription& GetDescription() {
		return desc;
	}
};

#endif