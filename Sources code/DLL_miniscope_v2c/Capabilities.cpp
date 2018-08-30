//---------------------------------------------------------------------------

#pragma hdrstop

#include "Capabilities.h"
#include "ArmScope.h"
#include "Utils.h"
#include <string>
#include <fstream>
#include <json/json.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

Capabilities capabilities;

static enum E_TRIGGER_MODE eMode[] = { TRIGGER_MODE_AUTO, TRIGGER_MODE_NORMAL, TRIGGER_MODE_SINGLE, TRIGGER_MODE_CONTINUOUS };
static enum E_TRIGGER_TYPE eTrigger[] = { TRIGGER_SLOPE_UP, TRIGGER_SLOPE_DOWN };
static enum E_COUPLING_TYPE eCoupling[2] = { COUPLING_DC };
/** \sa size of Armscope::output_buffer when increasing max size
*/
static unsigned int iBuf[] = { 4096, 8192, 12288, 16384, 20480, 32768, 32768*2, 32768*4,
	32768*8, 32768*16, 32768*32 };
static float fSens[32] = { 0.02578 };
const float fBasicSampl = (12.5 + 13.5)/12e6;
static float fSampl[] = { fBasicSampl*1024, fBasicSampl*512, fBasicSampl*256, fBasicSampl*128, fBasicSampl*64, fBasicSampl*32,
	fBasicSampl*16, fBasicSampl*8, fBasicSampl*4, fBasicSampl*2, fBasicSampl };

Capabilities::Capabilities(void):
	signalInverted(false),
	signalOffset(0),
	bitsPerSample(9)
{
	unsigned int couplingArrSize = 1;
	unsigned int sensArrSize = 1;

	std::string path = Utils::GetDllPath();
	path = Utils::ReplaceFileExtension(path, "_capabilities.cfg");
	if (path != "")
	{
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;

		std::ifstream ifs(path.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();

		bool parsingSuccessful = reader.parse( strConfig, root );
		if (parsingSuccessful)
		{
            const Json::Value& other = root["Other"];
			bitsPerSample = other.get("bitsPerSample", bitsPerSample).asInt();
			signalOffset = other.get("signalOffset", signalOffset).asInt();
			signalInverted = other.get("signalInverted", signalInverted).asBool();

			const Json::Value& jsonCap = root["Capabilities"];
			{
				const Json::Value& sens = jsonCap["Sensitivity"];
				sensArrSize = std::min(ARRAY_SIZE(fSens), sens.size());
				if (sensArrSize < 1)
					sensArrSize = 1;
				for (unsigned int i=0; i<sensArrSize; i++)
				{
					fSens[i] = sens.get(i, 1.0f).asDouble();
				}
			}

			{
				const Json::Value& coupl = jsonCap["Coupling"];
				couplingArrSize = std::min(ARRAY_SIZE(eCoupling), coupl.size());
				if (couplingArrSize < 1)
					couplingArrSize = 1;
				for (unsigned int i=0; i<couplingArrSize; i++)
				{
					eCoupling[i] = coupl.get(i, COUPLING_DC).asInt();
				}
			}

		}
	}

	memset(&caps, 0, sizeof(caps));

	caps.iChannelCount = CHANNELS;
	caps.iBitsPerSample = bitsPerSample;		//bits/sample; use 8 for symmetric input (measuring negative voltages), 9 for positive voltages only

	caps.iSensitivityLevelsCount = sensArrSize;
	caps.fSensitivityLevels = fSens;

	caps.iSamplingPeriodCount =	sizeof(fSampl)/sizeof(fSampl[0]);
	caps.fSamplingPeriod = fSampl;

	caps.iTriggerModeCount = sizeof(eMode)/sizeof(eMode[0]);
	caps.iTriggerModes = eMode;

	caps.iTriggerTypeCount = sizeof(eTrigger)/sizeof(eTrigger[0]);
	caps.iTriggerTypes = eTrigger;

	caps.iCouplingTypeCount = couplingArrSize;
	caps.iCouplingType = eCoupling;

	caps.iBufferSizeCount = sizeof(iBuf)/sizeof(iBuf[0]);
	caps.iBufferSizes = iBuf;

	{
        // write back JSON
		Json::Value root;
		Json::StyledWriter writer;
		for (unsigned int i=0; i<caps.iSensitivityLevelsCount; i++)
		{
			  root["Capabilities"]["Sensitivity"][i] = caps.fSensitivityLevels[i];
		}
		for (unsigned int i=0; i<caps.iCouplingTypeCount; i++)
		{
			  root["Capabilities"]["Coupling"][i] = caps.iCouplingType[i];
		}

		root["Other"]["bitsPerSample"] = bitsPerSample;
		root["Other"]["signalOffset"] = signalOffset;
		root["Other"]["signalInverted"] = signalInverted;

		std::string outputConfig = writer.write( root );

		std::string path = Utils::GetDllPath();
		path = Utils::ReplaceFileExtension(path, "_capabilities.cfg");
		if (path != "")
		{
			std::ofstream ofs(path.c_str());
			ofs << outputConfig;
			ofs.close();
        }
	}
}

struct S_SCOPE_CAPABILITIES& Capabilities::get(void)
{
	return caps;
};

E_TRIGGER_MODE Capabilities::getSafeTriggerMode(int mode)
{
	for (unsigned int i=0; i<caps.iTriggerModeCount; i++)
	{
		if (caps.iTriggerModes[i] == mode)
			return caps.iTriggerModes[i];
	}
	return caps.iTriggerModes[0];
}

E_TRIGGER_TYPE Capabilities::getSafeTriggerType(int type)
{
	for (unsigned int i=0; i<caps.iTriggerTypeCount; i++)
	{
		if (caps.iTriggerTypes[i] == type)
			return caps.iTriggerTypes[i];
	}
	return caps.iTriggerTypes[0];
}

E_COUPLING_TYPE Capabilities::getSafeCouplingType(int type)
{
	for (unsigned int i=0; i<caps.iCouplingTypeCount; i++)
	{
		if (caps.iCouplingType[i] == type)
			return caps.iCouplingType[i];
	}
	return caps.iCouplingType[0];
}

int Capabilities::getSafeBufferSize(unsigned int size)
{
	for (unsigned int i=0; i<caps.iBufferSizeCount; i++)
	{
		if (caps.iBufferSizes[i] == size)
			return caps.iBufferSizes[i];
	}
	return caps.iBufferSizes[0];
}

