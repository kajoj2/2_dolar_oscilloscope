/** \file
*/ 

#ifndef NotificationUnitH
#define NotificationUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include <vector>

/** \brief Window intended to display important messages in a non-blocking way
*/
class TfrmNotification : public TfrmCommon
{
__published:	// IDE-managed Components
	TListView *lvEntries;
	TImageList *imgList;
	TTimer *Timer;
	void __fastcall lvEntriesData(TObject *Sender, TListItem *Item);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TimerTimer(TObject *Sender);
public:		// User declarations
	__fastcall TfrmNotification(TComponent* Owner);
	/** \brief Type of message (icon at message)
	*/
	enum TYPE
	{
		NONE = -1,
		NOTIFY,
		WARNING,
		ERR,
		_LIMITER
	};
	/** \brief Add new notification
		\param type Icon displayed next to notification
		\param szFormat Notification string formatting (printf-like)
	*/
	static void Add(enum TYPE type, char *szFormat, ...);
private:	// User declarations
	struct ENTRY
	{
		enum TYPE type;
        AnsiString timestamp;
		AnsiString msg;
	};
	static std::vector<struct ENTRY> entries;

	void __fastcall ListViewWndProc(TMessage &Message);
	Classes::TWndMethod OldListViewProc;
	virtual void __fastcall CreateParams(TCreateParams &Params);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmNotification *frmNotification;
//---------------------------------------------------------------------------
#endif
