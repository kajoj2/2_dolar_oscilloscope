//---------------------------------------------------------------------------


#pragma hdrstop

#include "Recorder.h"
#include "device/data.h"
#include "Utilities.h"

#include <json/json.h>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <assert.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)


int Recorder::Store(const std::string &filename, const enum E_FMT format, const Data &data)
{
	int status;
	if (format == FMT_JSON)
		status = StoreJson(filename, data);
	else if (format == FMT_CSV)
		status = StoreCsv(filename, data);
	else
		return -1;
	return status;
}


int Recorder::StoreJson(const std::string &filename, const Data &data)
{
	Json::Value root;
	Json::StyledWriter writer;

	char buf[64] = {0};
	char szTime[32] = {0};
	int size = strftime(szTime, sizeof(szTime), "%Y-%m-%d %T", localtime(&data.timestamp.time));
	if (size > 0)
	{
		// size--;	// not counting null-terminator
		snprintf(buf, sizeof(buf), "%s.%03hu", szTime, data.timestamp.millitm);
		buf[sizeof(buf)-1] = '\0';
	}

	root["Timestamp"] = buf;
	float fDivX;
	if (data.GetMode() == MODE_RECORDER)
		fDivX = data.fRecorderInterval;
	else
		fDivX = data.fOscDivX;
	root["fDivX"] = fDivX;
	root["TriggerMode"] = data.trigger_mode;
	root["TriggerType"] = data.trigger_type;
	root["TriggerSource"] = data.trigger_source;	

	unsigned ch = 0;
	for (unsigned i = 0; i<data.GetChannelCount(); i++)
	{
		const Data::Channel& channel = data.getChannelConst(i);
		if (channel.bEnabled == false)
		{
        	continue;
		}
		root["Channels"][ch]["Coupling"] = channel.iCouplingType;
		const std::vector<float> *samples;
		if (data.GetMode() == MODE_OSCILLOSCOPE || data.GetMode() == MODE_SPECTRUM)
			samples = &channel.dataOsc;
		else
			samples = &channel.dataRec;

		for (unsigned int i=0; i<samples->size(); i++)
		{
			// see json_writer for default precision
			root["Channels"][ch]["Samples"][i] = (*samples)[i];
		}
		ch++;
	}

	std::string outputConfig = writer.write( root );
	std::ofstream ofs(filename.c_str());
	if (!ofs.is_open())
		return -2;

	ofs << outputConfig;
	ofs.close();
	return 0;
}

int Recorder::StoreCsv(const std::string &filename, const Data &data)
{
	std::ofstream ofs(filename.c_str());
	if (!ofs.is_open())
		return -2;

	unsigned int ch_count = data.GetChannelCount();
	unsigned int active_ch_count = data.GetEnabledChannelCount();
	if (active_ch_count == 0)
	{
		ofs.close();
		return -3;
    }

	ofs << "Time";
	unsigned int samples_cnt = 0;
	for (unsigned int i=0; i<ch_count; i++)
	{
		if (data.getChannelConst(i).bEnabled)
		{
			ofs << ",Voltage " << GetChannelName(i).c_str();
			if (samples_cnt == 0)
			{
				const std::vector<float> *samples;
				if (data.GetMode() == MODE_OSCILLOSCOPE || data.GetMode() == MODE_SPECTRUM)
					samples = &data.getChannelConst(i).dataOsc;
				else
					samples = &data.getChannelConst(i).dataRec;
				samples_cnt = samples->size();
			}
		}
	}

	float time = 0;

	float fDivX;
	if (data.GetMode() == MODE_RECORDER)
		fDivX = data.fRecorderInterval;
	else
		fDivX = data.fOscDivX;

	for (unsigned int i=0; i<samples_cnt; i++)
	{
		ofs << "\r\n" << time;
		time += fDivX;
		for (unsigned int ch=0; ch<ch_count; ++ch)
		{
			const Data::Channel& channel = data.getChannelConst(ch);
			if (channel.bEnabled == false)
			{
            	continue;
			}
			const std::vector<float> *samples;
			if (data.GetMode() == MODE_OSCILLOSCOPE || data.GetMode() == MODE_SPECTRUM)
				samples = &channel.dataOsc;
			else
				samples = &channel.dataRec;

			ofs << "," << (*samples)[i];
		}
	}
	ofs.close();
	return 0;
}


