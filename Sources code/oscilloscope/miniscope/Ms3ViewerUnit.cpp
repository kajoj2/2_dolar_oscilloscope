//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Ms3ViewerUnit.h"
#include "Ms3FrameListUnit.h"
#include "display\ValueStringFormatter.h"
#include "display\Display.h"
#include "device\MiniscopeDeviceInterface.h"
#include "device\Data.h"
#include "Ms3Recorder.h"
#include "Log.h"

#include <fstream>
#include <time.h>
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMs3Viewer *frmMs3Viewer;
//---------------------------------------------------------------------------

__fastcall TfrmMs3Viewer::TfrmMs3Viewer(TComponent* Owner)
	: TfrmCommon(Owner),
	callbackClose(NULL),
	frame_cnt(0),
	iCurrentChannel(0)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmMs3Viewer::FormClose(TObject *Sender, TCloseAction &Action)
{
	frmMs3FrameList->Close();
	if (callbackClose)
	{
		callbackClose();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMs3Viewer::btnExitViewerClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------

int TfrmMs3Viewer::LoadFile(AnsiString asFileName, CallbackUpdate cbUpdate)
{
	int status = Ms3Recorder::Instance().SetMode(Ms3Recorder::MODE_READ, asFileName);
	if (status)
		return status;
	asCurrentFile = asFileName;
	edFilename->Text = asCurrentFile;
	this->callbackUpdate = cbUpdate;
	frame_cnt = Ms3Recorder::Instance().GetFrameCount();
	if (frame_cnt == 0)
	{
		frmMs3FrameList->selection.first = 0;
		frmMs3FrameList->selection.count = 0;
		frmMs3FrameList->UpdateView();
		//callbackUpdate();
		lblFrameVal->Caption = "---";
		//status = LoadFrames(frmMs3FrameList->selection.first, frmMs3FrameList->selection.count);
		return 0;
	}

	frmMs3FrameList->selection.first = 0;
	frmMs3FrameList->selection.count = 1;
	frmMs3FrameList->UpdateView();
	status = LoadFrames(frmMs3FrameList->selection.first, frmMs3FrameList->selection.count);

	return status;
}

int TfrmMs3Viewer::LoadFrames(int first, int count)
{
	if (count == 0)
	{
		AnsiString asFrameVal;
		asFrameVal.sprintf("--- of %d", frame_cnt);
		lblFrameVal->Caption = asFrameVal;
		lblSamplingVal->Caption = "---";
		lblTriggerTypeVal->Caption = "---";
		lblCouplingTypeVal->Caption = "---";
		lblTimestampVal->Caption = "---";
		lblSamplesCountVal->Caption = "---";
		/** \todo update with no data? */
		//callbackUpdate(data);
		return 0;
	}

	Data tmp;
	int status;
	for (int i=0; i<count; i++)
	{
		status = Ms3Recorder::Instance().GetFrame(first+i, tmp);
		if (status)
		{
			LOG(E_LOG_TRACE, "Failed to get data frame (status = %d)", status);
			return status;
		}
		else
		{
			if (i == 0)
			{
				data.SetChannelCount(tmp.GetChannelCount());
				for (unsigned int ch = 0; ch < tmp.GetChannelCount(); ch++)
				{
					data.getChannel(ch).dataOsc.clear();
					data.getChannel(ch).iCouplingType = tmp.getChannel(ch).iCouplingType;
				}				
			}
			for (unsigned int ch = 0; ch < tmp.GetChannelCount(); ch++)
			{
				// concatenate samples vectors for each channel
				Data::Channel &channel = data.getChannel(ch);
				Data::Channel &src = tmp.getChannel(ch);
				channel.dataOsc.reserve(channel.dataOsc.size() + src.dataOsc.size());
				channel.dataOsc.insert(channel.dataOsc.end(), src.dataOsc.begin(), src.dataOsc.end());
				data.buffer_size = channel.dataOsc.size();
			}
			if (i == 0)
			{
				data.timestamp = tmp.timestamp;
				data.fOscDivX = tmp.fOscDivX;
				data.trigger_source = tmp.trigger_source;
				data.trigger_type = tmp.trigger_type;

				// show metadata from first frame from selection
				float fSamplVal = (float)1.0 / data.fOscDivX;
				lblSamplingVal->Caption =
					ValueStringFormatter::FreqToStr(fSamplVal, 1).c_str();

				lblTriggerTypeVal->Caption = DeviceInterface::GetTriggerTypeName((E_TRIGGER_TYPE)data.trigger_type);
				lblCouplingTypeVal->Caption = DeviceInterface::GetCouplingTypeName((E_COUPLING_TYPE)data.getChannel(iCurrentChannel).iCouplingType);

				char szTime[64] = {0};
				AnsiString asTime;
				if (strftime(szTime, sizeof(szTime), "%Y-%m-%d %T", localtime(&data.timestamp.time)) > 0)
				{
					asTime.sprintf("%s.%03hu", szTime, data.timestamp.millitm);
				}
				lblTimestampVal->Caption = asTime;
				lblSamplesCountVal->Caption = data.buffer_size;
			}
		}
	}

	callbackUpdate(data);

	AnsiString asFrameVal;
	if (count > 1)
	{
		AnsiString fmt = _("%d...%d of %d");	//displayed frame range from ... to ... from total set of ...
		asFrameVal.sprintf(fmt.c_str(), first+1, first+count, frame_cnt);
	}
	else
	{
		AnsiString fmt = _("%d of %d");			//displayed frame ... from set of ...
		asFrameVal.sprintf(fmt.c_str(), first+1, frame_cnt);
	}

	lblFrameVal->Caption = asFrameVal;

	return status;
}

void TfrmMs3Viewer::SetCurrentChannel(unsigned int channel)
{
	iCurrentChannel = channel;
	lblCouplingTypeVal->Caption =
		DeviceInterface::GetCouplingTypeName((E_COUPLING_TYPE)frmMs3FrameList->selection.data.getChannel(iCurrentChannel).iCouplingType);
}            

/** Behaviour: one frame added, one removed from selection (if data looks continuous)
*/
void __fastcall TfrmMs3Viewer::btnPrevFrameClick(TObject *Sender)
{
	if (frame_cnt == 0 || frmMs3FrameList->selection.first <= 0 || frmMs3FrameList->selection.count == 0)
		return;
	int id = frmMs3FrameList->selection.first - 1;
	Data data;
	int status = Ms3Recorder::Instance().GetFrameDesc(id, data);
	if (status)
	{
		LOG(E_LOG_TRACE, "Ms3Viewer: failed to get desc for frame %d", id);
		frmMs3FrameList->selection.first = 0;
		frmMs3FrameList->selection.count = 0;
		LoadFrames(frmMs3FrameList->selection.first, frmMs3FrameList->selection.count);
		frmMs3FrameList->UpdateView();
		return;
	}
	if (!frmMs3FrameList->IsConsistentWithSelection(data))
	{
		frmMs3FrameList->selection.first = id;
		frmMs3FrameList->selection.count = 1;
	}
	else
	{
		frmMs3FrameList->selection.first = id;
	}

	LoadFrames(frmMs3FrameList->selection.first, frmMs3FrameList->selection.count);
	frmMs3FrameList->UpdateView();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMs3Viewer::btnNextFrameClick(TObject *Sender)
{
	if (frame_cnt == 0 ||
		(frmMs3FrameList->selection.first + frmMs3FrameList->selection.count >= (int)frame_cnt) ||
		frmMs3FrameList->selection.count == 0)
		return;
	int id = frmMs3FrameList->selection.first + frmMs3FrameList->selection.count;
	Data data;
	int status = Ms3Recorder::Instance().GetFrameDesc(id, data);
	if (status)
	{
		LOG(E_LOG_TRACE, "Ms3Viewer: failed to get desc for frame %d", id);
		frmMs3FrameList->selection.first = 0;
		frmMs3FrameList->selection.count = 0;
		LoadFrames(frmMs3FrameList->selection.first, frmMs3FrameList->selection.count);
		frmMs3FrameList->UpdateView();
		return;
	}
	if (!frmMs3FrameList->IsConsistentWithSelection(data))
	{
		frmMs3FrameList->selection.first = id;
		frmMs3FrameList->selection.count = 1;
	}
	else
	{
		frmMs3FrameList->selection.first++;
	}

	LoadFrames(frmMs3FrameList->selection.first, frmMs3FrameList->selection.count);
	frmMs3FrameList->UpdateView();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMs3Viewer::btnFrameListClick(TObject *Sender)
{
	frmMs3FrameList->Show();	
}
//---------------------------------------------------------------------------

