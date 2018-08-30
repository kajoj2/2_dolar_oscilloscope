/** \file MiniscopeDeviceInterface.cpp
 *  \brief Encapsulation of "C" interface to input device dll
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

#ifndef MiniscopeDeviceInterfaceH
#define MiniscopeDeviceInterfaceH
//---------------------------------------------------------------------------

#include "MiniscopeDevice.h"
#include "MiniscopeDeviceCapabilities.h"
#include "Data.h"
#include "Calibration.h"
#include "gnugettext.hpp"
#include <System.hpp>
#include <Forms.hpp>
#include <vector>
#include <set>

/** \brief Info about dll device library */
struct DllInfo
{
	AnsiString name;
	AnsiString description;
	AnsiString file_version;			///< file version (as reported by OS)
	struct S_DEVICE_DLL_INTERFACE ver;
};

/** \brief Info about current connection state
 */
struct ConnectionInfo
{
	enum E_CONNECTION_STATE state;
	AnsiString msg;
};

/** \brief Encapsulates dll plugin
*/
class DeviceInterface
{
private:
	enum E_LIB_STATUS
	{
		E_OK = 0,
		E_LOADFAILED,			///< failed to load library - library not found or other dll dependency not met
		E_NOTVALID,				///< does not look like a plugin library
		E_INCOMPATIBLE_VER		///< incompatible plugin interface version
	};

	/** \brief Verify if dll looks correctly and fill dllinfo for valid dll
		\return loading status
	*/
	static enum E_LIB_STATUS VerifyDll(AnsiString filename, struct DllInfo* const dllinfo);
	/** \brief Add dll to list
	*/
	static void AddDll(const struct DllInfo &dllinfo);

	static std::set<class DeviceInterface*> instances;

	static AnsiString asDllDir;
	AnsiString filename;	///< dll name (with full path)
	HINSTANCE hInstance;	///< dll instance ptr
	// dll function pointers
	pfGetMiniscopeInterfaceDescription dllGetMiniscopeInterfaceDescription;
	pfGetMiniscopeSettings dllGetMiniscopeSettings;
	pfSaveMiniscopeSettings dllSaveMiniscopeSettings;
	pfSetCallbacks dllSetCallbacks;
	pfShowSettings dllShowSettings;
	pfGetDeviceCapabilities dllGetDeviceCapabilities;
    pfEnableChannel dllEnableChannel;
	pfSetSensitivity dllSetSensitivity;
	pfSetSamplingPeriod dllSetSamplingPeriod;
	pfRun dllRun;
	pfSetTriggerMode dllSetTriggerMode;
	pfSetTriggerSource dllSetTriggerSource;
	pfSetTriggerLevel dllSetTriggerLevel;
	pfSetTriggerPosition dllSetTriggerPosition;
	pfSetCouplingType dllSetCouplingType;
	pfSetBufferSize dllSetBufferSize;
	pfConnect dllConnect;
	pfDisconnect dllDisconnect;
	pfManualTrigger dllManualTrigger;
	pfStoreCalibration dllStoreCalibration;
	pfGetCalibration dllGetCalibration;

	struct ConnectionInfo connInfo;


	// CALLBACKS DEFINITIONS
	typedef void (__closure *CallbackData)(void);
	typedef void (__closure *CallbackConnect)(int state, const char *szMsgText);
	typedef void (__closure *CallbackTrigger)(void);


public:
	/** \brief Make list of valid dlls in supplied location
	*/
	static void EnumerateDlls(AnsiString dir);
	/** \brief Refresh list of valid dlls in previously supplied location
	*/
	static void ReEnumerateDlls(void);
	/** \brief Constructor
		\param asDllName	Name of dll used by object to communicate with device
	*/
	DeviceInterface(AnsiString asDllName);
	~DeviceInterface();
	int Load(void);			///< load dll (dll name was supplied to constructor)
    /** Info about found device interface libraries */
	static std::vector<DllInfo> dlls;
	/** Ptr to structure describing device functionality. Actual structure is allocated
	 *  by device library
	 */
	struct S_SCOPE_CAPABILITIES *capabilities;
	CallbackData callbackData;         ///< if set called when new data is received from device
	CallbackConnect callbackConnect;   ///< if set called when connection state changes
    CallbackTrigger callbackTrigger;	///< if set called when device is triggered

    struct S_SCOPE_SETTINGS settings;

	/** Calibration data for each channel, for each sensitivity (2D array)
	*/
	std::vector< std::vector<struct DEV_CALIBRATION> > calibration;

	Data upcoming;							///< state updated immediately when GUI state changes
    int rx_progress;						///< upcoming frame buffer fill level (percentage)

	// dll callbacks
	static void __stdcall OnLog(void *cookie, const char *szText);                   ///< called to generate log in parent application
	static void __stdcall OnConnect(void *cookie, int state, const char *szMsgText); ///< called on connection/disconnection of device
	static void __stdcall OnData(void *cookie, void *buffer, unsigned int buffersize); ///< called to pass data (samples) to parent application
	static void __stdcall OnTrigger(void *cookie);                              ///< called when device is triggered

    /** \brief Show device settings window.
     *
     *  Show device settings window. Miniscope doesn't manage any settigs related to input devices. Device dll
     *  is responsible for storing/restoring data and can supply it's own window for this settings.
     */
	void ShowSettings(void) {
		if (dllShowSettings)
			dllShowSettings(Application->Handle);
	}
	/** \brief Connect device
	*/
	int Connect(void) {
		if (dllConnect)
			return dllConnect();
		return 0;
	}
	/** \brief Disconnect device
	*/
	int Disconnect(void) {
		if (dllDisconnect)
			return dllDisconnect();
		return 0;
	}
	/** \brief Enable/disable specified channel
	*/
	int EnableChannel(int channel, bool state) {
		if (dllEnableChannel)
		{
			int status = dllEnableChannel(channel, state);
			if (!status)
			{
				upcoming.getChannel(channel).bEnabled = state;
				settings.pChannelEnable[channel] = state;
			}
			return status;
		}
		return -1;
	}
	/** \brief Set analog gain
        \param sens index of new sensitivity value inside capabilities structure
        \return 0 on successs
	*/
	int SetSensitivity(int channel, int sens) {
		upcoming.getChannel(channel).fDivY = capabilities->fSensitivityLevels[sens];
		//int test = calibration.size();
		//int test2 = calibration[0].size();
		upcoming.getChannel(channel).calibration = calibration[channel][sens];
		if (dllSetSensitivity) {
			int status = dllSetSensitivity(channel, sens);
			if (!status)
				settings.pSensitivity[channel] = sens;
			return status;
		}
		return 0;
	}
	/** \brief Set sampling frequency
        \param sampl index of new sampling period inside capabilities structure
        \return 0 on successs
    */
	int SetSampling(int sampl) {
		upcoming.fOscDivX = capabilities->fSamplingPeriod[sampl];
		if (dllSetSamplingPeriod) {
			int status = dllSetSamplingPeriod(sampl);
			if (!status)
				settings.iSamplingPeriod = sampl;
			return status;
		}
		return 0;
	}
	int Run(bool state);
	int SetTriggerMode(int type) {
		upcoming.trigger_mode = capabilities->iTriggerModes[type];
		if (dllSetTriggerMode) {
			int status = dllSetTriggerMode(type);
			if (!status)
			{
            	settings.eTriggerMode = capabilities->iTriggerModes[type];
			}
			return status;
		}
		return 0;
	}
	/** \brief Set required trigger type
		\param channel source channel
		\param type trigger type according to enum E_TRIGGER_TYPE
		\return 0 on successs
	*/
	int SetTriggerType(int channel, int type) {
		upcoming.trigger_type = capabilities->iTriggerTypes[type];
		upcoming.trigger_source = channel;
		if (dllSetTriggerSource) {
			int status = dllSetTriggerSource(channel, type);
			if (!status) {
				settings.iTriggerSrcChannel = channel;
				settings.eTriggerType = capabilities->iTriggerTypes[type];
			}
			return status;
		}
		return 0;
	}
	/** \brief Set level of trigger
        \param level trigger level -100%...100%
        \return 0 on successs
    */
	int SetTriggerLevel(int level) {
		if (dllSetTriggerLevel) {
			int status = dllSetTriggerLevel(level);
			if (!status)
				settings.iTriggerLevel = level;
			return status;
		}
		return 0;
	}
	/** \brief Set position of trigger inside data frame (pre-/posttriggering)
		\param position position inside frame, 0%...100%
		\return 0 on successs
	*/
	int SetTriggerPosition(int position) {
		if (dllSetTriggerPosition) {
			int status = dllSetTriggerPosition(position);
			if (!status)
				settings.iTriggerPosition = position;
			return status;
		}
		return 0;
	}
	/** \brief Immediate trigger request
        \return 0 on success
    */
	int ManualTrigger(void) {
		if (dllManualTrigger)
			return dllManualTrigger();
		return -1;
	}
	/** \brief Set device acquision buffer size. After each triggering receiving full buffer from device is expected
        \param size size of buffer [samples]
	*/
	int SetBufferSize(int size) {
		upcoming.buffer_size = capabilities->iBufferSizes[size];	
		if (dllSetBufferSize) {
			int status = dllSetBufferSize(size);
			if (!status)
				settings.iBufferSize = capabilities->iBufferSizes[size];
			return status;
		}
		return 0;
	}
	/** \brief Set coupling type (AC/DC..) for analog part of device
        \param type index of coupling type inside capabilities structure
        \return 0 on success
    */
	int SetCouplingType(int channel, int type) {
		upcoming.getChannel(channel).iCouplingType = settings.eCouplingType[type];
		if (dllSetCouplingType) {
			int status = dllSetCouplingType(channel, type);
			if (!status)
				settings.eCouplingType[channel] = capabilities->iCouplingType[type];
			return status;
		}
		return 0;
	}
	/** \brief Get default or saved in configuration settings for device
	*/
	int GetSettings(struct S_SCOPE_SETTINGS* settings)
	{
		if (dllGetMiniscopeSettings)
			return dllGetMiniscopeSettings(settings);
		return -1;
	}

	/** \brief Save configuration settings for device
	*/
	int SaveSettings(struct S_SCOPE_SETTINGS* settings)
	{
		if (dllSaveMiniscopeSettings)
			return dllSaveMiniscopeSettings(settings);
		return -1;
	}

	/** \brief Save calibration data for specified sensitivity range
		\param range sensitivity range (index from capabilities structure)
		\param offset value (int LSB) corresponding to 0 V
		\param gain real_sensitivity/capabilities_sensitivity
		\return 0 on success
	*/
	int StoreCalibration(int channel, int range, int offset, float gain)
	{
		if (dllStoreCalibration)
			return dllStoreCalibration(channel, range, offset, gain);
		return -1;
	}

	/** \brief Read calibration data
		\param range sensitivity range (index from capabilities structure)
		\param offset value (int LSB) corresponding to 0 V
		\param gain real_sensitivity/capabilities_sensitivity
		\return 0 on success
	*/
	int GetCalibration(int channel, int range, int* const offset, float* const gain)
	{
		if (dllGetCalibration)
			return dllGetCalibration(channel, range, offset, gain);
		return -1;
	}

	bool hasCalibration(void) {
		if (dllGetCalibration && dllStoreCalibration)
			return true;
		return false;
	}

	bool hasTriggerPositionControl(void) {
		if (this->dllSetTriggerPosition)
			return true;
		return false;
	}

	bool hasChannelEnable(void) {
		if (this->dllEnableChannel)
			return true;
		return false;
	}

	bool hasManualTrigger(void) {
		if (this->dllManualTrigger)
			return true;
		return false;
	}

	const struct ConnectionInfo& GetConnectionInfo(void) {
		return connInfo;
	}

	static AnsiString GetTriggerTypeName(enum E_TRIGGER_TYPE type);
	static AnsiString GetTriggerModeName(enum E_TRIGGER_MODE mode);
	static AnsiString GetCouplingTypeName(enum E_COUPLING_TYPE type);
};

#endif
