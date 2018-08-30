/** \file
*/

#pragma warn -8091	// incorrectly issued by BDS2006

#include "FFTWindowFactory.h"
#include <algorithm>

FFTWindowFactory* FFTWindowFactory::fftwindowFactory = NULL;

bool FFTWindowFactory::Register(const FFTWindowDescription &desc, Callback cb)
{
	MapWindow::const_iterator i = windows.find(desc);
	if (i != windows.end())
	{
		throw std::runtime_error("Duplicated FFT window name!");
	}
	return windows.insert(MapWindow::value_type(desc, cb)).second;
}

bool CmpWindow(const FFTWindowDescription* wnd1, const FFTWindowDescription* wnd2)
{
	return wnd1->fLobeWidth < wnd2->fLobeWidth;
}

void FFTWindowFactory::GetRegisteredList(std::vector<const FFTWindowDescription*> &descriptions)
{
	descriptions.clear();
	descriptions.reserve(windows.size());
	MapWindow::iterator it;
	for (it = windows.begin(); it != windows.end(); ++it)
	{
		descriptions.push_back(&it->first);
	}
	// sort descriptions by main lobe width
	std::sort(descriptions.begin(), descriptions.end(), CmpWindow);
}

bool FFTWindowFactory::Unregister(const std::string &name)
{
	FFTWindowDescription desc(name, "", 0);
	MapWindow::const_iterator i = windows.find(desc);
	if (i == windows.end())
		return false;
	windows.erase(i->first);
	return true;
}

FFTWindow* FFTWindowFactory::Create(const std::string &name)
{
	FFTWindowDescription desc(name, "", 0);
	MapWindow::const_iterator i = windows.find(desc);
	if (i == windows.end())
		return NULL;
	// invoke object create callback
	return (i->second)();
}



