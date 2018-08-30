//---------------------------------------------------------------------------

#ifndef ThreadCommH
#define ThreadCommH
//---------------------------------------------------------------------------
#include <deque>
#include "Mutex.h"
#include "Fifo.h"
//---------------------------------------------------------------------------

extern TCriticalSection *SC;
extern struct usb_dev_handle *usb_handle;
extern bool writeLock;
extern int regIdx;

class ThreadComm
{
private:
	typedef void (*CallbackLog)(char*);
	typedef void (*CallbackData)(short*, int);
	typedef void (*CallbackConnect)(int, char*);
	Mutex mutexSend;
	bool bContinuousTrigger;
	struct Cmd {
		unsigned char cmd;
		unsigned char val;
	};
	friend DWORD WINAPI ThreadCommProc(LPVOID data);
	friend DWORD WINAPI ThreadRxQueueProc(LPVOID data);
	FIFO rxFifo;
	std::deque<struct Cmd> que_cmd;
	typedef void (__closure *CallbackDataRcv)(unsigned char *data, unsigned int len);


protected:
public:
	enum { E_USB_FRAME_SIZE = 4096 };
	bool connected;
   	CallbackLog callbackLog;
	CallbackDataRcv callbackDataRcv;
	CallbackConnect callbackConnect;
	__fastcall ThreadComm();

	HANDLE Thread;
	HANDLE ThreadRxQueue;
	void Execute();
	/** \brief Send command to armscope device
		\param cmd first byte of command
		\param val second byte of command (value or subcommand)
		\return zero on success
	*/
	int SendCmd(unsigned char cmd, unsigned char val);
	void log(char *lpData, ...);
};
//---------------------------------------------------------------------------
#endif
