//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Ms3FrameListUnit.h"
#include "Ms3ViewerUnit.h"
#include "Ms3Recorder.h"
#include "Log.h"
#include "display\ValueStringFormatter.h"
#include "device\MiniscopeDeviceInterface.h"
#include "Utilities.h"
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMs3FrameList *frmMs3FrameList;
//---------------------------------------------------------------------------
__fastcall TfrmMs3FrameList::TfrmMs3FrameList(TComponent* Owner)
	: TfrmCommon(Owner)
{
}
//---------------------------------------------------------------------------

void TfrmMs3FrameList::UpdateView(void)
{
	lvFrames->OnChange = NULL;
	lvFrames->Selected = NULL;

	lvFrames->Columns->Items[1]->AutoSize = false;

	lvFrames->Items->Count = frmMs3Viewer->frame_cnt;

	for (int i=selection.first; i<selection.first + selection.count; i++)
	{
		lvFrames->Items->Item[i]->Selected = true;
	}
	if (selection.count > 0)
	{
		lvFrames->Items->Item[selection.first]->MakeVisible(true);
		lvFrames->Items->Item[selection.first]->Focused = true;
		Ms3Recorder::Instance().GetFrameDesc(selection.first, selection.data);
	}
	lvFrames->Invalidate();
	lvFrames->OnChange = lvFramesChange;

    ShowScrollBar(lvFrames->Handle, SB_VERT, true);
	lvFrames->Columns->Items[1]->AutoSize = true;
	lvFrames->Width--;
	lvFrames->Width++;
}

void __fastcall TfrmMs3FrameList::lvFramesData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	Data data;
	int status = Ms3Recorder::Instance().GetFrameDesc(id, data);
	if (status)
		return;
	Item->Caption = id + 1;

	char szTime[64] = {0};
	AnsiString asTime;
	if (strftime(szTime, sizeof(szTime), "%Y-%m-%d %T", localtime(&data.timestamp.time)) > 0)
	{
		asTime.sprintf("%s.%03hu", szTime, data.timestamp.millitm);
	}
	Item->SubItems->Add(asTime);

	// Channels/Coupling
	AnsiString txt;
	for (unsigned int i=0; i<data.GetChannelCount(); i++)
	{
		const char* format = (i==0)?("%s/%s"):(" %s/%s");
		txt.cat_printf(format, GetChannelName(data.getChannel(i).id).c_str(),
			DeviceInterface::GetCouplingTypeName((E_COUPLING_TYPE)data.getChannel(i).iCouplingType).c_str());
	}
	Item->SubItems->Add(txt);

	float fSamplVal = (float)1.0 / data.fOscDivX;
	Item->SubItems->Add(ValueStringFormatter::FreqToStr(fSamplVal, 1).c_str());

	Item->SubItems->Add(data.buffer_size);

	// Trigger 
	enum E_TRIGGER_TYPE trigger_type = (E_TRIGGER_TYPE)data.trigger_type;
	txt = DeviceInterface::GetTriggerTypeName(trigger_type);
	if (trigger_type == TRIGGER_SLOPE_UP || trigger_type == TRIGGER_SLOPE_DOWN)
		txt.cat_printf("/CH %s", GetChannelName(data.trigger_source).c_str());
	Item->SubItems->Add(txt);


}
//---------------------------------------------------------------------------


void __fastcall TfrmMs3FrameList::lvFramesChange(TObject *Sender,
	  TListItem *Item, TItemChange Change)
{
	/** \note Behavior:
		Form allows to select several consecutive data frames if some criteria
		are met. 
		If criteria are not met or selected frame is not located next to previously
		selected then all previously selected frames are deselected.
	*/
	
	if (Change != ctState)
		return;
	if (!Item)
	{
		//LOG(E_LOG_TRACE, "Item == NULL");
		return;
	}

	TListView *lv = dynamic_cast<TListView*>(Sender);
	assert(lv);

	lv->OnChange = NULL;

	int changed = Item->Index;	// important; Item value changes even when reading listview state

	//LOG(E_LOG_TRACE, "Changed item #%02d, currently selected: from %d, %d items", changed, selection.first, selection.count);


	lv->OnSelectItem = NULL;	// clear current handler

	if (!lv->Selected)
	{
		// no item selected on the list
		selection.first = 0;
		selection.count = 0;
		//LOG(E_LOG_TRACE, "lv->Selected == NULL");
	}
	else if (lv->SelCount == 1)
	{
		//LOG(E_LOG_TRACE, "lv->SelCount == 1");

		int id = lv->Selected->Index;
		int status = Ms3Recorder::Instance().GetFrameDesc(id, selection.data);
		if (status)
		{
			lv->Selected = NULL;
			selection.first = 0;
			selection.count = 0;
		}
		else
		{
			selection.first = id;
			selection.count = 1;
		}
	}
	else
	{
		int start, end;
		if (selection.first > changed)
		{
			start = changed;
			end = selection.first + selection.count - 1;
		}
		else
		{
			start = selection.first;
			end = std::max(changed, selection.first + selection.count - 1);
		}
		bool bDiscontinuous = false;
		for (int i=start+1; i<=end-1; i++)
		{
			if (lv->Items->Item[i]->Selected == false)
			{
				bDiscontinuous = true;
				break;
			}
		}
		if (!bDiscontinuous)
		{
			if (selection.first > changed)
			{
				start = changed;
				end = selection.first - 1;
			}
			else
			{
				start = selection.first + selection.count;
				end = changed;
			}
			for (int i=start; i<=end; i++)
			{
				Data data;
				int status = Ms3Recorder::Instance().GetFrameDesc(i, data);
				if (status || !IsConsistentWithSelection(data))
				{
					bDiscontinuous = true;
					break;
				}
				if (i == start && i == changed)
				{
					// update timestamp
					selection.data = data;
				}
			}
		}
		if (bDiscontinuous)
		{
			// selected frames are not next to each other - select only newly selected frame
			lv->Selected = NULL;
			lv->Items->Item[changed]->Selected = true;
			Data data;
			int status = Ms3Recorder::Instance().GetFrameDesc(changed, data);
			if (status)
			{
				selection.first = 0;
				selection.count = 0;
			}
			else
			{
				selection.first = changed;
				selection.count = 1;
			}
			tmrRefresh->Enabled = true;
			lv->OnChange = lvFramesChange;	// restore previous handler
			return;
		}
		selection.first = lv->Selected->Index;
		selection.count = lv->SelCount;
	}

	tmrRefresh->Enabled = true;
	lv->OnChange = lvFramesChange;
}
//---------------------------------------------------------------------------

bool TfrmMs3FrameList::IsConsistentWithSelection(const Data &data)
{
	// check frame parameters consistency
	if (data.fOscDivX != selection.data.fOscDivX ||
		data.GetChannelCount() != selection.data.GetChannelCount()
		)
	{
		return false;
	}
	for (unsigned int ch=0; ch<data.GetChannelCount(); ch++)
	{
		if (data.getChannelConst(ch).id != selection.data.getChannel(ch).id)
		{
			return false;
		}
	}
	return true;
}

void __fastcall TfrmMs3FrameList::tmrRefreshTimer(TObject *Sender)
{
	tmrRefresh->Enabled = false;
	frmMs3Viewer->LoadFrames(selection.first, selection.count);
}
//---------------------------------------------------------------------------

