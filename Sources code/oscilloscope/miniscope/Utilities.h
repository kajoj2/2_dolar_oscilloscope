//---------------------------------------------------------------------------

#ifndef UtilitiesH
#define UtilitiesH
//---------------------------------------------------------------------------

#include <System.hpp>

/** Get version (windows/resource based) of specified file
*/
AnsiString GetFileVer(AnsiString FileName);

/** Get symbolic name for specified channel
*/
AnsiString GetChannelName(int channel_id);


#endif
