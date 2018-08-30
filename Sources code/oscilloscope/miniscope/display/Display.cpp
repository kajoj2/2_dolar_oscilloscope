/** \file Display.cpp
    \brief Abstart display interface.
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

#include "Display.h"
#include "device/Data.h"
#include "Hsv.h"
#include <assert.h>

Display::Display(Data *data):
	data(data)
{
    assert(data);
	iNumberOfChannels = 0;
	iCurrentChannel = 0;
	iTriggerChannel = 0;
	mode = MODE_OSCILLOSCOPE;
	SetNumberOfChannels(1);

	for (unsigned int i=0; i<sizeof(visibility)/sizeof(visibility[0]); i++)
	{
    	visibility[i] = true;
	}

	fPlotLineWidth = 1.0f;
}

void Display::SetData(Data *data)
{
	assert(data);
	this->data = data;
	SetNumberOfChannels(data->GetChannelCount());
}

Data* Display::GetData(void)
{
	return data;
}

void Display::SetRange(int channel, float range)
{
#if 0
	spectrum.fOffsetY = (float)fRange / spectrum.fZoomY;
#endif
	assert(channel < iNumberOfChannels);
	scope.channels[channel].fRange = range;
	spectrum.channels[channel].fRange = range;
	recorder.channels[channel].fRange = range;

	DisplayNotifyArgument arg;
	arg.type = DisplayNotifyArgument::SCALE;
	notifyObservers(&arg);
}

void Display::SetNumberOfChannels(int number)
{
	iNumberOfChannels = number;
	if (iTriggerChannel >= iNumberOfChannels)
		iTriggerChannel = 0;
	scope.channels.resize(number);
	spectrum.channels.resize(number);
	recorder.channels.resize(number);
}

int Display::GetNumberOfChannels(void)
{
	return iNumberOfChannels;
}

void Display::SetChannelColors(const std::vector<unsigned int> &colors)
{
    channel_colors = colors;
}

TColor Display::GetChannelColor(unsigned int channel_id)
{
	if (channel_id < channel_colors.size())
	{
		return static_cast<TColor>(channel_colors[channel_id]);
	} else {
		Hsv hsv;
		return hsv.GetUniformColor(iNumberOfChannels, 0, channel_id);
	}
}   

void Display::SetCurrentChannel(int number)
{
	iCurrentChannel = number;
	DisplayNotifyArgument arg;
	arg.type = DisplayNotifyArgument::SCALE;
	notifyObservers(&arg);
}

void Display::SetVisibility(enum E_ELEMENT element, bool state)
{
	assert(element >= 0 && element < ELEMENT_LIMIT);
	visibility[element] = state;
}

void Display::GetZoom(float& fZoomX, float& fZoomY)
{
	fZoomX = State().fZoomX;
	fZoomY = State().channels[iCurrentChannel].fZoomY;
}

unsigned int Display::NearestPowerOfTwo(unsigned int count)
{
	unsigned int i = 1;
	while (i < count) {
		i = i << 1;
	}
	return i;
}

float Display::GetDivX(void)
{
	return State().fWidth/10.0f;
}

float Display::GetDivY(void)
{
	return State().channels[iCurrentChannel].fRange/State().channels[iCurrentChannel].fZoomY/5.0f;
}

void Display::SetTriggerChannel(int channel)
{
	iTriggerChannel = channel;
	Update(UPDATE_CURSOR);
}

void Display::SetTriggerPosition(int pos)
{
	scope.iTriggerPosition = pos;
	Update(UPDATE_CURSOR);
}

void Display::SetTriggerValue(int val)
{
	scope.iTriggerValue = val;
	Update(UPDATE_CURSOR);
}

void Display::SetScopeChannelZoom(int channel, float fZoomY)
{
    scope.channels[channel].fZoomY = fZoomY;
}

float Display::GetScopeChannelOffsetY(unsigned int channel)
{
	if (channel >= scope.channels.size())
		return 0.0f;
	return scope.channels[channel].fOffsetY;
}

void Display::SetScopeChannelOffsetY(unsigned int channel, float offset)
{
	scope.channels[channel].fOffsetY = offset;
}

void Display::SetPlotLineWidth(float width)
{
    fPlotLineWidth = width;
}
