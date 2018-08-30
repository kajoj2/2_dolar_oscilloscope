/** \file
	\brief FFT window factory
*/
#ifndef FFTWindowFactoryH
#define FFTWindowFactoryH

#include <string>
#include <map>
#include <vector>

#include "FFT/FFTWindowDescription.h"

class FFTWindow;

/** Factory for FFT windows, singleton.
*/
class FFTWindowFactory
{
private:
	static FFTWindowFactory* fftwindowFactory;
	/** This factory is a singleton
	*/
	FFTWindowFactory() {};
	FFTWindowFactory(const FFTWindowFactory& source) {};
	FFTWindowFactory& operator=(const FFTWindowFactory&);

	/** \brief Callback type definition for creating concrete object
	*/
	typedef FFTWindow* (*Callback)();
	/** \brief Map type associating window name with callback to create specific
		window
	*/
	typedef std::map<FFTWindowDescription, Callback> MapWindow;

	/** \brief Actual map with window names and creation callbacks
	*/
	MapWindow windows;

public:
	/** Get reference to factory (singleton) instance
	*/
	static FFTWindowFactory& Instance(void)
	{
		/** I would prefer singleton with static instance here,
			but CG has some problems with it, incorrectly
			reporting memory leaks from time to time.
		*/
		if (fftwindowFactory == NULL)
			fftwindowFactory = new FFTWindowFactory();
		return *fftwindowFactory;
	}
	static void Destroy(void)
	{
    	delete fftwindowFactory;
	}
	/** Register new window type
		\param name Name of the window, must be unique or exception is thrown
		\param cb Callback to create specific window
		\return True on success
	*/
	bool Register(const FFTWindowDescription &desc, Callback cb);
	/** Unregister window type with specified name from factory. We'll probably
		won't use this.
		\param name Window name
		\return True on success
	*/
	bool Unregister(const std::string &name);
	/** Create window specified with name
		\name Name of window to create
		\return Pointer to FFTWindow class object or NULL
	*/
	FFTWindow* Create(const std::string &name);
	/** Get list of names of the registered filters
	*/
	void GetRegisteredList(std::vector<const FFTWindowDescription*> &descriptions);
};


//---------------------------------------------------------------------------
#endif
