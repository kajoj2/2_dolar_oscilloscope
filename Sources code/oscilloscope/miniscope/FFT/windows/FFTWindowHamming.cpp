/** \file
	\brief FFT window: Hamming
*/

#include "FFT/FFTWindow.h"
#include "FFT/FFTWindowFactory.h"
#include "FFT/FFTWindowDescription.h"
#include <math.h>
#include "gnugettext.hpp"

namespace
{
	FFTWindowDescription desc(
		((AnsiString)_("Hamming")).c_str(),
		((AnsiString)_("Preferred for closely spaced sine waves (next to rectangular window).")).c_str(),
		1.30f);

	class FFTWindowHamming: public FFTWindow
	{
	private:
		friend FFTWindow* CreateFFTWindow(void);
		FFTWindowHamming(const FFTWindowDescription &desc):
			FFTWindow(desc)
		{};
	public:
		virtual void Resize(unsigned int size);
		virtual void Process(float *data);
	};

	FFTWindow* CreateFFTWindow(void)
	{
		return new FFTWindowHamming(desc);
	}

	const bool registered =
		FFTWindowFactory::Instance().Register(desc, CreateFFTWindow);
}

void FFTWindowHamming::Resize(unsigned int size)
{
	weights.resize(size);
	static const float alpha = 2.0f * M_PI / size;
	for (unsigned i = 0; i < size; ++i)
	{
		// amplitude coherence gain compensation (0.54) included in calculation
		// original equation: 0.54 - 0.46 * cos(i*alpha)
		weights[i] = 1.0f - (0.85f * cos(i*alpha));
	}
}

void FFTWindowHamming::Process(float *data)
{
    unsigned int size = weights.size();
	for (unsigned i = 0; i < size; ++i)
		data[i] *= weights[i];
}


