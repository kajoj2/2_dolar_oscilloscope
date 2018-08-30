//---------------------------------------------------------------------------

#ifndef ArmScopeH
#define ArmScopeH
//---------------------------------------------------------------------------

#include "..\miniscope\device\MiniscopeDeviceCapabilities.h"
#include "..\miniscope\device\MiniscopeDevice.h"
#include "ThreadComm.h"
#include "Decimator.h"
#include <deque>

enum { CHANNELS = 2 };

class ArmScope
{
private:
	enum { VENDOR_ID = 0x03EB };
	enum { PRODUCT_ID = 0x6130 };

	bool bDeviceOpened;
	bool bRunning;
	bool bStopRequest;
	ThreadComm *threadComm;

	enum E_CONNECTION_STATE conn_state;
	unsigned char* hdr;
	unsigned int hdr_bytes;	///< skipped reply header bytes
	enum E_TRIGGER_MODE mode;
bool bChannelEnable[CHANNELS], bChannelEnableUpcoming[CHANNELS];
	unsigned int iSensitivity[CHANNELS], iSensitivityUpcoming[CHANNELS];
	E_COUPLING_TYPE eCouplingSettings[CHANNELS], eCouplingSettingsUpcoming[CHANNELS];
	struct S_SCOPE_SETTINGS settings, upcoming_settings;
	bool triggered;		///< we should start sending data buffer now
	bool bManualTrigger;///< manual trigger request flag
	unsigned int lastTriggerDistance;	///< "distance" in samples from last trigger used to start capture in auto mode
										///< when there is no trigger
	unsigned int frame_position;	///< current data buffer position (0 when just sent trigger signal)

    Decimator decimator_ch1, decimator_ch2;

	std::deque<char> deque_trigger;	///< samples buffer for triggering
	short samples_ch1[ThreadComm::E_USB_FRAME_SIZE];
	short samples_ch2[ThreadComm::E_USB_FRAME_SIZE];
	short output_buffer_ch1[32768];
	short output_buffer_ch2[32768];
	short output_buffer_concat[65536];
	/** \brief Test for trigger condition
		\param len number of samples added to trigger queue recently
		\return index of first sample after triggering, < 0 if not triggered
	*/
	int TestTrigger(short* output_buffer, int len);

	typedef void (*CallbackLog)(char*);
	typedef void (*CallbackData)(short*, int);
	typedef void (*CallbackConnect)(int, char*);
	typedef void (*CallbackTrigger)(void);
	void log(char *lpData, ...);
	void SetConnectionState(enum E_CONNECTION_STATE state, const char* const descr);

public:
	ArmScope();
	~ArmScope();
	int Connect(void);
	int Disconnect(void);
	CallbackLog callbackLog;
	CallbackData callbackData;
	CallbackConnect callbackConnect;
	CallbackTrigger callbackTrigger;
	void DataRcvCb(unsigned char data[], unsigned int len);
	/** \brief Request manual triggering
		\return 0 on success
	*/
	int ManualTrigger(void);
	int Run(bool state);
	int SetTriggerMode(enum E_TRIGGER_MODE);
	int SetTriggerType(int channel, enum E_TRIGGER_TYPE);
	int SetCouplingType(int channel, enum E_COUPLING_TYPE);
	/** \brief Set capture buffer size
		\param size number of samples
	*/
	int SetBufferSize(int size);
	int SetSensitivity(int channel, int id);
	int SetSampling(int id);
	int SetTriggerLevel(int level);
};
extern ArmScope armScope;


#endif
