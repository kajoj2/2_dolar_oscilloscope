#ifndef ModeH
#define ModeH

/** \brief Application/display mode
*/
enum E_MODE
{
	MODE_OSCILLOSCOPE = 0,  ///< plot versus time
	MODE_SPECTRUM,          ///< plot versus frequency
	MODE_RECORDER			///< plot versus time, slow changing signal recorder
};

extern enum E_MODE mode;

#endif
