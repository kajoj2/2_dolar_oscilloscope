//---------------------------------------------------------------------------

#pragma hdrstop

#include "ArmScope.h"
#include "armscope_cmd.h"
#include "ThreadComm.h"
#include "Capabilities.h"
#include "usb.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>
#include <assert.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "libusb.lib"

ArmScope armScope;
struct usb_device *my_dev = NULL;
struct usb_dev_handle *usb_handle = NULL;

/** Thanks to Openmoko for VID/PID pair */
#define VENDOR_ID   0x1D50
#define PRODUCT_ID  0x604F


static struct usb_device *device_init(void)
{
    struct usb_bus *usb_bus;
    struct usb_device *dev;
    usb_init();
//    armScope.log("USB: Enumerating devices...");
    usb_find_busses();
    usb_find_devices();
    for (usb_bus = usb_get_busses(); usb_bus; usb_bus = usb_bus->next)
    {
        for (dev = usb_bus->devices; dev; dev = dev->next)
        {
            if ((dev->descriptor.idVendor == VENDOR_ID) && (dev->descriptor.idProduct == PRODUCT_ID))
            {
				//Log("Device found\r\n");
                return dev;
            }
        }
    }
    return NULL;
}


ArmScope::ArmScope():
	threadComm(NULL),
	hdr_bytes(0),
	hdr(NULL),
	bRunning(false),
	mode(TRIGGER_MODE_AUTO),
	lastTriggerDistance(0)
{
    bDeviceOpened = false;
	bStopRequest = false;
	callbackTrigger = NULL;
    callbackData = NULL;
    callbackLog = NULL;
	callbackConnect = NULL;

	// some default values
	settings.iChannelCount = CHANNELS;
	settings.pChannelEnable = bChannelEnable;
	settings.pSensitivity = this->iSensitivity;
	for (int i=0; i<CHANNELS; i++)
	{
		settings.pSensitivity[i] = 0;
	}

	settings.eCouplingType = this->eCouplingSettings;
	for (int i=0; i<CHANNELS; i++)
	{
		settings.eCouplingType[i] = COUPLING_DC;
	}

	settings.iSamplingPeriod = 1;
	settings.eTriggerMode = TRIGGER_MODE_AUTO;
	settings.eTriggerType = TRIGGER_SLOPE_UP;
	settings.iTriggerLevel = 0;
	settings.iBufferSize = 4096;

	upcoming_settings.pChannelEnable = this->bChannelEnableUpcoming;
	upcoming_settings.pSensitivity = this->iSensitivityUpcoming;
	upcoming_settings.eCouplingType = this->eCouplingSettingsUpcoming;
	upcoming_settings = settings;

	frame_position = 0;
	triggered = false;
	bManualTrigger = false;
}

ArmScope::~ArmScope()
{
}

void ArmScope::log(char *lpData, ...)
{
    static char buf[256]; //determines max message length
    va_list ap;
	va_start(ap, lpData);
    vsnprintf(buf, sizeof(buf), lpData, ap);
    va_end(ap);
    buf[sizeof(buf)-1] = 0;

    if (callbackLog)
        callbackLog(buf);
}

int ArmScope::Connect(void)
{
	assert(callbackTrigger && callbackData && callbackLog && callbackConnect);

	if (conn_state != DEVICE_DISCONNECTED)
		Disconnect();
	log("Searching for libusb device VID = 0x%04X, PID = 0x%04X", VENDOR_ID, PRODUCT_ID);
	my_dev = device_init();
    if (my_dev == NULL)
    {
		SetConnectionState(DEVICE_DISCONNECTED, "Device not found. Connect device and restart program.");
		return 1;
    }
    else
    {
		//open device
        usb_handle = usb_open( my_dev );
        if (usb_handle == NULL)
		{
			SetConnectionState(DEVICE_DISCONNECTED, "Error opening device handle.");
            return 2;
        }
        else
        {
            //set configuration
            int ret;
            if ( ( ret = usb_set_configuration( usb_handle, 1 ) ) < 0 )
            {
				SetConnectionState(DEVICE_DISCONNECTED, "Failed to set device interface configuration.");
				log("usb config set failed with status %d", ret);
				return 3;
            }
            if ( ( ret = usb_claim_interface( usb_handle, 1 ) ) < 0 )   //chyba interfejs 1 - ten od BULK
            {
				SetConnectionState(DEVICE_DISCONNECTED, "Failed to claim USB interface.");
				Disconnect();
                log("FAIL: %s", usb_strerror());
                return 4;
            }
            else
            {
				SetConnectionState(DEVICE_CONNECTING, "Connecting...");
				threadComm = new ThreadComm();
				threadComm->callbackLog = callbackLog;
				threadComm->callbackDataRcv = DataRcvCb;
				threadComm->callbackConnect = callbackConnect;
				threadComm->Execute();

				if (threadComm->SendCmd(CMD_FUNCTION, CMD_FUNCTION_IDENTIFY) == 0)
				{
					log("Waiting for ID reply");
				}
				else
				{
					return 5;
				}
            }
        }
    }
    return 0;
}

int ArmScope::Disconnect(void)
{
	SetConnectionState(DEVICE_DISCONNECTING, "Disconnecting...");
	if (threadComm)
	{
		threadComm->connected = false;
		Sleep(600);
		delete threadComm;
		threadComm = NULL;
	}
	if (usb_handle == NULL)
        return 0;
    usb_close(usb_handle);
	usb_handle = NULL;
	SetConnectionState(DEVICE_DISCONNECTED, "Disconnected");
	hdr_bytes = 0;
	hdr = NULL;
	return 0;
}

int ArmScope::ManualTrigger(void)
{
	if (!threadComm)
		return -1;
	if (conn_state != DEVICE_CONNECTED)
		return -2;
	bManualTrigger = true;
	return 0;
}

int ArmScope::Run(bool state)
{
	bRunning = state;
	if (!bRunning)
	{
        bManualTrigger = false;
		lastTriggerDistance = 0;
		frame_position = 0;
	}
	return 0;
}

int ArmScope::SetTriggerMode(enum E_TRIGGER_MODE m)
{
	mode = m;
	if (mode == TRIGGER_MODE_AUTO)
		lastTriggerDistance = 0;	// reset
	return 0;
}

int ArmScope::SetTriggerType(int channel, enum E_TRIGGER_TYPE type)
{
	if (!threadComm)
		return -1;
	if (type == TRIGGER_SLOPE_UP)
	{
		upcoming_settings.eTriggerType = type;
	}
	else if (type == TRIGGER_SLOPE_DOWN)
	{
		upcoming_settings.eTriggerType = type;
	}
	else
		return -2;
	upcoming_settings.iTriggerSrcChannel = channel;
	return 0;
}

int ArmScope::SetCouplingType(int channel, enum E_COUPLING_TYPE type)
{
	if (!threadComm)
		return -1;
	enum E_SUBCOMMAND_FUNCTION_TYPE cmd;
	if (type == COUPLING_DC)
		cmd = CMD_FUNCTION_COUPLING_DC;
	else if (type == COUPLING_AC)
		cmd = CMD_FUNCTION_COUPLING_AC;
	else
		return -2;
#if 1
TODO___COUPLING_TYPE_CHANNEL:
	return 0;
#else
	return threadComm->SendCmd(CMD_FUNCTION, cmd);
#endif
}

int ArmScope::SetBufferSize(int size)
{
	if (!threadComm)
		return -1;
	upcoming_settings.iBufferSize = size;
	return 0;
}

int ArmScope::SetSensitivity(int channel, int id)
{
	if (channel < 0 || channel >= 2)
		return -2;
	if (id < 0 || id > 5)
		return -1;
#if 1
	TODO___CMD_SET_SENSITIVITY:
	return 0;
#else
	return threadComm->SendCmd(CMD_SET_SENSITIVITY, id + 1);
#endif
}

int ArmScope::SetSampling(int id)
{
	if (id < 0 || id > 10)
		return -1;
	upcoming_settings.iSamplingPeriod = (1 << id);
	return 0;
}

int ArmScope::SetTriggerLevel(int level)
{
	if (level < -100 || level > 100)
		return -1;
	upcoming_settings.iTriggerLevel = level;
	return 0;
}

static unsigned char* memsearch(const unsigned char* hay, int haylen, const unsigned char* needle, int needlelen)
{
	for (int i=0; i<=haylen-needlelen; i++)
	{
		if (memcmp(hay+i, needle, needlelen) == 0)
			return (unsigned char*)(hay+i);
	}
	return NULL;
}

void ArmScope::DataRcvCb(unsigned char data[], unsigned int len)
{
	if (len%2)
		log("Uneven data count!");

	if (conn_state == DEVICE_CONNECTING)
	{
		static const char szId[] = "miniscope_v2c 20120414";
	#if 0
		// search for ID string
		///< \note fails if header+id will be splitted across 2 or more function calls
		static const unsigned char replyHeader[] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
												0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
												0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF };
		if (!hdr)
		{
			hdr = memsearch(data, len, replyHeader, sizeof(replyHeader));
			if (hdr)
			{
				log("Header found, looking for ID: %s", szId);
				hdr_bytes = hdr - data + sizeof(replyHeader);
				len = len - hdr_bytes;
				data += hdr_bytes;
			}
		}
		if (hdr)
		{
			unsigned char* id = memsearch(data, len, (unsigned char*)szId, strlen(szId));
			if (id)
			{
				// skip 4096 bytes starting from hdr, this is counter for skipped bytes
				hdr_bytes += id - data + strlen(szId);
				len -= id - data + strlen(szId);
				data += id - data + strlen(szId);
				SetConnectionState(DEVICE_CONNECTED, szId);
			}
		}
	#else
		unsigned char* id = memsearch(data, len, (unsigned char*)szId, strlen(szId));
		if (id)
		{
			// skip 4096 bytes starting from hdr, this is counter for skipped bytes
			hdr_bytes += id - data + strlen(szId);
			len -= id - data + strlen(szId);
			data += id - data + strlen(szId);
			SetConnectionState(DEVICE_CONNECTED, szId);
		}
	#endif
	}

	if (conn_state == DEVICE_CONNECTED)
	{
		if (hdr_bytes < ThreadComm::E_USB_FRAME_SIZE)
		{
			int add = std::min(len, ThreadComm::E_USB_FRAME_SIZE-hdr_bytes);
			hdr_bytes += add;
			data += add;
			len -= add;
			log("ID header skipped");
		}
		if (len == 0)
			return;
		if (len % 2)
			return;

		if (frame_position == 0)
		{
			settings = upcoming_settings;
			decimator_ch1.SetFactor(settings.iSamplingPeriod);
			decimator_ch2.SetFactor(settings.iSamplingPeriod);
		}
#if 0
		char logbuf[128] = {0};
		for (int i=0; i<len; i++)
		{
			if (i >= 16)
				break;
			sprintf(logbuf + strlen(logbuf), "%03d, ", data[i]);
		}
		log("Data: %s", logbuf);
#endif
		unsigned int pos = 0;
		int offset = capabilities.getSignalOffset();
		if (capabilities.isSignalInverted() == false)
		{
			for (unsigned int i=0; i<len; i+=2)
			{
				samples_ch1[pos++] = offset + data[i];
			}
			pos = 0;
			for (unsigned int i=1; i<len; i+=2)
			{
				samples_ch2[pos++] = offset + data[i];
			}
		}
		else
		{
			for (unsigned int i=0; i<len; i+=2)
			{
				samples_ch1[pos++] = offset - data[i];
			}
			pos = 0;
			for (unsigned int i=1; i<len; i+=2)
			{
				samples_ch2[pos++] = offset - data[i];
			}
        }

		int iSamplesCnt = decimator_ch1.Process(samples_ch1, len/2, output_buffer_ch1);
		decimator_ch2.Process(samples_ch2, len/2, output_buffer_ch2);
		if (iSamplesCnt <= 0)
			return;

		if (bRunning == false && bManualTrigger == false)
		{
			frame_position = 0;
			return;
		}

		short* samples_ch1_p = output_buffer_ch1;
		short* samples_ch2_p = output_buffer_ch2;
#if 0
		log("Ch1: %d, %d, %d, %d | Ch2: %d, %d, %d, %d",
			samples_ch1_p[0], samples_ch1_p[1], samples_ch1_p[2], samples_ch1_p[2],
			samples_ch2_p[0], samples_ch2_p[1], samples_ch2_p[2], samples_ch2_p[3]);
#endif
		//log("RX len = %d", len);
		if (frame_position == 0)
		{
			int trigger_pos;
			short *trigger_data;
			if (settings.iTriggerSrcChannel == 0)
				trigger_data = output_buffer_ch1;
			else
				trigger_data = output_buffer_ch2;
			trigger_pos = TestTrigger(trigger_data, iSamplesCnt);
			if (trigger_pos < 0)
			{
				// ignore this data portion
				return;
			}
			else
			{
				samples_ch1_p += trigger_pos;
				samples_ch2_p += trigger_pos;
				iSamplesCnt -= trigger_pos;
				//log("callbackTrigger");
				callbackTrigger();
			}
		}

		if (iSamplesCnt + frame_position < settings.iBufferSize)
		{
			memcpy(output_buffer_concat, samples_ch1_p, iSamplesCnt * sizeof(output_buffer_ch1[0]));
			memcpy((char*)output_buffer_concat + iSamplesCnt * sizeof(output_buffer_ch1[0]),
				samples_ch2_p, iSamplesCnt * sizeof(output_buffer_ch2[0]));
			//log("L%03d| Ch1: %d, %d | Ch2: %d, %d", __LINE__, samples_ch1_p[0], samples_ch1_p[1], samples_ch2_p[0], samples_ch2_p[1]);
			//log("callbackData0 %d", iSamplesCnt * sizeof(output_buffer_ch1[0]) * 2);
			callbackData(output_buffer_concat, iSamplesCnt * sizeof(output_buffer_ch1[0]) * 2);
			frame_position += iSamplesCnt;
		}
		else
		{
			if (settings.iBufferSize-frame_position > 0)
			{
				memcpy(output_buffer_concat, samples_ch1_p, (settings.iBufferSize-frame_position) * sizeof(output_buffer_ch1[0]));
				memcpy((char*)output_buffer_concat + (settings.iBufferSize-frame_position) * sizeof(output_buffer_ch1[0]),
					samples_ch2_p, (settings.iBufferSize-frame_position) * sizeof(output_buffer_ch2[0]));
				//log("callbackData1 %d, frame_position %d", (settings.iBufferSize-frame_position) * sizeof(output_buffer_ch1[0]) * 2, frame_position);
				callbackData(output_buffer_concat, (settings.iBufferSize-frame_position) * sizeof(output_buffer_ch1[0]) * 2);
			}
			iSamplesCnt -= (settings.iBufferSize-frame_position);
			frame_position += (settings.iBufferSize-frame_position);

			//log("updated by %d, samplesCnt = %d", settings.iBufferSize-frame_position, iSamplesCnt);

			memmove(output_buffer_ch1, output_buffer_ch1+(settings.iBufferSize-frame_position),
				iSamplesCnt * sizeof(output_buffer_ch1[0]));
			memmove(output_buffer_ch2, output_buffer_ch2+(settings.iBufferSize-frame_position),
				iSamplesCnt * sizeof(output_buffer_ch2[0]));
			frame_position = iSamplesCnt;


			bManualTrigger = false;
			if (mode != TRIGGER_MODE_CONTINUOUS)
			{
				frame_position = 0;
				settings = upcoming_settings;
				decimator_ch1.SetFactor(settings.iSamplingPeriod);
				decimator_ch2.SetFactor(settings.iSamplingPeriod);
			}
			else if (bRunning)
			{
				settings = upcoming_settings;
				decimator_ch1.SetFactor(settings.iSamplingPeriod);
				decimator_ch2.SetFactor(settings.iSamplingPeriod);
				//log("callbackTrigger, %d samples left", iSamplesCnt);
				callbackTrigger();
				if (iSamplesCnt > 0)
				{
					memcpy(output_buffer_concat, output_buffer_ch1, iSamplesCnt * sizeof(output_buffer_ch1[0]));
					memcpy((char*)output_buffer_concat + iSamplesCnt * sizeof(output_buffer_ch1[0]),
						output_buffer_ch2, iSamplesCnt * sizeof(output_buffer_ch2[0]));
					//log("callbackData2 %d", iSamplesCnt * sizeof(output_buffer_ch1[0]) * 2);
					callbackData(output_buffer_concat, iSamplesCnt * sizeof(output_buffer_ch1[0]) * 2);
				}
				if (frame_position == 0)
					bManualTrigger = true;
			}
			if (mode == TRIGGER_MODE_SINGLE)
				bRunning = false;
		}
	}
}

void ArmScope::SetConnectionState(enum E_CONNECTION_STATE state, const char* const descr)
{
	conn_state = state;
	if (callbackConnect)
		callbackConnect(state, (char*)descr);
	log("Connection state changed to %d. %s", (int)state, descr);
}

int ArmScope::TestTrigger(short* output_buffer, int len)
{
	int pos = -1;
	float fs;
	int TODO__VERY_UGLY_TRIGGER_RANGE_SELECTION_FOR_SYMMETRICAL_OR_ASSYMETRICAL_INPUT;
	if (capabilities.get().iBitsPerSample == 9)
		fs = 2.55f;
	else
		fs = 1.27f;

	int level = fs * (float)settings.iTriggerLevel;
	if (bManualTrigger || mode == TRIGGER_MODE_CONTINUOUS)
	{
		pos = 0;
	}
	else
	{
		switch (settings.eTriggerType)
		{
#if 0
		case TRIGGER_SLOPE_UP:
			for (int i=0; i<len-6; i++)
			{
				if (output_buffer[i] < level && output_buffer[i+1] <= level && output_buffer[i+2] <= level &&
					output_buffer[i+3] >= level && output_buffer[i+4] >= level && output_buffer[i+5] > level)
				{
					//log("i = %d, len = %d, %d %d %d %d %d %d", i, len, output_buffer[i], output_buffer[i+1], output_buffer[i+2],
					//	output_buffer[i+3], output_buffer[i+4], output_buffer[i+5]);
					pos = i;
					break;
				}
			}
			break;
		case TRIGGER_SLOPE_DOWN:
			for (int i=0; i<len-6; i++)
			{
				if (output_buffer[i] > level && output_buffer[i+1] >= level && output_buffer[i+2] >= level &&
					output_buffer[i+3] <= level && output_buffer[i+4] <= level && output_buffer[i+5] < level)
				{
					//log("i = %d, len = %d, %d %d %d %d %d %d", i, len, output_buffer[i], output_buffer[i+1], output_buffer[i+2],
					//	output_buffer[i+3], output_buffer[i+4], output_buffer[i+5]);
					pos = i;
					break;
				}
			}
			break;
#else
		case TRIGGER_SLOPE_UP:
			for (int i=0; i<len-2; i++)
			{
				if (output_buffer[i] < level && output_buffer[i+1] >= level)
				{
					pos = i;
					break;
				}
			}
			break;
		case TRIGGER_SLOPE_DOWN:
			for (int i=0; i<len-2; i++)
			{
				if (output_buffer[i] > level && output_buffer[i+1] <= level)
				{
					//log("i = %d, len = %d, %d %d %d %d %d %d", i, len, output_buffer[i], output_buffer[i+1], output_buffer[i+2],
					//	output_buffer[i+3], output_buffer[i+4], output_buffer[i+5]);
					pos = i;
					break;
				}
			}
			break;
#endif
		default:
			break;
		}
	}
	if (mode == TRIGGER_MODE_AUTO)
	{
		if (pos == -1)
		{
			lastTriggerDistance += len;
			// generate auto-trigger when there is no trigger for the equivalent time of 3 frames
			if (lastTriggerDistance >= 3 * upcoming_settings.iBufferSize)
			{
				pos = 0;
				lastTriggerDistance = 0;
			}
		}
		else
		{
			lastTriggerDistance = 0;
		}
	}
	return pos;
}

