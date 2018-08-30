/** \file MiniscopeDeviceInterface.cpp
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

//---------------------------------------------------------------------------

#pragma hdrstop

#include "MiniscopeDeviceInterface.h"
#include "MiniscopeDevice.h"
#include "Utilities.h"
#include "Log.h"
#include "gnugettext.hpp"

//---------------------------------------------------------------------------

#pragma package(smart_init)

std::vector<DllInfo> DeviceInterface::dlls;
std::set<class DeviceInterface*> DeviceInterface::instances;
AnsiString DeviceInterface::asDllDir;

void DeviceInterface::EnumerateDlls(AnsiString dir)
{
	LOG(E_LOG_TRACE, "Enumerating device dlls...");
	LOG(E_LOG_TRACE, "Directory: %s", dir.c_str());
	asDllDir = dir;
	DeviceInterface::dlls.clear();
	WIN32_FIND_DATA file;
	if (dir[dir.Length()] != '\\')
		dir += "\\";
	AnsiString asSrchPath = dir + "*.dll";
	HANDLE hFind = FindFirstFile(asSrchPath.c_str(), &file);
	int hasfiles = (hFind != INVALID_HANDLE_VALUE);
	struct DllInfo dllinfo;

	while (hasfiles)
	{
		AnsiString filename = dir + file.cFileName;
		if(VerifyDll(filename, &dllinfo) == E_OK)
		{
			AddDll(dllinfo);
		}
		else
		{
			LOG(E_LOG_TRACE, "Library %s not loaded", ExtractFileName(filename).c_str());
        }
		hasfiles = FindNextFile(hFind, &file);
	}
	FindClose(hFind);
}

void DeviceInterface::ReEnumerateDlls(void)
{
	EnumerateDlls(asDllDir);
}

enum DeviceInterface::E_LIB_STATUS DeviceInterface::VerifyDll(AnsiString filename, struct DllInfo* const dllinfo)
{
#if 0
	// Suppressing "Cannot Find ThisLibrary.dll" or "Cannot Find SomeOtherRequired.dll" Error Box
	UINT uOldErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
	HINSTANCE hInstance = LoadLibrary(filename.c_str());
	SetErrorMode(uOldErrorMode);

	if ((int)hInstance == 2)
	{
		//MessageBox(NULL, "MYDLL.DLL does not exist", "ERROR", MB_OK);
		return false;
	}
	else if ((int)hInstance <= HINSTANCE_ERROR)
	{
		//MessageBox(NULL, "Other problems loading MYDLL.DLL, "ERROR", MB_OK);
		return false;
	}
#else
	// Windows error message when loading may actually be usefull when resolving additional dll dependencies
	HINSTANCE hInstance = LoadLibrary(filename.c_str());
	if (!hInstance)
	{
		int error = GetLastError();
		LOG(E_LOG_TRACE, "Failed to load %s library (error %d)", ExtractFileName(filename).c_str(), error);
		return E_LOADFAILED;
	}
#endif

	pfGetMiniscopeInterfaceDescription gmid =
		(pfGetMiniscopeInterfaceDescription)GetProcAddress(hInstance, "GetMiniscopeInterfaceDescription");
	if (!gmid)
	{
		LOG(E_LOG_TRACE, "Library %s DOES NOT look like a compatible plugin",
			ExtractFileName(filename).c_str());
		FreeLibrary(hInstance);
		return E_NOTVALID;
	}
	struct S_DEVICE_DLL_INTERFACE interf_version;
	gmid(&interf_version);
	if (interf_version.majorVersion != DLL_INTERFACE_MAJOR_VERSION ||
		interf_version.minorVersion != DLL_INTERFACE_MINOR_VERSION)
	{
		LOG(E_LOG_TRACE, "Library %s has interface v.%d.%d instead of required %d.%d",
			ExtractFileName(filename).c_str(),
			interf_version.majorVersion, interf_version.minorVersion,
			DLL_INTERFACE_MAJOR_VERSION);
		FreeLibrary(hInstance);
		return E_INCOMPATIBLE_VER;
	}

	FreeLibrary(hInstance);
	dllinfo->name = filename;
	dllinfo->file_version = GetFileVer(dllinfo->name);
	dllinfo->ver.majorVersion = interf_version.majorVersion;
	dllinfo->ver.minorVersion = interf_version.minorVersion;
	return E_OK;
}

void DeviceInterface::AddDll(const struct DllInfo &dllinfo)
{
	LOG(E_LOG_TRACE, "Adding %s", ExtractFileName(dllinfo.name).c_str());
	DeviceInterface::dlls.push_back(dllinfo);
}

void __stdcall DeviceInterface::OnLog(void *cookie, const char *szText)
{
#if 0
	class DeviceInterface *dev;
	dev = reinterpret_cast<class DeviceInterface*>(cookie);
	if (instances.find(dev) == instances.end())
	{
		LOG(E_LOG_TRACE, "OnLog called with unknown cookie %p. No matching object.", dev);
		return;
	}
#endif
	LOG(E_LOG_TRACE, "%s", szText);
}

void __stdcall DeviceInterface::OnConnect(void *cookie, int state, const char *szMsgText)
{
	class DeviceInterface *dev;
	dev = reinterpret_cast<class DeviceInterface*>(cookie);
	if (instances.find(dev) == instances.end())
	{
		//LOG(E_LOG_TRACE, "OnConnect called with unknown cookie %p. No matching object.", dev);
		return;
	}
	dev->connInfo.state = static_cast<enum E_CONNECTION_STATE>(state);
	dev->connInfo.msg = szMsgText;
	if (dev->callbackConnect)
		dev->callbackConnect(state, szMsgText);
}

void __stdcall DeviceInterface::OnData(void *cookie, void *buffer, unsigned int buffersize)
{
	class DeviceInterface *dev;
	dev = reinterpret_cast<class DeviceInterface*>(cookie);
	if (instances.find(dev) == instances.end())
	{
		//LOG(E_LOG_TRACE, "OnData called with unknown cookie %p. No matching object.", dev);
		return;
	}

	short* ptr = (short*)buffer;
	int size = buffersize / sizeof(*ptr);
	if (size == 0)
		return;
#if 0
	AnsiString log = "";
	int log_cnt = size;
	if (log_cnt > 20)
		log_cnt = 20;
	for (int i=0; i<log_cnt; i++) {
		log = log + " " + buf[i];
	}
	LOG(E_LOG_TRACE, "Data (%d): %s", size, log.c_str());
	//LOG(E_LOG_TRACE, "Data: %d", size);
	//LOG(E_LOG_TRACE, "DataFn: %d samples, total: %d samples", size, dataOscTmp.size());
#endif

	// push received samples to "upcoming" temporary vector
	for (unsigned int channel_id=0; channel_id<dev->upcoming.GetChannelCount(); channel_id++)
	{
		Data::Channel& channel = dev->upcoming.getChannel(channel_id);
		if (channel.bEnabled == false)
		{
        	continue;
		}
		float coeff = dev->upcoming.getChannel(channel_id).fDivY * dev->upcoming.getChannel(channel_id).calibration.gain;
		float offset = dev->upcoming.getChannel(channel_id).calibration.offset;
		for (unsigned int i=0; i<size/dev->upcoming.GetEnabledChannelCount(); i++)
		{
			channel.dataOsc.push_back(((*ptr)-offset) * coeff);
			ptr++;
		}

		if (channel.dataOsc.size() >= dev->upcoming.buffer_size)
		{
			dev->upcoming.bFrameFilled = true;
			dev->rx_progress = 100;
		}
	}

	if (dev->callbackData)
	{
		dev->callbackData();
	}

	if (dev->upcoming.bFrameFilled)
	{
		for (unsigned int i=0; i<dev->upcoming.GetChannelCount(); i++)
		{
			dev->upcoming.getChannel(i).dataOsc.clear();
			dev->upcoming.getChannel(i).dataOsc.reserve(dev->upcoming.buffer_size);
		}
		dev->upcoming.bFrameFilled = false;
	}
	else
	{
		for (unsigned int channel_id=0; channel_id<dev->upcoming.GetChannelCount(); channel_id++)
		{
			Data::Channel& channel = dev->upcoming.getChannel(channel_id);
			if (channel.bEnabled == true)
			{
				dev->rx_progress = 100 * channel.dataOsc.size()/dev->upcoming.buffer_size;
				break;;
			}
		}
	}
}

void __stdcall DeviceInterface::OnTrigger(void *cookie)
{
	class DeviceInterface *dev;
	dev = reinterpret_cast<class DeviceInterface*>(cookie);
	if (instances.find(dev) == instances.end())
	{
		//LOG(E_LOG_TRACE, "OnData called with unknown cookie %p. No matching object.", dev);
		return;
	}

	ftime(&dev->upcoming.timestamp);
#if 0
	LOG(E_LOG_TRACE, "Trigger");
#endif
	for (unsigned int i=0; i<dev->upcoming.GetChannelCount(); i++)
	{
		Data::Channel& channel = dev->upcoming.getChannel(i);
		channel.dataOsc.clear();
		channel.dataOsc.reserve(dev->upcoming.buffer_size);
	}
	dev->upcoming.bFrameFilled = false;
	dev->rx_progress = 0;
	if (dev->callbackTrigger)
		dev->callbackTrigger();	
}




DeviceInterface::DeviceInterface(AnsiString asDllName):
	hInstance(NULL),
	filename(asDllName),
	capabilities(NULL),
	callbackData(NULL),
	callbackConnect(NULL),
	dllGetMiniscopeInterfaceDescription(NULL),
	dllGetMiniscopeSettings(NULL),
	dllSaveMiniscopeSettings(NULL),
	dllSetCallbacks(NULL),
	dllShowSettings(NULL),
	dllGetDeviceCapabilities(NULL),
    dllEnableChannel(NULL),
	dllSetSensitivity(NULL),
	dllSetSamplingPeriod(NULL), dllSetTriggerSource(NULL),
	dllSetTriggerLevel(NULL), dllSetTriggerPosition(NULL),
	dllSetCouplingType(NULL), dllSetBufferSize(NULL),
	dllConnect(NULL), dllDisconnect(NULL),
	dllRun(NULL),
	dllSetTriggerMode(NULL),
	dllManualTrigger(NULL),
	dllStoreCalibration(NULL), dllGetCalibration(NULL),
	rx_progress(0)
	//connInfo.state( {DEVICE_DISCONNECTED, "Not connected"} ),
{
	LOG(E_LOG_TRACE, "Creating object using %s", ExtractFileName(asDllName).c_str());
	connInfo.state = DEVICE_DISCONNECTED;
	connInfo.msg = _("Not connected");
	instances.insert(this);
}

DeviceInterface::~DeviceInterface()
{
	instances.erase(this);
	if (hInstance)
		FreeLibrary(hInstance);
}

int DeviceInterface::Load(void)
{
	hInstance = LoadLibrary(filename.c_str());
	if (!hInstance)
		return 1;
	dllSetCallbacks = (pfSetCallbacks)GetProcAddress(hInstance, "SetCallbacks");
	dllShowSettings = (pfShowSettings)GetProcAddress(hInstance, "ShowSettings");
	dllGetDeviceCapabilities = (pfGetDeviceCapabilities)GetProcAddress(hInstance, "GetDeviceCapabilities");

	dllGetMiniscopeSettings = (pfGetMiniscopeSettings)GetProcAddress(hInstance, "GetMiniscopeSettings");
	dllSaveMiniscopeSettings = (pfSaveMiniscopeSettings)GetProcAddress(hInstance, "SaveMiniscopeSettings");
    dllEnableChannel = (pfEnableChannel)GetProcAddress(hInstance, "EnableChannel");
	dllSetSensitivity = (pfSetSensitivity)GetProcAddress(hInstance, "SetSensitivity");
	dllSetSamplingPeriod = (pfSetSamplingPeriod)GetProcAddress(hInstance, "SetSamplingPeriod");
	dllRun = (pfRun)GetProcAddress(hInstance, "Run");
	dllSetTriggerMode = (pfSetTriggerMode)GetProcAddress(hInstance, "SetTriggerMode");
	dllSetTriggerSource = (pfSetTriggerSource)GetProcAddress(hInstance, "SetTriggerSource");
	dllSetTriggerLevel = (pfSetTriggerLevel)GetProcAddress(hInstance, "SetTriggerLevel");
	dllSetTriggerPosition = (pfSetTriggerPosition)GetProcAddress(hInstance, "SetTriggerPosition");
	dllSetCouplingType = (pfSetCouplingType)GetProcAddress(hInstance, "SetCouplingType");
	dllSetBufferSize = (pfSetBufferSize)GetProcAddress(hInstance, "SetBufferSize");
	dllConnect = (pfConnect)GetProcAddress(hInstance, "Connect");
	dllDisconnect = (pfDisconnect)GetProcAddress(hInstance, "Disconnect");
	dllManualTrigger = (pfManualTrigger)GetProcAddress(hInstance, "ManualTrigger");
	dllStoreCalibration = (pfStoreCalibration)GetProcAddress(hInstance, "StoreCalibration");
	dllGetCalibration = (pfGetCalibration)GetProcAddress(hInstance, "GetCalibration");

	if ((dllSetCallbacks && dllShowSettings &&
		dllGetDeviceCapabilities && dllConnect &&
		dllDisconnect && dllRun) == 0)
	{
		LOG(E_LOG_TRACE, "Dll load failed. Some of the required functions are missing");
		return 2;
	}

	dllSetCallbacks(this, &OnLog, &OnConnect, &OnData, &OnTrigger);
	dllGetDeviceCapabilities(&this->capabilities);
	upcoming.SetBitsPerSample(this->capabilities->iBitsPerSample);
	return 0;
}

AnsiString DeviceInterface::GetTriggerTypeName(enum E_TRIGGER_TYPE type)
{
	switch(type)
	{
	case TRIGGER_SLOPE_UP:
		return _("Raising slope");
	case TRIGGER_SLOPE_DOWN:
		return _("Falling slope");
	default:
		return _("<unknown>");
	}
}

AnsiString DeviceInterface::GetTriggerModeName(enum E_TRIGGER_MODE mode)
{
	switch(mode)
	{
	case TRIGGER_MODE_SINGLE:
		return _("Single");
	case TRIGGER_MODE_NORMAL:
		return _("Normal");
	case TRIGGER_MODE_AUTO:
		return _("Auto");
	case TRIGGER_MODE_CONTINUOUS:
		return _("Continuous");
	default:
		return _("<unknown>");
	}
}

AnsiString DeviceInterface::GetCouplingTypeName(enum E_COUPLING_TYPE type)
{
	switch(type)
	{
	case COUPLING_GND:
		return _("GND");
	case COUPLING_DC:
		return _("DC");
	case COUPLING_AC:
		return _("AC");
	default:
		return _("???");
	}
}

int DeviceInterface::Run(bool state)
{
	if (dllRun) {
		return dllRun(state);
	}
	return 0;
}

