//---------------------------------------------------------------------------

#pragma hdrstop

#define _EXPORTING
#include "..\miniscope\device\MiniscopeDevice.h"
#include "..\miniscope\device\MiniscopeDeviceCapabilities.h"
#include <assert.h>
#include "ArmScope.h"
#include "Capabilities.h"
#include <algorithm>	// needed by Utils::in_group
#include "Utils.h"
#include <string>
#include <fstream> 
#include <json/json.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

static const struct S_DEVICE_DLL_INTERFACE dll_interface =
	{DLL_INTERFACE_MAJOR_VERSION, DLL_INTERFACE_MINOR_VERSION};

// callback ptrs
CALLBACK_LOG lpLogFn = NULL;
CALLBACK_CONNECT lpConnectFn = NULL;
CALLBACK_DATA lpDataFn = NULL;
CALLBACK_TRIGGER lpTriggerFn = NULL;


void *callbackCookie;	///< used by upper class to distinguish library instances when receiving callbacks

void __stdcall GetMiniscopeInterfaceDescription(struct S_DEVICE_DLL_INTERFACE* interf)
{
	interf->majorVersion = dll_interface.majorVersion;
	interf->minorVersion = dll_interface.minorVersion;
}

void Log(char* txt)
{
	if (lpLogFn)
		lpLogFn(callbackCookie, txt);
}

void Data(short* data, int size)
{
	if (lpDataFn)
		lpDataFn(callbackCookie, data, size);
}

void Connect(int state, char *szMsgText)
{
	if (lpConnectFn)
		lpConnectFn(callbackCookie, state, szMsgText);
}

void Trigger(void)
{
	if (lpTriggerFn)
		lpTriggerFn(callbackCookie);
}

void __stdcall SetCallbacks(void *cookie, CALLBACK_LOG lpLog, CALLBACK_CONNECT lpConnect,
	CALLBACK_DATA lpData, CALLBACK_TRIGGER lpTrigger)
{
	assert(cookie && lpLog && lpConnect && lpData && lpTrigger);
	lpLogFn = lpLog;
	lpConnectFn = lpConnect;
	lpDataFn = lpData;
	lpTriggerFn = lpTrigger;
	callbackCookie = cookie;
	lpLogFn(callbackCookie, "miniscope_v2c.dll loaded. Yay!");

	armScope.callbackLog = Log;
	armScope.callbackData = Data;
	armScope.callbackConnect = Connect;
	armScope.callbackTrigger = Trigger;
}

bool pChannelEnable[CHANNELS];
unsigned int pSensitivity[CHANNELS];
enum E_COUPLING_TYPE eCouplingType[CHANNELS];

/** \brief Calibration data for single sensitivity range
*/
static struct S_CALIBRATION
{
	int offset;
	float gain;
} calibration[CHANNELS][Capabilities::MAX_SENSITIVITY_RANGES];

void __stdcall GetDeviceCapabilities(struct S_SCOPE_CAPABILITIES **caps)
{
	*caps = &capabilities.get();
}

void __stdcall ShowSettings(HANDLE parent)
{
	MessageBox(parent, "No additional settings.", "Device DLL", MB_ICONINFORMATION);
}

int __stdcall SetSensitivity(int iChannelId, int iId)
{
	if (iChannelId < 0 || iChannelId >= CHANNELS)
		return -1;
	if (iId < 0 || iId >= (int)capabilities.get().iSensitivityLevelsCount)
		return -2;
	return armScope.SetSensitivity(iChannelId, iId);
}

int __stdcall SetSamplingPeriod(int iId)
{
	if (iId < 0 || iId >= (int)capabilities.get().iSamplingPeriodCount)
		return -1;
	return armScope.SetSampling(capabilities.get().iSamplingPeriodCount - iId - 1);
}

int __stdcall SetTriggerSource(int iChannelId, int iId)
{
	if (iChannelId < 0 || iChannelId >= CHANNELS)
		return -1;
	if (iId < 0 || iId >= (int)capabilities.get().iTriggerTypeCount)
		return -2;
	return armScope.SetTriggerType(iChannelId, capabilities.get().iTriggerTypes[iId]);
}

int __stdcall SetTriggerLevel(int level)
{
	return armScope.SetTriggerLevel(level);	
}

int __stdcall SetCouplingType(int iChannelId, int iId)
{
	if (iChannelId < 0 || iChannelId >= CHANNELS)
		return -1;
	if (iId < 0 || iId >= (int)capabilities.get().iCouplingTypeCount)
		return -2;
	return armScope.SetCouplingType(iChannelId, capabilities.get().iCouplingType[iId]);
}

int __stdcall SetBufferSize(int iId)
{
	if (iId < 0 || iId >= (int)capabilities.get().iBufferSizeCount)
		return -1;
	return armScope.SetBufferSize(capabilities.get().iBufferSizes[iId]);
}


int __stdcall Connect(void)
{
	return armScope.Connect();
}

int __stdcall Disconnect(void)
{
	return armScope.Disconnect();
}

int __stdcall Run(bool state)
{
	return armScope.Run(state);
}

int __stdcall SetTriggerMode(int iId)
{
	if (iId < 0 || iId >= (int)capabilities.get().iTriggerModeCount)
		return -1;
	return armScope.SetTriggerMode(capabilities.get().iTriggerModes[iId]);
}

int __stdcall ManualTrigger(void)
{
	return armScope.ManualTrigger();
}

static bool bSettingsReaded = false;

static int GetDefaultSettings(struct S_SCOPE_SETTINGS* settings)
{
	settings->pChannelEnable = pChannelEnable;
	settings->pSensitivity = pSensitivity;
	settings->eCouplingType = eCouplingType;

	settings->pChannelEnable[0] = true;
	settings->pChannelEnable[1] = true;
	settings->eCouplingType[0] = capabilities.getSafeCouplingType(0);
	settings->eCouplingType[1] = capabilities.getSafeCouplingType(0);
	settings->pSensitivity[0] = 0;
	settings->iSamplingPeriod = 0;
	settings->eTriggerMode = TRIGGER_MODE_AUTO;
	settings->eTriggerType = TRIGGER_SLOPE_UP;
	settings->iTriggerLevel = 0;
	settings->iTriggerPosition = 50;
	settings->iTriggerSrcChannel = 0;
	settings->iBufferSize = capabilities.get().iBufferSizes[0];
	for (int i=0; i<CHANNELS; i++)
	{
		for (unsigned int j=0; j<sizeof(calibration[0])/sizeof(calibration[0][0]); j++)
		{
			calibration[i][j].offset = 0;
			calibration[i][j].gain = 1.0;
		}
	}
	bSettingsReaded = true;
	return 0;
}

int __stdcall GetMiniscopeSettings(struct S_SCOPE_SETTINGS* settings)
{
	settings->pChannelEnable = pChannelEnable;
	settings->pSensitivity = pSensitivity;
	settings->eCouplingType = eCouplingType;

	settings->iTriggerSrcChannel = 0;

	std::string path = Utils::GetDllPath();
	path = Utils::ReplaceFileExtension(path, ".cfg");
	if (path == "")
		return GetDefaultSettings(settings);

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	std::ifstream ifs(path.c_str());
	std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	bool parsingSuccessful = reader.parse( strConfig, root );
	if ( !parsingSuccessful )
		return GetDefaultSettings(settings);

	settings->pChannelEnable = pChannelEnable;

	const Json::Value sens = root["Sensitivity"];
	unsigned int cnt = std::min(sizeof(pSensitivity)/sizeof(pSensitivity[0]), sens.size());
	for (unsigned int i=0; i<cnt; i++)
	{
		pSensitivity[i] = sens.get(i, 0).asInt();
		if (pSensitivity[i] >= capabilities.get().iSensitivityLevelsCount)
			pSensitivity[i] = 0;
	}
	for (unsigned int i=cnt; i < sizeof(pSensitivity)/sizeof(pSensitivity[0]); i++)
	{
		pSensitivity[i] = 0;
	}
	settings->pSensitivity = pSensitivity;

	settings->iSamplingPeriod = root.get("SamplingPeriod", 0).asInt();
	if (settings->iSamplingPeriod >= capabilities.get().iSamplingPeriodCount)
		settings->iSamplingPeriod = 0;

	settings->eTriggerMode = capabilities.getSafeTriggerMode(root.get("TriggerMode", TRIGGER_MODE_AUTO).asInt());

	settings->eTriggerType = capabilities.getSafeTriggerType(root.get("TriggerType", TRIGGER_SLOPE_UP).asInt());

	settings->iTriggerLevel = root.get("TriggerLevel", 0).asInt();
	if (settings->iTriggerLevel < -100 || settings->iTriggerLevel > 100)
		settings->iTriggerLevel = 0;

	const Json::Value coupl = root["CouplingType"];
	settings->eCouplingType = eCouplingType;
	cnt = std::min(sizeof(eCouplingType)/sizeof(eCouplingType[0]), coupl.size());
	for (unsigned int i=0; i<cnt; i++)
	{
		settings->eCouplingType[i] = capabilities.getSafeCouplingType(coupl.get(i, COUPLING_DC).asInt());
	}
	for (unsigned int i=cnt; i < sizeof(eCouplingType)/sizeof(eCouplingType[0]); i++)
	{
		settings->eCouplingType[i] = capabilities.getSafeCouplingType(0);
	}

	settings->iBufferSize = capabilities.getSafeBufferSize(root.get("BufferSize", 4096).asInt());

	// read calibration data also
	const Json::Value rootcal = root["Calibration"];	
	for (int i=0; i<CHANNELS; i++)
	{
		const Json::Value ch_cal = rootcal[i];
		if (ch_cal.type() != Json::arrayValue)
		{
			for (unsigned int j=0; j<sizeof(calibration[i])/sizeof(calibration[i][0]); j++)
			{
				calibration[i][j].offset = 0;
				calibration[i][j].gain = 1.0f;
			}
		}
		else
		{
			for (unsigned int j=0; j<sizeof(calibration[i])/sizeof(calibration[i][0]); j++)
			{
				calibration[i][j].offset = rootcal[i][j].get("Offset", 0).asInt();
				calibration[i][j].gain = rootcal[i][j].get("Gain", 1.0).asDouble();
			}
		}
	}
	bSettingsReaded = true;
	return 0;
}

int __stdcall SaveMiniscopeSettings(struct S_SCOPE_SETTINGS* settings)
{
	Json::Value root;
	Json::StyledWriter writer;
	for (unsigned int i=0; i<sizeof(pSensitivity)/sizeof(pSensitivity[0]); i++)
	{
		  root["Sensitivity"][i] = pSensitivity[i];
	}
	root["SamplingPeriod"] = settings->iSamplingPeriod;
    root["TriggerMode"] = settings->eTriggerMode;
	root["TriggerType"] = settings->eTriggerType;
	root["TriggerLevel"] = settings->iTriggerLevel;
	for (unsigned int i=0; i<sizeof(eCouplingType)/sizeof(eCouplingType[0]); i++)
	{
		  root["CouplingType"][i] = eCouplingType[i];
	}
	root["BufferSize"] = settings->iBufferSize;

	// add calibration data also
	const Json::Value rootcal = root["Calibration"];
	for (unsigned int i=0; i<CHANNELS; i++)
	{
		for (unsigned int j=0; j<sizeof(calibration[0])/sizeof(calibration[0][0]); j++)
		{
			  root["Calibration"][i][j]["Offset"] = calibration[i][j].offset;
			  root["Calibration"][i][j]["Gain"] = calibration[i][j].gain;
		}
	}

	std::string outputConfig = writer.write( root );

	std::string path = Utils::GetDllPath();
	path = Utils::ReplaceFileExtension(path, ".cfg");
	if (path == "")
		return -1;

    std::ofstream ofs(path.c_str());
    ofs << outputConfig;
	ofs.close();

	return 0;
}

int __stdcall StoreCalibration(int channel, int range, int offset, float gain)
{
	if (channel < 0 || channel >= CHANNELS)
		return -1;
	if (range < 0 || range >= static_cast<int>(capabilities.get().iSensitivityLevelsCount))
		return -2;
	calibration[channel][range].offset = offset;
	calibration[channel][range].gain = gain;
	return 0;
}

int __stdcall GetCalibration(int channel, int range, int* const offset, float* const gain)
{
	if (channel < 0 || channel >= CHANNELS)
		return -1;
	if (range < 0 || range >= (int)capabilities.get().iSensitivityLevelsCount)
		return -2;
	struct S_SCOPE_SETTINGS settings;
	if (bSettingsReaded == false)
	{
		int status = GetMiniscopeSettings(&settings);
		if (status)
			return status;
	}
	*offset = calibration[channel][range].offset;
	*gain = calibration[channel][range].gain;
	return 0;
}

