/** \file
	\brief FFT window: Flat Top
*/

#include "FFT/FFTWindow.h"
#include "FFT/FFTWindowFactory.h"
#include "FFT/FFTWindowDescription.h"
#include <math.h>
#include "gnugettext.hpp"

namespace
{
	FFTWindowDescription desc(
		((AnsiString)_("Flat top")).c_str(),	//FFT window name, I don't know if this should be translated
		((AnsiString)_("Best in ampliture accuracy, poor frequency resolution. Use for sinusoidal signals.")).c_str(),
		2.94f);

	class FFTWindowFlatTop: public FFTWindow
	{
	private:
		friend FFTWindow* CreateFFTWindow(void);
		FFTWindowFlatTop(const FFTWindowDescription &desc):
			FFTWindow(desc)
		{};
	public:
		virtual void Resize(unsigned int size);
		virtual void Process(float *data);
	};

	FFTWindow* CreateFFTWindow(void)
	{
		return new FFTWindowFlatTop(desc);
	}

	const bool registered =
		FFTWindowFactory::Instance().Register(desc, CreateFFTWindow);
}

void FFTWindowFlatTop::Resize(unsigned int size)
{
	weights.resize(size);
	// original coefficients: 0.281 –0.521 0.198
	static const float alpha = 2.0f * M_PI / size;
	static const float alpha2 = 2.0f * alpha;
	for (unsigned i = 0; i < size; ++i)
		weights[i] = 1.0f - (1.834f * cos(i*alpha)) + (0.705f * cos(i*alpha2));
}

void FFTWindowFlatTop::Process(float *data)
{
    unsigned int size = weights.size();
	for (unsigned i = 0; i < size; ++i)
		data[i] *= weights[i];
}


