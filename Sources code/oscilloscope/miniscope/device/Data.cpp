#include "Data.h"
#include <assert.h>
#include <mem.h>


Data::Data():
	fOscDivX(0.001f),
	fRecorderInterval(1.0f),
	trigger_mode(0),
	trigger_source(0),
	trigger_type(0),
	buffer_size(0),
	bFrameFilled(false),
	iBitsPerSample(0),
	iChannelCount(0),
	mode(MODE_OSCILLOSCOPE)
{
	memset(&timestamp, 0, sizeof(timestamp));
}

void Data::SetChannelCount(int ch_count)
{
	struct Channel channel;
	channels.clear();
	assert(ch_count >= 1);
	for (int i = 0; i < ch_count; i++)
	{
    	channels.push_back(channel);
	}
	iChannelCount = ch_count;
}

unsigned int Data::GetEnabledChannelCount(void) const
{
	unsigned int cnt = 0;
	for (int i=0; i<iChannelCount; i++)
	{
		if (channels[i].bEnabled == true)
			cnt++;
	}
	return cnt;
}

struct Data::Channel& Data::getEnabledChannel(void)
{
	for (int i=0; i<iChannelCount; i++)
	{
		if (channels[i].bEnabled == true)
			return channels[i];
	}
	assert(!"At least one channel must be enabled");
	return channels[0];
}

const struct Data::Channel& Data::getEnabledChannelConst(void) const
{
	for (int i=0; i<iChannelCount; i++)
	{
		if (channels[i].bEnabled == true)
			return channels[i];
	}
	assert(!"At least one channel must be enabled");
	return channels[0];
}

