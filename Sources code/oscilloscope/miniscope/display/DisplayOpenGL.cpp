/** \file DisplayOpenGL.cpp
	\brief Display implementation based on OpenGL renderer
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

#include <vcl.h>

#pragma hdrstop

#include "DisplayOpenGL.h"
#include "ValueStringFormatter.h"
#include "device/Data.h"
#include "TimeCounter.h"
#include "Utilities.h"
#include <stdio.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <algorithm>
#include <assert.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

#define MYGL_COLORS(color) GetRValue(color)/256.0, GetGValue(color)/256.0, GetBValue(color)/256.0

const float DisplayOpenGL::fHeight = 1.0;

void DisplayOpenGL::SetVsync(bool enabled)
{
    typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
    PFNWGLSWAPINTERVALEXTPROC wglSwapInterval;
    wglSwapInterval = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
    //sets the number of vert retraces before redrawing
    if ( wglSwapInterval )
        wglSwapInterval(enabled ? 1 : 0);
}

void DisplayOpenGL::CreateBmpFont(void)					 
{
	HFONT	font;						// Windows Font ID
	HFONT	oldfont;
	base = glGenLists(96);				// Storage For 96 Characters

	font = CreateFont(	-12,			// Height Of Font
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_BOLD,							// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
		"Courier New");					// Font Name
	oldfont = (HFONT)SelectObject(this->hdc, font);
	wglUseFontBitmaps(this->hdc, 32, 96, base);
	SelectObject(this->hdc, oldfont);
	DeleteObject(font);
}

void DisplayOpenGL::DestroyFont(void)
{
	glDeleteLists(base, 96);	// delete char list
}

void DisplayOpenGL::Print(const char *fmt, ...)
{
	char buf[128];
	va_list	ap;
	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf)-1, fmt, ap);
	va_end(ap);
	buf[sizeof(buf)-1] = NULL;

	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(buf), GL_UNSIGNED_BYTE, buf);
    glPopAttrib();										
}

void DisplayOpenGL::PrintWithOutline(float x, float y,
	TColor color, TColor outline,
	const char *fmt, ...)
{
	float dx = State().fWidth/iWidth;
	float dy = 1.0f/iHeight;

	if (mode == MODE_OSCILLOSCOPE || mode == MODE_RECORDER)
	{
		dy *= 2.0f;
	}

	char buf[128];
	va_list	ap;
	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf)-1, fmt, ap);
	va_end(ap);
	buf[sizeof(buf)-1] = NULL;

	glColor3f(MYGL_COLORS(outline));
	glRasterPos2f(x-dx, y);
	Print("%s", buf);
	glRasterPos2f(x+dx, y);
	Print("%s", buf);
	glRasterPos2f(x, y - dy);
	Print("%s", buf);
	glRasterPos2f(x, y + dy);
	Print("%s", buf);

	glColor3f(MYGL_COLORS(color));
	glRasterPos2f(x, y);
	Print("%s", buf);
}

//---------------------------------------------------------------------------
DisplayOpenGL::DisplayOpenGL(Data* data, void* hParent):
	iWidth(100), iHeight(100), Display(data)
{
	//_control87(MCW_EM, MCW_EM);
	SetVsync(true);
	hdc = GetDC(hParent);
	SetPixelFormatDescriptor();
	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);
	SetupRC();
	CreateBmpFont();
}
//---------------------------------------------------------------------------

DisplayOpenGL::~DisplayOpenGL()
{
    //kolejnosc?
    ReleaseDC(0, hdc);
    wglMakeCurrent(hdc, NULL);
    wglDeleteContext(hrc);
}

void DisplayOpenGL::SetData(Data *data)
{
	Display::SetData(data);
	float newVal;
	bool updated = false;

	newVal = data->fOscDivX * iWidth / scope.fZoomX;
	if (scope.fWidth != newVal)
	{
		updated = true;
		scope.fWidth = newVal;
	}
	newVal = 0.5f/(data->fOscDivX * spectrum.fZoomX);
	if (spectrum.fWidth != newVal)
	{
		updated = true;
		spectrum.fWidth = newVal;
	}
	newVal = data->fRecorderInterval * iWidth / recorder.fZoomX;
	if (recorder.fWidth != newVal)
	{
		updated = true;
		recorder.fWidth = newVal;
	}
	ChangeSize(iWidth, iHeight);
	
	if (updated)
	{
		DisplayNotifyArgument arg;
		arg.type = DisplayNotifyArgument::SCALE;
		notifyObservers(&arg);
    }	
}

void DisplayOpenGL::ChangeSize(int iWidth, int iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // describes a transformation that produces a parallel projection
	glOrtho(State().fOffsetX,          		// left edge
		  State().fOffsetX+State().fWidth,  // right edge
		  -fHeight,					   		// bottom edge
		  fHeight,				    		// top edge
		  0.0, 1.0);             			// near and far clipping, planes
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);			// not using depth buffer
	// Displacement trick for exact pixelization
	// glTranslatef(0.375, 0.375, 0);

	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glEnable(GL_BLEND);

	// antialiasing
	//glEnable (GL_LINE_SMOOTH);
	//glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);

	//glLineWidth (1.5);

	this->iWidth = iWidth;
	this->iHeight = iHeight;
}

void DisplayOpenGL::SetMode(const enum E_MODE m)
{
	mode = m;
	ChangeSize(iWidth, iHeight);		
	Update(UPDATE_ALL);
	DisplayNotifyArgument arg;
	arg.type = DisplayNotifyArgument::SCALE;
	notifyObservers(&arg);
}

//---------------------------------------------------------------------------
void DisplayOpenGL::Update(enum E_UPDATE_LEVEL level)
{
	TColor colorGrid = (TColor)0x303030;	// BGR
	TColor colorLabel = (TColor)0xB0B0B0;
	TColor colorCursor1 = (TColor)0x0000FF;
	TColor colorCursor2 = (TColor)0xFF0000;
	TColor colorBackground = (TColor)0x000000;
	TColor colorTriggerPosMarker = (TColor)0xA0A0A0;
	TColor colorRangeLabel = (TColor)0xB0B0B0;

	//TimeCounter tc("DisplayOpenGL::Update");

	glClearColor(MYGL_COLORS(colorBackground), 0);

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(MYGL_COLORS(colorGrid));
	glEnable(GL_LINE_STIPPLE);         /* dashed lines */
	glLineStipple(1, 0xF0F0);
	glBegin(GL_LINES);
	if (visibility[GRID_Y])
	{
		// horizontal grid
		for (int i=0; i<9; i++)
		{
			glVertex2f(State().fOffsetX, fHeight*(i-4)/5.0f); // origin
			glVertex2f(State().fWidth+State().fOffsetX, fHeight*(i-4)/5.0f); // end
		}
	}
	if (visibility[GRID_X])
	{
		// vertical grid
		for (int i=1; i<=9; i++)
		{
			glVertex2f(State().fWidth*(i)/10.0f + State().fOffsetX, -fHeight); // origin
			glVertex2f(State().fWidth*(i)/10.0f + State().fOffsetX, fHeight); // end
		}
	}
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	glColor3f(MYGL_COLORS(colorLabel));
	std::string label;

	if (visibility[GRID_Y] && visibility[GRID_LABELS_Y])
	{
		// horizontal grid labels
		for (int i=-4; i<=4; i+=2)
		{
			glRasterPos2f(State().fWidth/40.0 + State().fOffsetX, fHeight*((float)i+0.07f)/5.0f);
			float fVal = State().channels[iCurrentChannel].fOffsetY +
				(State().channels[iCurrentChannel].fRange/State().channels[iCurrentChannel].fZoomY * i / 5.0f);
			label = ValueStringFormatter::VoltageToStr(fVal, 2);
			Print("%s", label.c_str());
		}
	}

	if (visibility[GRID_X] && visibility[GRID_LABELS_X])
	{
		// vertical grid labels
		for (int i=1; i<=9; i+=2)
		{
			glRasterPos2f(State().fWidth*((float)i+0.07f)/10.0f + State().fOffsetX, -fHeight*0.95f);
			float fVal;
			if (mode == MODE_OSCILLOSCOPE || mode == MODE_RECORDER)
			{
				fVal = State().fWidth*((float)i)/10.0f + State().fOffsetX;
				label = ValueStringFormatter::TimeToStr(fVal, 2);
			}
			else if (mode == MODE_SPECTRUM)
			{
				label = "";
				if (data->GetChannelCount() > 0)
				{
					Data::Channel& channel = data->getEnabledChannel();
					if (channel.dataOsc.size())
					{
						//fVal = (fWidth*((float)i)/10.0f + State().fOffsetX) / (fDivX*NearestPowerOfTwo(channel.dataOsc.size())*2.0f);
						fVal = State().fWidth*((float)i)/10.0f + State().fOffsetX;
						label = ValueStringFormatter::FreqToStr(fVal, 2);
					}
				}
			}
			Print("%s", label.c_str());
		}
	}

	// plot
	for (unsigned int channel_id=0; channel_id<data->GetChannelCount(); channel_id++)
	{
		Data::Channel& channel = data->getChannel(channel_id);
		if (channel.bEnabled == false)
		{
        	continue;
		}
		std::vector<float> *samples;
		if (mode == MODE_OSCILLOSCOPE)
			samples = &channel.dataOsc;
		else if (mode == MODE_RECORDER)
			samples = &channel.dataRec;
		else if (mode == MODE_SPECTRUM)
			samples = &channel.dataFFT;
		else
			assert(0);

		if (samples->size() == 0)
			continue;

		// get user defined or default color for channel trace
		TColor color = GetChannelColor(channel_id);
		glColor3f(((float)GetRValue(color))/255.0f,
			((float)GetGValue(color))/255.0f,
			((float)GetBValue(color))/255.0f);

		if (mode == MODE_OSCILLOSCOPE || mode == MODE_RECORDER)
		{
			glPushMatrix();
			glScalef(1.0, State().channels[channel_id].fZoomY/State().channels[channel_id].fRange, 1.0);
			glTranslatef(0.0, -1.0f * State().channels[channel_id].fOffsetY, 0.0);
			register float deltaX = (mode==MODE_OSCILLOSCOPE)?(data->fOscDivX):(data->fRecorderInterval);
			int i = std::max<float>(State().fOffsetX, 0.0f) / deltaX;
			float limit = std::min<float>(samples->size(), (State().fOffsetX + State().fWidth)/deltaX + i);
			register float fHPos = deltaX * i;

			glLineWidth(fPlotLineWidth);
			//TimeCounter tc("DisplayOpenGL ch update (line strip)");
			/** \note When zoom is very low (i.e. there are hundreds of points that
			should be draw at single 'X' display position) drawing all the points
			seems overkill. However, after testing with single line that was drawed
			vertically from min to max from set that belongs to 'X' position instead
			of multiple GL_LINE_STRIP objects - this was only marginally faster
			with 3 x 1MSample data set.
			*/
			glBegin(GL_LINE_STRIP);
			/** \note Taking value directly from std::vector* is much slower */
			register float *fs = &((*samples)[0]) + i;
			for (register int cnt = i; cnt<limit; cnt++)
			{
				glVertex2f(fHPos, *fs++);
				fHPos += deltaX;
			}
			glEnd();
			glLineWidth(1.0f);

			glPopMatrix();
		}
		else if (mode == MODE_SPECTRUM)
		{
			register float deltaF = 	0.5f/(data->fOscDivX * samples->size());
			int i = std::max<float>(State().fOffsetX, 0.0f) / deltaF;
			float limit = std::min<float>(samples->size(), State().fWidth/data->fOscDivX + i);
			glPushMatrix();
			glScalef(1.0,
					State().channels[channel_id].fZoomY/State().channels[channel_id].fRange,
					1.0);
			glTranslatef(0.0, -1.0f * State().channels[channel_id].fOffsetY, 0.0);
			glLineWidth(fPlotLineWidth);
			glBegin(GL_LINES);
			register float fHPos = data->fOscDivX * i;
			register float *fs = &((*samples)[0]);
			for (register int cnt = i; cnt<limit; cnt++)
			{
				glVertex2f(fHPos, 0.0f);
				glVertex2f(fHPos, fs[cnt]);
				fHPos += deltaF;
			}
			glEnd();
			glLineWidth(1.0f);

			i = std::max<float>(State().fOffsetX, 0.0f);
			glBegin(GL_POINT);
			for (float fHPos = i; fHPos<limit; fHPos+=1.0f)
			{
				if ((*samples)[i] == 0.0f)
				{
					glVertex2f(fHPos, 0.0f);
				}
				i++;
			}
			glEnd();
			glPopMatrix();
		}
	}

	// GND markers
	if (visibility[GND_MARKER])
	{
		std::vector<std::vector<float> > gndPositions;	///< used to prevent marker overlap
		for (unsigned int channel_id=0; channel_id<data->GetChannelCount(); channel_id++)
		{
			Data::Channel& channel = data->getChannel(channel_id);
			if (channel.bEnabled == false)
			{
				continue;
			}
			// get user defined or default color for channel trace
			TColor color = GetChannelColor(channel_id);
			glColor3f(((float)GetRValue(color))/255.0f,
				((float)GetGValue(color))/255.0f,
				((float)GetBValue(color))/255.0f);
			unsigned int level;
			static const float arrowHeight = 0.025f;
			float arrowWidth = State().fWidth/50.0f;
			float fPos = State().channels[channel_id].fZoomY *
				State().channels[channel_id].fOffsetY/State().channels[channel_id].fRange;
			bool bFit = false;
			for (level = 0; level<gndPositions.size(); level++)
			{
				bool bOverlapp = false;
				for (unsigned int i=0; i<gndPositions[level].size(); i++)
				{
					if (fabs(gndPositions[level][i] - fPos) < arrowHeight)
					{
						bOverlapp = true;
						break;	//could not fit on this level - marker should be moved right
					}
				}
				if (!bOverlapp)
				{
					bFit = true;
					gndPositions[level].push_back(fPos);
					break;
				}
			}
			if (!bFit)
			{
				gndPositions.resize(gndPositions.size()+1);
				gndPositions[level].push_back(fPos);
			}
			glBegin(GL_TRIANGLES);
			glVertex2f(State().fOffsetX + level*arrowWidth, arrowHeight/2.0f - fPos);
			glVertex2f(State().fOffsetX + level*arrowWidth, arrowHeight/(-2.0f) - fPos);
			glVertex2f(State().fOffsetX + (level+1)*arrowWidth, (-1.0)*fPos);
			glEnd();
		}
	}

	// trigger position (pre-trigger) marker
	if (visibility[TRIGGER_POS_MARKER] && mode == MODE_OSCILLOSCOPE)
	{
		// get user defined or default color for channel trace
		glColor3f(MYGL_COLORS(colorTriggerPosMarker));
		
		float arrowWidth = 0.0125f * State().fWidth;
		static const float arrowHeight = 0.05f;
		float fPos = State().fWidth * (float)State().iTriggerPosition/100.0f;
		glBegin(GL_TRIANGLES);
		glVertex2f(fPos - arrowWidth, 1.0f);
		glVertex2f(fPos + arrowWidth, 1.0f);
		glVertex2f(fPos, 1.0f - arrowHeight);
		glEnd();
	}

	// trigger voltage value marker
	if (visibility[TRIGGER_VAL_MARKER] && mode == MODE_OSCILLOSCOPE)
	{
		// get user defined or default color for triggering channel
		TColor color = GetChannelColor(iTriggerChannel);
		glColor3f(((float)GetRValue(color))/255.0f,
			((float)GetGValue(color))/255.0f,
			((float)GetBValue(color))/255.0f);
		static const float arrowHeight = 0.025f;
		float arrowWidth = State().fWidth/50.0f;
		float fPos = State().channels[iTriggerChannel].fZoomY *
			((float)State().iTriggerValue*State().channels[iTriggerChannel].fRange/100.0f - State().channels[iTriggerChannel].fOffsetY) /
			State().channels[iTriggerChannel].fRange;
		glBegin(GL_TRIANGLES);
		glVertex2f(State().fOffsetX + State().fWidth, arrowHeight/2.0f + fPos);
		glVertex2f(State().fOffsetX + State().fWidth, arrowHeight/(-2.0f) + fPos);
		glVertex2f(State().fOffsetX + State().fWidth - arrowWidth, fPos);
		glEnd();
	}

	// cursors
	if (visibility[CURSOR] && iNumberOfChannels > 0)
	{
		glColor3f(MYGL_COLORS(colorCursor1));
		glBegin(GL_LINES);
		float ypos = (-1.0) * State().channels[iCurrentChannel].fOffsetY /
			(State().channels[iCurrentChannel].fRange);
		float zoom = State().channels[iCurrentChannel].fZoomY;
		glVertex2f(State().fCursor1X-State().fWidth/(2*10*iWidth/iHeight),
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor1Y/State().channels[iCurrentChannel].fRange));
		glVertex2f(State().fCursor1X+State().fWidth/(2*10*iWidth/iHeight),
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor1Y/State().channels[iCurrentChannel].fRange));
		glVertex2f(State().fCursor1X,
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor1Y/State().channels[iCurrentChannel].fRange) - fHeight/(10));
		glVertex2f(State().fCursor1X,
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor1Y/State().channels[iCurrentChannel].fRange) + fHeight/(10));
		glEnd();

		glColor3f(MYGL_COLORS(colorCursor2));
		glBegin(GL_LINES);
		glVertex2f(State().fCursor2X-State().fWidth/(2*10*iWidth/iHeight),
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor2Y/State().channels[iCurrentChannel].fRange));
		glVertex2f(State().fCursor2X+State().fWidth/(2*10*iWidth/iHeight),
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor2Y/State().channels[iCurrentChannel].fRange));
		glVertex2f(State().fCursor2X,
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor2Y/State().channels[iCurrentChannel].fRange) - fHeight/(10));
		glVertex2f(State().fCursor2X,
			zoom * (ypos + fHeight * State().channels[iCurrentChannel].fCursor2Y/State().channels[iCurrentChannel].fRange) + fHeight/(10));
		glEnd();
	}

	if (visibility[RANGE_VALUES])
	{
		float fVal;
		if (mode == MODE_OSCILLOSCOPE || mode == MODE_RECORDER)
		{
			fVal = State().fWidth/10.0f;
			label = ValueStringFormatter::TimeToStr(fVal, 2);
		}
		else if (mode == MODE_SPECTRUM)
		{
			label = "";
			if (data->GetChannelCount() > 0)
			{
				Data::Channel& channel = data->getEnabledChannel();
				if (channel.dataOsc.size())
				{
					fVal = State().fWidth/10.0f;
					label = ValueStringFormatter::FreqToStr(fVal, 2);
				}
			}
		}


		float fLeft = (State().fWidth * 0.1) + State().fOffsetX;
		float fTop = 0.92f;

		PrintWithOutline(fLeft, fTop,
			colorRangeLabel, colorBackground,
			"%s/d", label.c_str());


		for (unsigned int channel_id=0; channel_id<data->GetChannelCount(); channel_id++)
		{
			Data::Channel& channel = data->getChannel(channel_id);
			if (channel.bEnabled == false)
			{
				continue;
			}
			// get user defined or default color for channel trace
			TColor color = GetChannelColor(channel_id);
			fLeft += (State().fWidth * 100.0f/iWidth);

			fVal = (State().channels[channel_id].fRange/State().channels[channel_id].fZoomY / 5.0f);
			if (mode == MODE_SPECTRUM)
				fVal *= 2.0f;
			label = ValueStringFormatter::VoltageToStr(fVal, 2);
			PrintWithOutline(fLeft, fTop,
				color, colorBackground,
				"%s: %s/d", GetChannelName(channel_id).c_str(), label.c_str());
		}
	}

	if (visibility[CURSOR_VALUES])
	{
		float fValX1, fValY1, fValX2, fValY2;
		GetCursorPhysical(CURSOR1, fValX1, fValY1);
		GetCursorPhysical(CURSOR2, fValX2, fValY2);
		std::string labelX1, labelY1, labelX2, labelY2;

		if (mode == MODE_OSCILLOSCOPE || mode == MODE_RECORDER)
		{
			labelX1 = ValueStringFormatter::TimeToStr(fValX1, 3);
			labelX2 = ValueStringFormatter::TimeToStr(fValX2, 3);
		}
		else if (mode == MODE_SPECTRUM)
		{
			labelX1 = ValueStringFormatter::FreqToStr(fValX1, 2);
			labelX2 = ValueStringFormatter::FreqToStr(fValX2, 2);
		}

		labelY1 = ValueStringFormatter::VoltageToStr(fValY1, 3);
		labelY2 = ValueStringFormatter::VoltageToStr(fValY2, 3);

		float fLeft1 = State().fOffsetX + State().fWidth - (State().fWidth * 180.0f/iWidth);;
		float fLeft2 = fLeft1 + (State().fWidth * 80.0f/iWidth);
		float fTop, dy;
		fTop = -0.8f;
		dy = 0.05f;

		TColor color = GetChannelColor(iCurrentChannel);
		PrintWithOutline(fLeft1, fTop,
			colorRangeLabel, colorBackground,
			"%s", labelX1.c_str());
		PrintWithOutline(fLeft2, fTop,
			color, colorBackground,
			"%s: %s", GetChannelName(iCurrentChannel).c_str(), labelY1.c_str());
		fTop -= dy;
		PrintWithOutline(fLeft1, fTop,
			colorRangeLabel, colorBackground,
			"%s", labelX2.c_str());
		PrintWithOutline(fLeft2, fTop,
			color, colorBackground,
			"%s: %s", GetChannelName(iCurrentChannel).c_str(), labelY2.c_str());
	}

	glFlush();
	SwapBuffers(hdc);
}
//---------------------------------------------------------------------------
void DisplayOpenGL::SetPixelFormatDescriptor()
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));					//ustalenie typu koloru, buforowania grafiki,
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int PixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, PixelFormat, &pfd);
}
//---------------------------------------------------------------------------
void DisplayOpenGL::SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// background color
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

float DisplayOpenGL::PosToPhysicalX(int x)
{
#if 0
	float fVal;
	assert(mode == MODE_OSCILLOSCOPE || mode == MODE_SPECTRUM || mode == MODE_RECORDER);
	if (mode == MODE_OSCILLOSCOPE || mode == MODE_RECORDER)
		fVal = fPosX * fDivX;
	else if (mode == MODE_SPECTRUM)
	{
		if (data->GetChannelCount() > 0)
		{
			Data::Channel& channel = data->getEnabledChannel();
			fVal = fPosX / (fDivX*NearestPowerOfTwo(channel.dataOsc.size())*2.0f);	///< \todo depends on FFT samples, not data size
		}
		else
		{
			return 0;
		}
	}
	return fVal;
#else
	return State().fOffsetX + (State().fWidth * x / iWidth);
#endif
}

float DisplayOpenGL::PosToPhysicalY(int y)
{
	return State().channels[iCurrentChannel].fOffsetY +
		((-1.0) * State().channels[iCurrentChannel].fRange +
		(2 * State().channels[iCurrentChannel].fRange * (iHeight-y-1) / iHeight))/
		State().channels[iCurrentChannel].fZoomY;
}

#if 0
float DisplayOpenGL::IPosToFloatX(int x)
{
	float val;
	val = State().fOffsetX + ((float)(x+1)/float(iWidth)*fWidth);
	return val;
}

float DisplayOpenGL::IPosToFloatY(int y)
{
	float val;
	val = State().channels[iCurrentChannel].fOffsetY +
		((float)(iHeight-y-1)/float(iHeight)*fHeight*2) - fHeight;
	return val;
}
#endif

void DisplayOpenGL::SetCursor1(int x, int y)
{
	State().fCursor1X = PosToPhysicalX(x);
	State().channels[iCurrentChannel].fCursor1Y = PosToPhysicalY(y);
}

void DisplayOpenGL::SetCursor2(int x, int y)
{
	State().fCursor2X = PosToPhysicalX(x);
	State().channels[iCurrentChannel].fCursor2Y = PosToPhysicalY(y);
}

void DisplayOpenGL::PosToPhysical(int x, int y, float &fValX, float &fValY)
{
	fValX = PosToPhysicalX(x);
	fValY = PosToPhysicalY(y);
}

int DisplayOpenGL::GetCursorPhysical(enum E_CURSOR cursor, float &fValX, float &fValY)
{
	if (cursor == CURSOR1)
	{
		fValX = State().fCursor1X;
		fValY = State().channels[iCurrentChannel].fCursor1Y;
	}
	else if (cursor == CURSOR2)
	{
		fValX = State().fCursor2X;
		fValY = State().channels[iCurrentChannel].fCursor2Y;
	}
	else
		return -1;
	return 0;
}

void DisplayOpenGL::SetZoom(int iCenterX, int iCenterY, float fZoomX, float fZoomY)
{
	float fNewWidth = State().fWidth * State().fZoomX / fZoomX;
	if (iCenterX == OFFSET_CLEAR) {
		this->State().fOffsetX = 0;
	} else if (iCenterX == OFFSET_LEAVE) {
	} else {
		this->State().fOffsetX = PosToPhysicalX(iCenterX) - fNewWidth/2.0f;
	}
	State().fWidth = fNewWidth;
	this->State().fZoomX = fZoomX;

	if (iCenterY == OFFSET_CLEAR) {
		if (mode == MODE_SPECTRUM)
			spectrum.channels[iCurrentChannel].fOffsetY = spectrum.channels[iCurrentChannel].fRange / spectrum.channels[iCurrentChannel].fZoomY;
		else
			this->State().channels[iCurrentChannel].fOffsetY = 0;
	} else if (iCenterY == OFFSET_LEAVE) {

	} else if (iCenterY == OFFSET_KEEP_GND_POSITION) {
		this->State().channels[iCurrentChannel].fOffsetY /= (fZoomY/this->State().channels[iCurrentChannel].fZoomY);
	} else {
		this->State().channels[iCurrentChannel].fOffsetY = PosToPhysicalY(iCenterY);
	}
	this->State().channels[iCurrentChannel].fZoomY = fZoomY;
	ChangeSize(this->iWidth, this->iHeight);

	DisplayNotifyArgument arg;
	arg.type = DisplayNotifyArgument::SCALE;
	notifyObservers(&arg);
}

void DisplayOpenGL::SetOffsetY(float fVal)
{
	this->State().channels[iCurrentChannel].fOffsetY = fVal;
	ChangeSize(this->iWidth, this->iHeight);
}


void DisplayOpenGL::Move(int iStartX, int iStartY, int iStopX, int iStopY)
{
	float deltaX = PosToPhysicalX(iStartX) - PosToPhysicalX(iStopX);
	float deltaY = PosToPhysicalY(iStartY) - PosToPhysicalY(iStopY);
	State().fOffsetX += deltaX;
	State().channels[iCurrentChannel].fOffsetY += deltaY;
	ChangeSize(this->iWidth, this->iHeight);
}

std::auto_ptr<Graphics::TBitmap> DisplayOpenGL::CreateBitmap(void)
{
	std::auto_ptr<Graphics::TBitmap> bitmap(new Graphics::TBitmap());
	bitmap->PixelFormat = pf24bit;
	bitmap->Width = iWidth;
	bitmap->Height = iHeight;

	int ww = ((bitmap->Width * 24 + 31) & ~31) >> 3;
	unsigned char *pData = new unsigned char[ww * bitmap->Height];
	glReadPixels(0, 0, bitmap->Width, bitmap->Height, GL_RGB, GL_UNSIGNED_BYTE, pData);

	for(int y=0; y<bitmap->Height; y++)
	{
		Byte* ptr = (Byte*)bitmap->ScanLine[iHeight-y-1];
		for(int x=0; x<bitmap->Width; x++)
		{
			ptr[x*3] = pData[y*ww + x*3 + 2];
			ptr[x*3+1]= pData[y*ww + x*3 + 1];
			ptr[x*3+2]= pData[y*ww + x*3];
		}
	}
	delete[] pData;

	return bitmap;
}

