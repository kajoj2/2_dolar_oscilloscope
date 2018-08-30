/** \todo trigger_src, trigger_mode
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JsonViewerUnit.h"
#include "display\ValueStringFormatter.h"
#include "device\MiniscopeDeviceInterface.h"
#include "Log.h"

#include <json/json.h>
#include <fstream>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmJsonViewer *frmJsonViewer;
//---------------------------------------------------------------------------
__fastcall TfrmJsonViewer::TfrmJsonViewer(TComponent* Owner)
	: TfrmCommon(Owner),
	callbackClose(NULL),
	current_channel(0)
{
}
//---------------------------------------------------------------------------

int TfrmJsonViewer::LoadFile(AnsiString filename)
{
	std::ifstream ifs(filename.c_str());
	if (!ifs.is_open())
	{
		LOG(E_LOG_TRACE, "Failed to open %s", filename.c_str());
		return -1;
	}
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( strJson, root );
	if ( !parsingSuccessful )
	{
		LOG(E_LOG_TRACE, "Failed to parse JSON file %s", filename.c_str());
		return -2;
	}

	struct JsonData tmp;
	tmp.data.fOscDivX = root.get("fDivX", 0).asDouble();
	if (tmp.data.fOscDivX <= 0)
	{
		LOG(E_LOG_TRACE, "Incorrect fDivX value");
		return -3;
	}

	tmp.iTriggerType = root.get("TriggerType", -1).asInt();
	if (tmp.iTriggerType < 0)
	{
		LOG(E_LOG_TRACE, "Incorrect trigger type value");
		return -4;
	}

	tmp.iTriggerSource = root.get("TriggerSource", -1).asInt();
	if (tmp.iTriggerSource < 0)
	{
		LOG(E_LOG_TRACE, "Incorrect trigger source value");
		return -5;
	}

	tmp.asTimestamp = root.get("Timestamp", "").asString().c_str();

	const Json::Value channels = root["Channels"];
	if (channels.type() != Json::arrayValue || channels.size() == 0)
	{
		LOG(E_LOG_TRACE, "No data found in JSON file (channel count = 0)");
		return -6;
	}

	const Json::Value channel0 = root["Channels"][0u];
	if (channel0["Samples"].type() != Json::arrayValue)
	{
		LOG(E_LOG_TRACE, "No usable data in JSON file");
		return -7;
	}

	tmp.data.SetChannelCount(channels.size());
	// make sure all channels have same number of samples
	unsigned int samples_cnt = channel0["Samples"].size();
	for (unsigned int i=0; i<channels.size(); i++)
	{
		const Json::Value channel = root["Channels"][i];
		if (channel.type() != Json::objectValue)
		{
			LOG(E_LOG_TRACE, "JSON structure is different than expected");
			return -8;
		}
		const Json::Value samples = channel["Samples"];
		if (samples.size() != samples_cnt)
		{
			LOG(E_LOG_TRACE, "Sample number must be same in each channel");
			return -9;
		}

		tmp.data.getChannel(i).iCouplingType = channel.get("Coupling", -1).asInt();
		if (tmp.data.getChannel(i).iCouplingType < 0)
		{
			LOG(E_LOG_TRACE, "Incorrect coupling type value");
			return -4;
		}
	}

	// now we are sure that JSON file is valid
	// copy temporary data
	jsonData = tmp;
	for (unsigned int i=0; i<channels.size(); i++)
	{
		struct Data::Channel &channel = jsonData.data.getChannel(i);
		channel.dataOsc.clear();
		channel.dataOsc.reserve(samples_cnt);
		const Json::Value samples = root["Channels"][i]["Samples"];
		for ( unsigned int index = 0; index < samples_cnt; ++index )  // Iterates over the sequence elements.
		{
			channel.dataOsc.push_back( samples[index].asDouble() );
		}
	}

	edFilename->Text = filename;


	float fSamplVal = (float)1.0 / jsonData.data.fOscDivX;
	lblSamplingVal->Caption =
		ValueStringFormatter::FreqToStr(fSamplVal, 1).c_str();

	lblTriggerTypeVal->Caption = DeviceInterface::GetTriggerTypeName((E_TRIGGER_TYPE)jsonData.iTriggerType);
	lblTimestampVal->Caption = jsonData.asTimestamp;
	lblSamplesCountVal->Caption = samples_cnt;
	current_channel = 0;
	UpdateChannel();

	return 0;
}

void __fastcall TfrmJsonViewer::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (callbackClose)
	{
		callbackClose();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmJsonViewer::btnExitViewerClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------

void TfrmJsonViewer::SetCurrentChannel(int channel)
{
	current_channel = channel;
	UpdateChannel();
}

void TfrmJsonViewer::UpdateChannel(void)
{
	struct Data::Channel &channel = jsonData.data.getChannel(current_channel);
	lblCouplingTypeVal->Caption =
		DeviceInterface::GetCouplingTypeName((E_COUPLING_TYPE)channel.iCouplingType);
}


