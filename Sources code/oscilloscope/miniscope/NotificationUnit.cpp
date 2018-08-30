//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "NotificationUnit.h"
#include "Log.h"
#include "ScopedLock.h"
#include "Mutex.h"
#include <SysUtils.hpp>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmNotification *frmNotification;
//---------------------------------------------------------------------------

static Mutex mutex;
std::vector<struct TfrmNotification::ENTRY> TfrmNotification::entries;

__fastcall TfrmNotification::TfrmNotification(TComponent* Owner)
	: TfrmCommon(Owner)
{
#if 0	// test
	struct ENTRY entry;
	entry.timestamp = FormatDateTime("hh:nn:ss", Now());
	entry.type = NOTIFY;
	entry.msg = "Notification type message";
	entries.push_back(entry);
	entry.type = WARNING;
	entry.msg = "Warning type message";
	entries.push_back(entry);
	entry.type = ERR;
	entry.msg = "Error type message";
	entries.push_back(entry);

	lvEntries->Items->Count = entries.size();
	lvEntries->Invalidate();
#endif

	OldListViewProc = lvEntries->WindowProc;
	lvEntries->WindowProc = ListViewWndProc;
}
//---------------------------------------------------------------------------

void TfrmNotification::Add(enum TYPE type, char *szFormat, ...)
{
	ScopedLock<Mutex> lock(mutex);

	va_list ap;
	static char buf[1024]; //determines max message length
	struct ENTRY entry;

	va_start(ap, szFormat);
	int size = vsnprintf(buf, sizeof(buf)-1, szFormat, ap);
	va_end(ap);
	buf[size+1] = 0;

	LOG(E_LOG_TRACE, "Notification type %d: %s", type, buf);

	if (type >= NONE && type < _LIMITER)
	{
		entry.type = type;
	}
	else
	{
		entry.type = NONE;
	}
	entry.timestamp = FormatDateTime("hh:nn:ss", Now());
	entry.msg = buf;
	entries.push_back(entry);
}

void __fastcall TfrmNotification::TimerTimer(TObject *Sender)
{
	ScopedLock<Mutex> lock(mutex);
	if (entries.size() == (unsigned int)lvEntries->Items->Count)
		return;
	lvEntries->Items->Count = entries.size();
	lvEntries->Invalidate();

	this->Show();
	this->BringToFront();
	lvEntries->Items->Item[lvEntries->Items->Count - 1]->MakeVisible(false);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmNotification::lvEntriesData(TObject *Sender,
	  TListItem *Item)
{
	ScopedLock<Mutex> lock(mutex);

	int id = Item->Index;
	Item->Caption = "";
	Item->SubItems->Add(entries[id].timestamp);
	Item->SubItems->Add(entries[id].msg);
	Item->ImageIndex = entries[id].type;
}
//---------------------------------------------------------------------------

void __fastcall TfrmNotification::ListViewWndProc(TMessage &Message)
{
	// hiding both scrollbars
	/** \note Important! This doesn't work when FlatScrollbars is set to true.
	*/
	ShowScrollBar(lvEntries->Handle, SB_HORZ, false);
	ShowScrollBar(lvEntries->Handle, SB_VERT, false);
	OldListViewProc(Message);
}


void __fastcall TfrmNotification::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	ScopedLock<Mutex> lock(mutex);

	entries.clear();
	lvEntries->Items->Count = 0;
	lvEntries->Invalidate();	
}
//---------------------------------------------------------------------------


void __fastcall TfrmNotification::CreateParams(TCreateParams &Params)
{
	 TForm::CreateParams(Params);
	 Params.ExStyle = Params.ExStyle | WS_EX_TOOLWINDOW;
}


