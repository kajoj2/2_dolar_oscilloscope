//---------------------------------------------------------------------------
//  watek odczytu danych z USB

#include "ThreadComm.h"
#include "usb.h"
#include <stdio.h>
#include "..\miniscope\device\MiniscopeDevice.h"
#include "ScopedLock.h"
#include "armscope_cmd.h"

#pragma package(smart_init)

bool writeLock;

__fastcall ThreadComm::ThreadComm()
		: callbackLog(NULL), callbackDataRcv(NULL),
		connected(true),
		rxFifo(512, E_USB_FRAME_SIZE)
{
}
//---------------------------------------------------------------------------


void ThreadComm::log(char *lpData, ...)
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

//#define DUMP_STREAM

//---------------------------------------------------------------------------
DWORD WINAPI ThreadCommProc(LPVOID data)
{
#ifdef DUMP_STREAM
	FILE *dump;
#endif

	ThreadComm *thrRead = (ThreadComm*)data;

	enum { CONTEXTS_COUNT = 50 };
	void* contexts[CONTEXTS_COUNT];
	static char buffers[CONTEXTS_COUNT][ThreadComm::E_USB_FRAME_SIZE];
	int ret;

	for (unsigned int i=0; i<sizeof(contexts)/sizeof(contexts[0]); i++)
	{
		ret = usb_bulk_setup_async(usb_handle, &contexts[i], 0x81);
		if (ret < 0)
		{
			thrRead->log("usb_bulk_setup_async: status = %d, %s", ret, usb_strerror());
			thrRead->connected = false;
			if (thrRead->callbackConnect)
				thrRead->callbackConnect(DEVICE_DISCONNECTED, "usb_bulk_setup_async error, disconnected");
			return 0;
		}
	}
	for (unsigned int i=0; i<sizeof(contexts)/sizeof(contexts[0]); i++)
	{
		ret = usb_submit_async(contexts[i], buffers[i], sizeof(buffers[i]));
		if (ret < 0)
		{
			thrRead->log("usb_submit_async: status = %d, %s", ret, usb_strerror());
			if (thrRead->callbackConnect)
				thrRead->callbackConnect(DEVICE_DISCONNECTED, "usb_submit_async error, disconnected");
			goto CLEANUP;
		}
	}

#ifdef DUMP_STREAM
	dump = fopen("C:\miniscope_v2c_stream.dump", "wb");
#endif

	int current_context = 0;
	while (thrRead->connected)
	{
		ret = usb_reap_async_nocancel(contexts[current_context], 400);
		if (ret < 0)
		{
			if (ret != -116)	//timeout
			{
				thrRead->log("usb_reap_async: status = %d, %s", ret, usb_strerror());
				if (thrRead->callbackConnect)
					thrRead->callbackConnect(DEVICE_DISCONNECTED, "usb_reap_async failed, disconnected");
				goto CLEANUP;
			}
		}
		else if (ret > 0)
		{
#ifdef DUMP_STREAM
			fwrite(buffers[current_context], sizeof(buffers[current_context]), 1, dump);
#endif
			//thrRead->log("RX %d", ret);
			int status = thrRead->rxFifo.put((unsigned char*)buffers[current_context], ret);
			if (status)
				thrRead->log("ThreadComm: RX FIFO overflowed!");

			ret = usb_submit_async(contexts[current_context], buffers[current_context], sizeof(buffers[0]));
			if (ret < 0)
			{
				thrRead->log("usb_submit_async: status = %d, %s", ret, usb_strerror());
				if (thrRead->callbackConnect)
					thrRead->callbackConnect(DEVICE_DISCONNECTED, "usb_submit_async error, disconnected");
				goto CLEANUP;
			}
			current_context++;
			current_context %= sizeof(contexts)/sizeof(contexts[0]);
		}

		{
			ScopedLock<Mutex> lock(thrRead->mutexSend);
			ThreadComm::Cmd scmd;
			if (!thrRead->que_cmd.empty())
			{
				scmd = thrRead->que_cmd.front();
				thrRead->que_cmd.pop_front();

				char buffer[3];
				buffer[0] = scmd.cmd;
				buffer[1] = scmd.val;
				buffer[2] = 0x00;
				thrRead->log("Sending cmd %02X, val %02X", scmd.cmd, scmd.val);
				int ret = usb_bulk_write(usb_handle, 0x03, buffer, 3, 500);
				if (ret < 3)
					thrRead->log("usb_bulk_write: status = %d, %s", ret, usb_strerror());
			}
		}
	}

CLEANUP:

#ifdef DUMP_STREAM
	fclose(dump);
#endif

	thrRead->connected = false;
	for (unsigned int i=0; i<sizeof(contexts)/sizeof(contexts[0]); i++)
	{
		usb_free_async(&contexts[i]);
	}
	return 0;
}
//---------------------------------------------------------------------------

DWORD WINAPI ThreadRxQueueProc(LPVOID data)
{
	ThreadComm *thrRead = (ThreadComm*)data;

	while (thrRead->connected)
	{
		unsigned char* usb_rcv;
		int len = thrRead->rxFifo.get(&usb_rcv);
		if (len > 0)
		{
			if (thrRead->callbackDataRcv) {
				thrRead->callbackDataRcv(usb_rcv, len);
			}
			for (int i=0; i<3; i++) {
				len = thrRead->rxFifo.get(&usb_rcv);
				if (len > 0)
				{
					if (thrRead->callbackDataRcv) {
						thrRead->callbackDataRcv(usb_rcv, len);
					}
				}
				else
				{
                	break;
				}
			}
			/** \note It seems that in some situations this thread is suspended for
			more than specified - loop above with rxFifo.get tries to fix it. */
			Sleep(2);
		}
		else
		{
			Sleep(10);
		}
	}
	return 0;
}

void ThreadComm::Execute(void)
{
	DWORD dwtid;
	Thread = CreateThread(NULL, 0, ThreadCommProc, this, 0, &dwtid);
	if (Thread)
	{
		bool status = SetThreadPriority(Thread, THREAD_PRIORITY_TIME_CRITICAL);
		if (!status)
			log("Failed to set communication thread priority!");
	}
	else
	{
		log("Failed to create communication thread!");
	}

	ThreadRxQueue = CreateThread(NULL, 0, ThreadRxQueueProc, this, 0, &dwtid);
	if (ThreadRxQueue)
	{
		//bool status = SetThreadPriority(ThreadRxQueue, THREAD_PRIORITY_ABOVE_NORMAL);
		bool status = SetThreadPriority(ThreadRxQueue, THREAD_PRIORITY_TIME_CRITICAL);
		if (!status)
			log("Failed to set data queue thread priority!");
	}
	else
	{
		log("Failed to create data queue thread!");
	}
}

int ThreadComm::SendCmd(unsigned char cmd, unsigned char val)
{
	if (!connected)
	{
		return -1;
	}
	else
	{
		//ScopedLock<Mutex> lock(mutexSend);
		Cmd scmd;
		scmd.cmd = cmd;
		scmd.val = val;
		mutexSend.lock();
		que_cmd.push_back(scmd);
		mutexSend.unlock();
		return 0;
	}
}


