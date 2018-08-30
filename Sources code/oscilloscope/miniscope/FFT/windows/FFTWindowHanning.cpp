/** \file
	\brief FFT window: Hanning
*/

#include "FFT/FFTWindow.h"
#include "FFT/FFTWindowFactory.h"
#include "FFT/FFTWindowDescription.h"
#include <math.h>
#include "gnugettext.hpp"

namespace
{
	FFTWindowDescription desc(
		((AnsiString)_("Hanning")).c_str(),
		((AnsiString)_("Prefered for unknown content, sine wave and combination of sine waves.")).c_str(),
		1.44f);

	class FFTWindowHanning: public FFTWindow
	{
	private:
		friend FFTWindow* CreateFFTWindow(void);
		FFTWindowHanning(const FFTWindowDescription &desc):
			FFTWindow(desc)
		{};
	public:
		virtual void Resize(unsigned int size);
		virtual void Process(float *data);
	};

	FFTWindow* CreateFFTWindow(void)
	{
		return new FFTWindowHanning(desc);
	}

	const bool registered =
		FFTWindowFactory::Instance().Register(desc, CreateFFTWindow);
}

void FFTWindowHanning::Resize(unsigned int size)
{
	weights.resize(size);
	static const float alpha = 2.0f * M_PI / size;
	for (unsigned i = 0; i < size; ++i)
	{
		// amplitude coherence gain compensation (0.5) included in calculation
		// original equation: 0.5 - 0.5 * cos(i*alpha)
		weights[i] = 1.0f - cos(i*alpha);
	}
}

void FFTWindowHanning::Process(float *data)
{
    unsigned int size = weights.size();
	for (unsigned i = 0; i < size; ++i)
		data[i] *= weights[i];
}


