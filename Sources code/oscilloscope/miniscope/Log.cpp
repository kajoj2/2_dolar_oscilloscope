/** \file
 *  \brief Global logging unit
 */

/* Copyright (C) 2008-2009 Tomasz Ostrowski <tomasz.o.ostrowski at gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.

 * Miniscope is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <time.h>
#include <sys\timeb.h>
#include "Log.h"



CLog::CLog():
	callbackLog(NULL),
	sFile(""),
	bLogToFile(false),
	iLogLevel(0)
{
};

int CLog::SetFile(std::string file)
{
	sFile = file;
	iLogLevel = E_LOG_NONE;
	FILE *fout = fopen(sFile.c_str(),"at+");
	if (fout == NULL)
	{
		return -1;
	}
	fclose(fout);	
	return 0;
}

void CLog::LogToFile(bool state)
{
	bLogToFile = state;
}

void CLog::SetLevel(int level)
{
	iLogLevel = level;
}

void CLog::log(int level, char *lpData, ...)
{
	if (level > iLogLevel)
		return;
	if (sFile == "")
		return;

	char buf[1024]; //determines max message length

	struct timeb timebuffer;
	ftime( &timebuffer );
	/*
	After looking inside RTL sources it seems that this is thread-safe (when linking
	with MT version).
	*/
	int size = strftime(buf, sizeof(buf), "%Y-%m-%d %T", localtime(&timebuffer.time));
	int res = snprintf(buf+size, sizeof(buf)-size, ".%03hu ", timebuffer.millitm);
	buf[sizeof(buf)-1] = '\0';
	size += res;

	if ((int)sizeof(buf)-size-2 > 0)
	{
		va_list ap;
		va_start(ap, lpData);
		size += vsnprintf(buf + size, sizeof(buf)-size-2, lpData, ap);
		va_end(ap);
	}
	if (size > (int)sizeof(buf) - 2)
		size = sizeof(buf) - 2;
	buf[size] = '\n';
	buf[size+1] = 0;

	if (bLogToFile)
	{
		FILE* fout = fopen(sFile.c_str(),"at+");
		if (fout)
		{
			fwrite(buf, size + 1, 1, fout);
			fclose(fout);
		}
	}

	if (callbackLog)
		callbackLog(buf);
}

