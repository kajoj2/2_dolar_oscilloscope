//---------------------------------------------------------------------------


#pragma hdrstop

#include "Utilities.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//i.e. GetFileVer(Application->ExeName)
AnsiString GetFileVer(AnsiString FileName)
{
    AnsiString asVer="";
    VS_FIXEDFILEINFO *pVsInfo;
    unsigned int iFileInfoSize = sizeof( VS_FIXEDFILEINFO );
    
    int iVerInfoSize = GetFileVersionInfoSize(FileName.c_str(), NULL);
    if (iVerInfoSize!= 0)
    {
        char *pBuf = new char[iVerInfoSize];
        if (GetFileVersionInfo(FileName.c_str(),0, iVerInfoSize, pBuf ) )
        {
            if (VerQueryValue(pBuf, "\\",(void **)&pVsInfo,&iFileInfoSize))
            {
				asVer.sprintf("%d.%02d.%02d.%02d",
					HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS),
					HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
			}
        }
        delete [] pBuf;
    }
    return asVer;
}

AnsiString GetChannelName(int channel_id)
{
	int alpha_cnt = 'Z' - 'A' + 1;
	AnsiString result;
	char str[2];
	str[1] = '\0';
	for(;;)
	{
		str[0] = 'A' + (channel_id % alpha_cnt);
		result = (AnsiString)str + result;
		channel_id /= alpha_cnt;
		if (channel_id == 0)
			break;
		channel_id--;
	}
	return result;
}
