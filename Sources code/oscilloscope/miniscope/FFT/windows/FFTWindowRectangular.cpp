/** \file
	\brief FFT window: rectangular (no window)
*/

#include "FFT/FFTWindow.h"
#include "FFT/FFTWindowFactory.h"
#include "FFT/FFTWindowDescription.h"
#include "gnugettext.hpp"

namespace
{
	FFTWindowDescription desc(
		((AnsiString)_("Rectangular")).c_str(),
		((AnsiString)_("Rectangular window (or no window) is preferred for broadband random "
		"(white noise), transient or synchrounously samples signals. Best frequency resolution, worst spectrum leakage.")).c_str(),
		0.88f);

	class FFTWindowRectangular: public FFTWindow
	{
	private:
		friend FFTWindow* CreateFFTWindow(void);
		FFTWindowRectangular(const FFTWindowDescription &desc):
			FFTWindow(desc)
		{};
	public:
		virtual void Resize(unsigned int size);
		virtual void Process(float *data);
	};

	FFTWindow* CreateFFTWindow(void)
	{
		return new FFTWindowRectangular(desc);
	}

	const bool registered =
		FFTWindowFactory::Instance().Register(desc, CreateFFTWindow);
}

void FFTWindowRectangular::Resize(unsigned int size)
{

}

void FFTWindowRectangular::Process(float *data)
{

}


