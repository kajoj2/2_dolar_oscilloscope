/** \file MiniscopeDevice.h
	\brief Interface description for miniscope project device (ADC, preamplifier...) plugins
	\author Tomasz Ostrowski, http://tomeko.net
	\date October 2010

	This file should be shared by main application and device dll.
	You MUST declare _EXPORTING macro before including this file in dll sources.
*/

#ifndef MiniscopeDeviceH
#define MiniscopeDeviceH
//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#ifdef _MSC_VER
#	define CALL_CONV
#else
#	define CALL_CONV __stdcall
#endif

/** \def DECLARE_FN
    \brief Declare function or consistent function + function pointer set depending on _EXPORTING
*/
#ifdef _EXPORTING
#define DECLARE_FN(type, fn, ...) __declspec(dllexport) type CALL_CONV fn(__VA_ARGS__)
#else
#define DECLARE_FN(type, fn, ...) __declspec(dllimport) type __stdcall fn(__VA_ARGS__); \
	typedef type (__stdcall *pf##fn)(__VA_ARGS__)
#endif

/** \brief Dll interface version, used to dismiss outdated libraries
*/
enum { DLL_INTERFACE_MAJOR_VERSION = 12 };
/** \brief Dll interface version; used to dismiss outdated libraries
*/
enum { DLL_INTERFACE_MINOR_VERSION = 0 };

/** \brief Holds Dll interface version
*/
struct S_DEVICE_DLL_INTERFACE
{
	int majorVersion;   ///< major part of interface version
	int minorVersion;   ///< minor part of interface version
};

/** \brief State of connection between PC and data acquisition device
*/
enum E_CONNECTION_STATE
{
	DEVICE_DISCONNECTED = 0,
	DEVICE_CONNECTING,
	DEVICE_CONNECTED,
	DEVICE_DISCONNECTING
};

struct S_SCOPE_CAPABILITIES;
struct S_SCOPE_SETTINGS;

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS
///////////////////////////////////////////////////////////////////////////////

/** \brief Called to generate log in parent application
    \param cookie Holds internal data of callee (upper layer)
    \param szText Text to add to log
*/
typedef void (__stdcall *CALLBACK_LOG)(void *cookie, const char *szText);
/** \brief Called on connection/disconnection of device
    \param cookie Holds internal data of callee (upper layer)
	\param state Current state of device (\sa E_CONNECTION_STATE)
    \param szMsgText Additional text (if applicable)
*/
typedef void (__stdcall *CALLBACK_CONNECT)(void *cookie, int state, const char *szMsgText);
/** \brief Called to pass data (samples) to parent application
	\param cookie Holds internal data of callee (upper layer)
	\param buffer Pointer to samples buffer. Buffer should be allocated and released by library
	\param buffersize Size in bytes of buffer (or it's part that holds valid data)
	\note Data is expected to be divided equally between all channel. For 4096 bytes of
	data and 2 channels first 2048 B must belong to first channel, next 2048 B
	to second channel.
*/
typedef void (__stdcall *CALLBACK_DATA)(void *cookie, void *buffer, unsigned int buffersize);
/** \brief Callback function type called when device triggers
	\param cookie Holds internal data of callee (upper layer)
*/
typedef void (__stdcall *CALLBACK_TRIGGER)(void *cookie);


///////////////////////////////////////////////////////////////////////////////
// EXPORTED/IMPORTED FUNCTION SET
///////////////////////////////////////////////////////////////////////////////

/** \brief Get dll interface version to check for compatibility
*/
DECLARE_FN(void, GetMiniscopeInterfaceDescription, struct S_DEVICE_DLL_INTERFACE*);

/** \brief Get initial settings, either default or from config file/registry/etc.
*/
DECLARE_FN(int, GetMiniscopeSettings, struct S_SCOPE_SETTINGS*);

/** \brief Save settings to config file/registry/etc.
*/
DECLARE_FN(int, SaveMiniscopeSettings, struct S_SCOPE_SETTINGS*);

/** \brief Set dll callbacks (to i.e. send new data from dll to main application)
*/
DECLARE_FN(void, SetCallbacks, void *cookie, CALLBACK_LOG lpLog, CALLBACK_CONNECT lpConnect, CALLBACK_DATA lpData, CALLBACK_TRIGGER lpTrigger);

/** \brief Get detailed info about device capabilities
*/
DECLARE_FN(void, GetDeviceCapabilities, struct S_SCOPE_CAPABILITIES **caps);

/** \brief Show dll specific settings window (if available)
*/
DECLARE_FN(void, ShowSettings, HANDLE parent);

/** \brief Enable/disable channel
*/
DECLARE_FN(int, EnableChannel, int iChannelId, bool state);

/** \brief Set senstivity of device
*/
DECLARE_FN(int, SetSensitivity, int iChannelId, int iId);

/** \brief Set device sampling speed
*/
DECLARE_FN(int, SetSamplingPeriod, int iId);

/** \brief Run/stop acquisition
*/
DECLARE_FN(int, Run, bool state);

/** \brief Set acquisition mode (single/normal/auto...)
*/
DECLARE_FN(int, SetTriggerMode, int iId);

/** \brief Select trigger source (channel) and type
*/
DECLARE_FN(int, SetTriggerSource, int iChannelId, int iId);

/** \brief Set trigger level (-100%...+100% of FS)
*/
DECLARE_FN(int, SetTriggerLevel, int level);

/** \brief Set trigger position inside data frame (0%...100%, pre-/post-trigger).
	This function is optional and makes sense only for edge triggering.
*/
DECLARE_FN(int, SetTriggerPosition, int position);

/** \brief Request immediate triggering
*/
DECLARE_FN(int, ManualTrigger, void);

/** \brief Set type of coupling (AC/DC/..) for device
*/
DECLARE_FN(int, SetCouplingType, int iChannelId, int iId);

/** \brief Set device acquisition buffer size
*/
DECLARE_FN(int, SetBufferSize, int iId);

/** \brief Connect to device
*/
DECLARE_FN(int, Connect, void);

/** \brief Disconnect from device
*/
DECLARE_FN(int, Disconnect, void);

/** \brief Store calibration data (zero offset and sensitivity coefficient)
	for specified channel sensitivity range
*/
DECLARE_FN(int, StoreCalibration, int channel, int range, int offset, float gain);

/** \brief Get calibration data for specified channel sensitivity range
*/
DECLARE_FN(int, GetCalibration, int channel, int range, int* const offset, float* const gain);

#ifdef __cplusplus
}
#endif

#endif
