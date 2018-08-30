/** \file
	\brief FFT window: Blackman-Harris
*/

#include "FFT/FFTWindow.h"
#include "FFT/FFTWindowFactory.h"
#include "FFT/FFTWindowDescription.h"
#include <math.h>
#include "gnugettext.hpp"

namespace
{
	FFTWindowDescription desc(
		((AnsiString)_("Blackman-Harris")).c_str(),
		((AnsiString)_("Best in spectral leakage category, poor frequency resolution")).c_str(),
		1.66f);

	class FFTWindowBlackmanHarris: public FFTWindow
	{
	private:
		friend FFTWindow* CreateFFTWindow(void);
		FFTWindowBlackmanHarris(const FFTWindowDescription &desc):
			FFTWindow(desc)
		{};
	public:
		virtual void Resize(unsigned int size);
		virtual void Process(float *data);
	};

	FFTWindow* CreateFFTWindow(void)
	{
		return new FFTWindowBlackmanHarris(desc);
	}

	const bool registered =
		FFTWindowFactory::Instance().Register(desc, CreateFFTWindow);
}

void FFTWindowBlackmanHarris::Resize(unsigned int size)
{
	weights.resize(size);
	// original coefficients: 0.423, –0.497, 0.079
	static const float alpha = 2.0f * M_PI / size;
	static const float alpha2 = 2.0f * alpha;
	for (unsigned i = 0; i < size; ++i)
		weights[i] = 1.0f - (1.183f * cos(i*alpha)) + (0.188f * cos(i*alpha2));
}

void FFTWindowBlackmanHarris::Process(float *data)
{
    unsigned int size = weights.size();
	for (unsigned i = 0; i < size; ++i)
		data[i] *= weights[i];
}


