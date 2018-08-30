//---------------------------------------------------------------------------

#ifndef Ms3FrameListUnitH
#define Ms3FrameListUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include "device/Data.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include <ExtCtrls.hpp>

class TfrmMs3FrameList : public TfrmCommon
{
__published:	// IDE-managed Components
	TListView *lvFrames;
	TTimer *tmrRefresh;
	void __fastcall lvFramesData(TObject *Sender, TListItem *Item);
	void __fastcall lvFramesChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
	void __fastcall tmrRefreshTimer(TObject *Sender);
private:	// User declarations
	friend class TfrmMs3Viewer;
	struct
	{
		Data data;					///< parameters of selected frame block count;
									///< most elements are currently unused
		int first;					///< id of the first selected frame
		int count;					///< number of selected frames
	} selection;
public:		// User declarations
	__fastcall TfrmMs3FrameList(TComponent* Owner);
	/** Test if data frame with specified description would be
		consistent with current frames selection */
	bool IsConsistentWithSelection(const Data &data);
	void UpdateView();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMs3FrameList *frmMs3FrameList;
//---------------------------------------------------------------------------
#endif
