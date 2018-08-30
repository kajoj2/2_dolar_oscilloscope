/** \file
 *  \brief Main form module
 */

/* Copyright (C) 2008-2010 Tomasz Ostrowski <tomasz.o.ostrowski at gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 */

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "Mode.h"
#include "display\DisplayOpenGL.h"
#include "display\ValueStringFormatter.h"
#include "FFT.h"
#include "FFTWindowFactory.h"
#include "Log.h"
#include "LogUnit.h"
#include "ControlUnit.h"
#include "NotificationUnit.h"
#include "ScopedLock.h"
#include "Mutex.h"
#include "AboutUnit.h"

#include "device\MiniscopeDeviceInterface.h"
#include "device\Data.h"

#include "SettingsUnit.h"
#include "Settings.h"

#include "Recorder.h"

#include "MeasurementsUnit.h"
#include "StatisticsUnit.h"
#include "CalibrateSensitivityUnit.h"
#include "PlotToolbarUnit.h"
#include "WaitUnit.h"
#include "JsonViewerUnit.h"
#include "Ms3ViewerUnit.h"
#include "Ms3Recorder.h"
#include "Ms3FileRepairUnit.h"
#include "FilterSelector.h"
#include "DataFilterChain.h"
#include "ImageExport.h"

#include "Utilities.h"
#include "TimeCounter.h"
#include "CompileTimeChecker.h"

#include "Rounding.h"

#include "Hotkeys.h"
#include "OS.h"
#include "Action.h"

#include <SysUtils.hpp>
#include <ClipBrd.hpp>
#include <math.h>
#include <limits>
#include <algorithm>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
Settings appSettings;   ///< application settings instance

namespace {
	HotKeys hotKeys;
	DataFilterChain filterChain;	
}

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TfrmCommon(Owner),
	fZoomChangeGrid(1.2f),
	device(NULL),
	bPaintUpdate(false),
	bFrameProgressUpdate(false),
	bTriggered(false),
	bJsonViewerMode(false),
	bMs3ViewerMode(false)
{
	CLog::Instance()->SetFile(ChangeFileExt(Application->ExeName, ".log").c_str());
	CLog::Instance()->LogToFile(appSettings.Logging.bLogToFile);
	CLog::Instance()->SetLevel(E_LOG_TRACE);

    StatusBar->DoubleBuffered = true;

	display = new DisplayOpenGL(&current, pnlCenter->Handle);
	display->addObserver(*this);

	btnModeScope->Down = true;

	display->ChangeSize(pnlCenter->Width, pnlCenter->Height);

	SetMode(MODE_OSCILLOSCOPE);

	// load custom cursors
	Screen->Cursors[CURSOR_HAND] = LoadCursor(HInstance, "HANDCUR");
	Screen->Cursors[CURSOR_ZOOM] = LoadCursor(HInstance, "ZOOMCUR");

	mouse_tool = MOUSE_CURSOR;
	pnlCenter->Cursor = crCross;
	btnMouseCursor->Down = true;

	bMovingPlot = false;

	float fValX, fValY;
	display->GetCursorPhysical(Display::CURSOR1, fValX, fValY);
	UpdateCursorReadings(MODE_OSCILLOSCOPE, 1, fValX, fValY);
	display->GetCursorPhysical(Display::CURSOR2, fValX, fValY);
	UpdateCursorReadings(MODE_OSCILLOSCOPE, 2, fValX, fValY);

	bDataDivXUpdate = false;

	Timer->Interval = 50;

	//STATIC_CHECK(sizeof(int) <= sizeof(char), Just_Testing);
}
//---------------------------------------------------------------------------

__fastcall TfrmMain::~TfrmMain()
{
	FFT::SetWindowType("");	// cleanup (makes CG happy)
	CLog::Instance()->Destroy();
	FFTWindowFactory::Destroy();
}

void __fastcall TfrmMain::pnlCenterResize(TObject *Sender)
{
	if (display)
		display->ChangeSize(pnlCenter->Width, pnlCenter->Height);
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::TimerTimer(TObject *Sender)
{
	Timer->Enabled = false;

	ScopedLock<Mutex> lock(mutexDeviceData);

	if (bTriggered)
	{
		frmControl->ProgressBarFrameRx->Visible = true;
	}
    unsigned int iExpectedFrameSize = current.buffer_size;
	if (bDataUpdate)
	{
		if (bDataDivXUpdate)
		{
            bDataDivXUpdate = false;
			display->SetData(&current);
			if (appSettings.Zoom.bKeepRound)
			{
				float fZoomX, fZoomY;
				display->GetZoom(fZoomX, fZoomY);
				float fDivX = display->GetDivX();
				if (IsRoundingNeeded(fDivX))
				{
					float fZoomX1 = fZoomX * fDivX/GetNextRoundLowerValue(fDivX);
					float fZoomX2 = fZoomX * fDivX/GetNextRoundHigherValue(fDivX);
					if (fZoomX1/fZoomX < fZoomX/fZoomX2)
					{
						fZoomX = fZoomX1;
					}
					else
					{
						fZoomX = fZoomX2;
					}
					display->SetZoom(Display::OFFSET_LEAVE, Display::OFFSET_LEAVE, fZoomX, fZoomY);
				}
			}
		}

		for (unsigned int i=0; i<current.GetChannelCount(); i++) {
			if (current.getChannel(i).bDivYChanged)
			{
				current.getChannel(i).bDivYChanged = false;
				if (appSettings.Zoom.bKeepRound)
				{
					float fZoomX, fZoomY;
					display->GetZoom(fZoomX, fZoomY);
					float fDivY = display->GetDivY();
					if (IsRoundingNeeded(fDivY))
					{
						float fZoomY1 = fZoomY * fDivY/GetNextRoundLowerValue(fDivY);
						float fZoomY2 = fZoomY * fDivY/GetNextRoundHigherValue(fDivY);
						if (fZoomY1/fZoomY < fZoomY/fZoomY2)
						{
							fZoomY = fZoomY1;
						}
						else
						{
							fZoomY = fZoomY2;
						}
						display->SetZoom(Display::OFFSET_LEAVE, Display::OFFSET_LEAVE, fZoomX, fZoomY);
					}
				}
			}
		}

		for (unsigned int i=0; i<current.GetChannelCount(); i++) {
			filterChain.Process(current.getChannel(i).dataOsc);
		}

		if (mode == MODE_SPECTRUM)
		{
			if (current.bFrameFilled)
			{
				if (frmMeasurements->Visible)
					frmMeasurements->Process(current, CurrentChannel());
				CalculateSpectrum();
			}
		}
		else if (mode == MODE_RECORDER)
		{
			if (current.bFrameFilled)
			{
				if (frmMeasurements->Visible)
					frmMeasurements->Process(current, CurrentChannel());
				std::vector<float>::const_iterator it;
				for (unsigned int i=0; i<current.GetChannelCount(); i++)
				{
					Data::Channel& channel = current.getChannel(i);
					float fAvg = 0;
					if (channel.dataOsc.size() > 0)
					{
						for (it = channel.dataOsc.begin(); it != channel.dataOsc.end(); ++it)
						{
							fAvg += (*it);
						}
						fAvg /= channel.dataOsc.size();
					}
					channel.dataRec.push_back(fAvg);
				}
			}
		}
		else if (mode == MODE_OSCILLOSCOPE)
		{
			if (frmMeasurements->Visible)
				frmMeasurements->Process(current, CurrentChannel());
		}
	}
#if 1
	if (bFrameProgressUpdate || bDataUpdate)
	{
		if (iExpectedFrameSize == 0)
		{
			frmControl->ProgressBarFrameRx->Position = frmControl->ProgressBarFrameRx->Min;
		}
		else
		{
			if (current.bFrameFilled)
				frmControl->ProgressBarFrameRx->Position = device->rx_progress;
			else
				frmControl->ProgressBarFrameRx->Position = 100*current.getEnabledChannel().dataOsc.size()/iExpectedFrameSize;
		}

		if (frmControl->ProgressBarFrameRx->Position == frmControl->ProgressBarFrameRx->Max)
		{
			frmControl->ProgressBarFrameRx->Position = frmControl->ProgressBarFrameRx->Min;
			frmControl->ProgressBarFrameRx->Visible = false;
			bTriggered = false;

			if (current.trigger_mode == TRIGGER_MODE_SINGLE)
			{
				frmControl->btnStart->Enabled = true;
				frmControl->btnStop->Enabled = false;
			}
		}
		frmControl->ProgressBarFrameRx->Update();
		bFrameProgressUpdate = false;
	}
#endif
	if (bDataUpdate || bPaintUpdate)
	{
		//TimeCounter tc("Display update");
		display->Update(Display::UPDATE_ALL);
		bPaintUpdate = false;
		bDataUpdate = false;
	}
	Timer->Enabled = true;
}
//---------------------------------------------------------------------------

void TfrmMain::SetCapabilities(const struct S_SCOPE_CAPABILITIES* const capabilities)
{
	assert(capabilities->iChannelCount > 0);
	current.SetChannelCount(capabilities->iChannelCount);
	device->upcoming.SetChannelCount(capabilities->iChannelCount);

	UseData(current);
	ActionModeRecorder->Enabled = device->hasManualTrigger();
	frmControl->SetCapabilities(capabilities);
}
//---------------------------------------------------------------------------

void TfrmMain::CalculateSpectrum(void)
{
	unsigned int count = FFT::NearestPowerOfTwo(current.getEnabledChannel().dataOsc.size());
	if (count > appSettings.FFT.iSamplesLimit)
		count = appSettings.FFT.iSamplesLimit;
	FFT::SetSamplesCount(count);
	float *ReIn = new float[count];
	float *ReOut = new float[count];
	float *ImOut = new float[count];

	for (unsigned int channel_id=0; channel_id<current.GetChannelCount(); channel_id++)
	{
		std::vector<float> &samples = current.getChannel(channel_id).dataOsc;
		std::vector<float> &fftv = current.getChannel(channel_id).dataFFT;
		if (samples.size() < count)
		{
			// skip disabled channel
			fftv.clear();
			continue;
		}
		for(unsigned int i=0;i < count;i++)
			ReIn[i]= samples[i];
		FFT::Transform(ReIn, ReOut, ImOut);
		fftv.resize(count/2);
		//LOG(E_LOG_TRACE, "Channel %d:", channel_id);
		for(unsigned int i=0;i < count/2;i++)
		{
			fftv[i] = sqrt((ReOut[i]*ReOut[i])+(ImOut[i]*ImOut[i]))/count * 2;
			//LOG(E_LOG_TRACE, "%f", fftv[i]);
		}
		if (count > 0) {
			fftv[0] /= 2.0f;
		}
	}
	delete [] ReIn;
	delete [] ReOut;
	delete [] ImOut;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::pnlCenterMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbMiddle)
	{
		if (!frmPlotToolbar->Visible) {
			POINT P = ClientToScreen(Point(pnlCenter->Left + X, pnlCenter->Top + Y));
			frmPlotToolbar->ShowAt(P.x, P.y);
		} else
			frmPlotToolbar->Hide();
		return;
	}

	if (mouse_tool == MOUSE_CURSOR )
	{
		float fValX, fValY;
		if(Button==mbLeft)  //left button clicked
		{
			display->SetCursor1(X, Y);
			display->PosToPhysical(X, Y, fValX, fValY);
			UpdateCursorReadings(mode, 1, fValX, fValY);
		}
		else if(Button==mbRight)
		{
			display->SetCursor2(X, Y);
			display->PosToPhysical(X, Y, fValX, fValY);
			UpdateCursorReadings(mode, 2, fValX, fValY);
		}
	}
	else if (mouse_tool == MOUSE_OFFSET)
	{
		iPlotMoveStartX = X;
		iPlotMoveStartY = Y;
		bMovingPlot = true;
	}
	else if (mouse_tool == MOUSE_ZOOM || mouse_tool == MOUSE_ZOOM_X || mouse_tool == MOUSE_ZOOM_Y)
	{
		float fZoomX, fZoomY;
		display->GetZoom(fZoomX, fZoomY);
		float fDivX = display->GetDivX();
		float fDivY = display->GetDivY();
		if(Button==mbLeft)  //left button clicked
		{
			if (mouse_tool != MOUSE_ZOOM_Y)
			{
				if (Shift.Contains(ssShift))
				{
					fZoomX *= fZoomChangeGrid;
				}
				else
				{
					fZoomX = fZoomX * fDivX/GetNextRoundLowerValue(fDivX);
				}
			}
			if (mouse_tool != MOUSE_ZOOM_X)
			{
				if (Shift.Contains(ssShift))
				{
					fZoomY *= fZoomChangeGrid;
				}
				else
				{
					fZoomY = fZoomY * fDivY/GetNextRoundLowerValue(fDivY);
				}
			}
		}
		else if(Button==mbRight)
		{
			if (mouse_tool != MOUSE_ZOOM_Y)
			{
				if (Shift.Contains(ssShift))
				{
					fZoomX /= fZoomChangeGrid;
				}
				else
				{
					fZoomX = fZoomX * fDivX/GetNextRoundHigherValue(fDivX);
				}
			}
			if (mouse_tool != MOUSE_ZOOM_X)
			{
				if (Shift.Contains(ssShift))
				{
					fZoomY /= fZoomChangeGrid;
				}
				else
				{
					fZoomY = fZoomY * fDivY/GetNextRoundHigherValue(fDivY);
				}
			}
		}
		display->SetZoom((mouse_tool==MOUSE_ZOOM_Y)?(Display::OFFSET_LEAVE):(X),
			(mouse_tool==MOUSE_ZOOM_X)?(Display::OFFSET_LEAVE):(Y),
			fZoomX, fZoomY);
		UpdateZoomCfg();
	}
	bPaintUpdate = true;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::pnlCenterMouseUp(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	if (bMovingPlot)
	{
		if (appSettings.Graph.bSmoothTraceMoving)
		{
			display->Move(iPlotMoveStartX, iPlotMoveStartY, X, Y);
			bPaintUpdate = true;
        }
		bMovingPlot = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pnlCenterMouseMove(TObject *Sender, TShiftState Shift,
	  int X, int Y)
{
	if (bMovingPlot && appSettings.Graph.bSmoothTraceMoving)
	{
		display->Move(iPlotMoveStartX, iPlotMoveStartY, X, Y);
		bPaintUpdate = true;
		iPlotMoveStartX = X;
		iPlotMoveStartY = Y;		
	}
}
//---------------------------------------------------------------------------

void TfrmMain::UpdateCursorReadings(enum E_MODE mode, int cursorId, float fValX, float fValY)
{
	static float fValX1 = 0.0, fValY1 = 0.0, fValX2 = 0.0, fValY2 = 0.0;

	if (mode==MODE_OSCILLOSCOPE || mode==MODE_RECORDER)
	{
		if (cursorId==1)
		{
			StatusBar->Panels->Items[0]->Text =
				(ValueStringFormatter::TimeToStr(fValX, 3) + " " +
				ValueStringFormatter::VoltageToStr(fValY, 3)).c_str();
			fValX1 = fValX;
			fValY1 = fValY;
		}
		else if (cursorId==2)
		{
			StatusBar->Panels->Items[1]->Text =
				(ValueStringFormatter::TimeToStr(fValX, 3) + " " +
				ValueStringFormatter::VoltageToStr(fValY, 3)).c_str();
			fValX2 = fValX;
			fValY2 = fValY;
		}
		float dx = fabs(fValX1-fValX2);
		float dy = fabs(fValY1-fValY2);
		StatusBar->Panels->Items[2]->Text =
			((std::string)"dx=" + ValueStringFormatter::TimeToStr(dx, 3) + " dy=" +
			ValueStringFormatter::VoltageToStr(dy, 3)).c_str();
		if (dx != 0)
		{
			StatusBar->Panels->Items[2]->Text = StatusBar->Panels->Items[2]->Text +
				" 1/dx=" + ValueStringFormatter::FreqToStr(1/dx, 3).c_str();
		}
	}
	else
	{
		if (cursorId==1)
		{
			StatusBar->Panels->Items[0]->Text =
				(ValueStringFormatter::FreqToStr(fValX, 3) + "  " +
				ValueStringFormatter::VoltageToStr(fValY, 3)).c_str();
			fValX1 = fValX;
			fValY1 = fValY;
		}
		else if (cursorId==2)
		{
			StatusBar->Panels->Items[1]->Text =
				(ValueStringFormatter::FreqToStr(fValX, 3) + "  " +
				ValueStringFormatter::VoltageToStr(fValY, 3)).c_str();
			fValX2 = fValX;
			fValY2 = fValY;
		}
		float dx = fabs(fValX1-fValX2);
		float dy = fabs(fValY1-fValY2);
		StatusBar->Panels->Items[2]->Text =
			((std::string)"dx=" + ValueStringFormatter::FreqToStr(dx, 3) + "  dy=" +
			ValueStringFormatter::VoltageToStr(dy, 3)).c_str();
	}
}


void __fastcall TfrmMain::ActionChangeMouseModeCursorExecute(TObject *Sender)
{
	mouse_tool = MOUSE_CURSOR;
	pnlCenter->Cursor = crCross;
	btnMouseCursor->Down = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionChangeMouseModeOffsetExecute(TObject *Sender)
{
	mouse_tool = MOUSE_OFFSET;
	pnlCenter->Cursor = (TCursor)CURSOR_HAND;
	btnMouseMove->Down = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionChangeMouseModeZoomExecute(TObject *Sender)
{
	mouse_tool = MOUSE_ZOOM;
	pnlCenter->Cursor = (TCursor)CURSOR_ZOOM;
	btnMouseZoomXY->Down = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionChangeMouseModeZoomXExecute(TObject *Sender)
{
	mouse_tool = MOUSE_ZOOM_X;
	pnlCenter->Cursor = (TCursor)CURSOR_ZOOM;
	btnMouseZoomX->Down = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionChangeMouseModeZoomYExecute(TObject *Sender)
{
	mouse_tool = MOUSE_ZOOM_Y;
	pnlCenter->Cursor = (TCursor)CURSOR_ZOOM;
	btnMouseZoomY->Down = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionZoomClearExecute(TObject *Sender)
{
	display->SetZoom(Display::OFFSET_CLEAR, Display::OFFSET_CLEAR,
		1.0, (mode == MODE_SPECTRUM)?(2.0):(1.0));
	bPaintUpdate = true;
	UpdateZoomCfg();
	btnZoomClear->Down = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionModeScopeExecute(TObject *Sender)
{
	SetMode(MODE_OSCILLOSCOPE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionModeSpectrumExecute(TObject *Sender)
{
	SetMode(MODE_SPECTRUM);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionModeRecorderExecute(TObject *Sender)
{
	SetMode(MODE_RECORDER);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miAboutClick(TObject *Sender)
{
	AboutBox->ShowModal();
}
//---------------------------------------------------------------------------



void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	this->Width = appSettings.frmMain.iWidth;
	this->Height = appSettings.frmMain.iHeight;
	this->Top = appSettings.frmMain.iPosY;
	this->Left = appSettings.frmMain.iPosX;
	if (appSettings.frmMain.bAlwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	if (appSettings.frmMain.bWindowMaximized)
		this->WindowState = wsMaximized;
	FFT::SetWindowType(appSettings.FFT.asWindowName);

	TMenuItem* mi = new TMenuItem(miLanguage);
	mi->Caption = "en";
	mi->OnClick = miSelectLangClick;
	mi->Checked = (mi->Caption == appSettings.frmMain.asLanguage);
	mi->RadioItem = true;
	miLanguage->Add(mi);
#if 0
	Classes::TStrings* list = new Classes::TStringList();
	DefaultInstance->GetListOfLanguages("default", list);
	for (int i=0; i<list->Count; i++)
	{
		TMenuItem* mi = new TMenuItem(miLanguage);
		mi->Caption = list->Strings[i];
		mi->OnClick = miSelectLangClick;
		mi->Checked = (mi->Caption == appSettings.asLanguage);
		mi->RadioItem = true;
		miLanguage->Add(mi);
	}
	delete list;
#endif
	UpdateMruMenu();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miSelectLangClick(TObject *Sender)
{
	TMenuItem *clicked;
	clicked = dynamic_cast<TMenuItem*>(Sender);
	if (!clicked)
		return;
	// deselect all menu items
	for (int i = miLanguage->Count - 1; i>=0; --i)
	{
		miLanguage->Items[i]->Checked = false;
	}
	clicked->Checked = true;
	appSettings.frmMain.asLanguage = clicked->Caption;

#if 0
	// we could try to re-translate, but complete re-translation
	// would require keeping track of i.e. trigger names added to combobox and
	// recreating them each time language is switched
	//UseLanguage(appSettings.asLanguage);
	//RetranslateComponent(this);
#else
	// I think this message should not be translated
	ShowMessage("New language will be used after restart.");
#endif
}

void __fastcall TfrmMain::miDeviceSettingsClick(TObject *Sender)
{
	device->ShowSettings();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miViewLogClick(TObject *Sender)
{
	frmLog->Visible = miViewLog->Checked;
	if (frmLog->Visible)
		frmLog->BringToFront();
}
//---------------------------------------------------------------------------

void TfrmMain::CallbackCloseLogFn(void)
{
	miViewLog->Checked = false;
}

void TfrmMain::CallbackDataFn(void)
{
	/** \note DO NOT use VCL code inside callbacks called by device dll */
	ScopedLock<Mutex> lock(mutexDeviceData);
	if (bJsonViewerMode || bMs3ViewerMode)
		return;

	if (current.bFrameFilled)
	{
		if (device->upcoming.bFrameFilled)
		{
			if (current.fOscDivX != device->upcoming.fOscDivX)
				bDataDivXUpdate = true;
			current = device->upcoming;
			bDataUpdate = true;
			if (Ms3Recorder::Instance().GetMode() == Ms3Recorder::MODE_WRITE)
			{
				Ms3Recorder::Instance().StoreFrame(current);
				if (Ms3Recorder::Instance().GetFileSize() > (unsigned int)appSettings.Ms3Recording.iSizeLimit*1024)
				{
					AnsiString msg = _("MS3 file size reached (check settings), recording stopped");
					TfrmNotification::Add(TfrmNotification::WARNING, msg.c_str());
					ActionMs3Record->Execute();
				}
			}
		}
		else
		{
			bFrameProgressUpdate = true;
		}
	}
	else
	{
   		if (current.fOscDivX != device->upcoming.fOscDivX)
			bDataDivXUpdate = true;
		current.fOscDivX = device->upcoming.fOscDivX;
		current.timestamp = device->upcoming.timestamp;
		current.trigger_mode = device->upcoming.trigger_mode;
		current.trigger_source = device->upcoming.trigger_source;
		current.trigger_type = device->upcoming.trigger_type;
		current.buffer_size = device->upcoming.buffer_size;
		for (unsigned int i=0; i<device->upcoming.GetChannelCount(); i++)
		{
			Data::Channel& channelDst = current.getChannel(i);
			Data::Channel& channelSrc = device->upcoming.getChannel(i);
			if (channelSrc.bEnabled == false)
			{
				continue;
			}
			int end = channelSrc.dataOsc.size();
			for (int i=channelDst.dataOsc.size(); i<end; i++)
			{
				channelDst.dataOsc.push_back(channelSrc.dataOsc[i]);
			}
		}
		if (device->upcoming.bFrameFilled)
		{
            current.bFrameFilled = true;
			if (Ms3Recorder::Instance().GetMode() == Ms3Recorder::MODE_WRITE)
				Ms3Recorder::Instance().StoreFrame(current);
        }
		bDataUpdate = true;
	}
}

void TfrmMain::CallbackTriggerFn(void)
{
	ScopedLock<Mutex> lock(mutexDeviceData);
	if (bJsonViewerMode || bMs3ViewerMode)
		return;

	/** \note DO NOT use VCL code inside callbacks called by device dll */

	if (current.fOscDivX != device->upcoming.fOscDivX ||
		current.trigger_type != device->upcoming.trigger_type ||
/** \todo multichannel: disable channels */
//		memcmp(&current.channels[0], &upcoming.channels[0], current.channels.size() * sizeof(current.channels[0])),
		current.buffer_size != device->upcoming.buffer_size)
	{
		// clear display if any acquisition settings changed
		for (unsigned int i=0; i<current.GetChannelCount(); i++)
		{
			Data::Channel& channel = current.getChannel(i);
			channel.dataOsc.clear();
			channel.dataOsc.reserve(device->upcoming.buffer_size);
		}
		current.bFrameFilled = false;
		bDataUpdate = true;
		if (current.fOscDivX != device->upcoming.fOscDivX)
			bDataDivXUpdate = true;
		current = device->upcoming;
	}

	bTriggered = true;
}

void TfrmMain::CallbackConnectFn(int state, const char *szMsgText)
{
	tmrUpdateConnState->Enabled = true;
}

void TfrmMain::UpdateConnectionState(void)
{
	struct ConnectionInfo conn = device->GetConnectionInfo();
	frmControl->pnlOscControl->Enabled = false;
	switch (conn.state)
	{
	case DEVICE_DISCONNECTED:
		miDeviceConnect->Enabled = true;
		break;
	case DEVICE_CONNECTING:
		break;
	case DEVICE_CONNECTED:
		frmControl->pnlOscControl->Enabled = true;
		miDeviceDisconnect->Enabled = true;
		break;
	case DEVICE_DISCONNECTING:
		break;
	default:
		break;
	}
    StatusBar->Panels->Items[3]->Text = conn.msg;
}

void __fastcall TfrmMain::tmrUpdateConnStateTimer(TObject *Sender)
{
	tmrUpdateConnState->Enabled = false;
	UpdateConnectionState();

	struct ConnectionInfo conn = device->GetConnectionInfo();
	if (conn.state == DEVICE_CONNECTED)
	{
		// start acquisition by default
		device->Run(true);
		frmControl->btnStart->Enabled = false;
		frmControl->btnStop->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::PaintBoxCenterPaint(TObject *Sender)
{
	/** \note PaintBoxCenter purpose is to fire OnPaint event
		to request graph repaint (TPanel has no OnPaint event but
		TPaintBox has no Handle)
    */
	bPaintUpdate = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::PaintBoxCenterMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	pnlCenterMouseDown(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::PaintBoxCenterMouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y)
{
	pnlCenterMouseMove(Sender, Shift, X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::PaintBoxCenterMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	pnlCenterMouseUp(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	Timer->Enabled = false;

	hotKeys.Unregister(Handle);

	int status = Ms3Recorder::Instance().SetMode(Ms3Recorder::MODE_IDLE);
	ReportError(status, ((AnsiString)(_("Failed to close MS3 recorder/player."))).c_str());

	for (int i=0; i<display->GetNumberOfChannels(); i++)
	{
		float offsetY = display->GetScopeChannelOffsetY(i);
		appSettings.PlotPosition.SetY(i, offsetY);
	}

	delete display;
	if (device)
	{
		device->SaveSettings(&device->settings);
		device->Disconnect();
		delete device;
		device = NULL;
	}

	filterChain.Release();

	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".json" );
	appSettings.frmMain.bWindowMaximized = (this->WindowState == wsMaximized);
	if (!appSettings.frmMain.bWindowMaximized)
	{
		// these values are meaningless is wnd is maximized
		appSettings.frmMain.iWidth = this->Width;
		appSettings.frmMain.iHeight = this->Height;
		appSettings.frmMain.iPosY = this->Top;
		appSettings.frmMain.iPosX = this->Left;
	}

	appSettings.Measurements.iPosX = frmMeasurements->Left;
	appSettings.Measurements.iPosY = frmMeasurements->Top;
	appSettings.Measurements.bVisible = frmMeasurements->Visible;

	appSettings.Write(asConfigFile);

	CanClose = true;
}
//---------------------------------------------------------------------------

void TfrmMain::LoadDevice(AnsiString dllname)
{
    // get rid of artifacts first
	frmMain->Repaint();
	display->Update(Display::UPDATE_ALL);
	frmWait->Show(_("Loading input device dll, please wait..."));

//	ToolBarMode->Enabled = false;
	if (device)
	{
		DisconnectDevice();
		delete device;
		device = NULL;
	}
	device = new DeviceInterface(dllname);
	if (device->Load())
	{
		ShowMessage(_("Failed to load device dll."));
		delete device;
		device = NULL;
		frmWait->Close();
		return;
	}
	device->callbackData = CallbackDataFn;
	device->callbackConnect = CallbackConnectFn;
	device->callbackTrigger = CallbackTriggerFn;

	current.fOscDivX = device->capabilities->fSamplingPeriod[0];
	display->SetData(&current);
	frmControl->Configure(&mutexDeviceData, &current, device, display, &bDataUpdate, &bPaintUpdate);
	frmControl->OnUpdateTriggerMode = UpdateDisplayVisibility;
	SetCapabilities(device->capabilities);

	int current_channel = frmControl->tabOscControlTop->TabIndex;
	if (device->GetSettings(&device->settings) == 0)
	{
		frmControl->trbarSensitivity->Position = device->settings.pSensitivity[current_channel];
		frmControl->trbarSampling->Position = device->settings.iSamplingPeriod;
		frmControl->trbarTriggerLevel->Position = device->settings.iTriggerLevel;
		frmControl->trbarTriggerPosition->Position = device->settings.iTriggerPosition;
		for (unsigned int i=0; i<device->capabilities->iTriggerTypeCount; i++)
		{
			if (device->capabilities->iTriggerTypes[i] == device->settings.eTriggerType)
			{
				frmControl->cbTriggerType->ItemIndex = i;
				break;
			}
		}
		for (unsigned int i=0; i<device->capabilities->iTriggerModeCount; i++)
		{
			if (device->capabilities->iTriggerModes[i] == device->settings.eTriggerMode)
			{
				frmControl->cbTriggerMode->ItemIndex = i;
				break;
			}
		}
		for (unsigned int i=0; i<device->capabilities->iCouplingTypeCount; i++)
		{
			if (device->capabilities->iCouplingType[i] == device->settings.eCouplingType[current_channel])
			{
				frmControl->cbCouplingType->ItemIndex = i;
				break;
			}
		}
		frmControl->chbChannelEnable->Checked = device->settings.pChannelEnable[current_channel];		
		frmControl->cbBufferSize->ItemIndex = frmControl->cbBufferSize->Items->IndexOf(device->settings.iBufferSize);
		if (frmControl->cbBufferSize->ItemIndex < 0)
			frmControl->cbBufferSize->ItemIndex = 0;
		frmControl->cbTriggerSource->ItemIndex = device->settings.iTriggerSrcChannel;

		current.fOscDivX = device->capabilities->fSamplingPeriod[device->settings.iSamplingPeriod];
		display->SetData(&current);
	}
	else
	{
		frmControl->cbTriggerSource->ItemIndex = 0;
		frmControl->cbBufferSize->ItemIndex = 0;
		frmControl->chbChannelEnable->Checked = true;
	}

	ActionCalibrateSensitivity->Enabled = device->hasCalibration();
	device->calibration.clear();
	if (device->hasCalibration())
	{
		for (unsigned int i=0; i<device->capabilities->iChannelCount; i++)
		{
			device->calibration.push_back ( std::vector<struct DEV_CALIBRATION>() );
			for (unsigned int j=0; j<device->capabilities->iSensitivityLevelsCount; j++)
			{
				struct DEV_CALIBRATION cal = {0, 1.0};
				device->GetCalibration(i, j, &cal.offset, &cal.gain);
				device->calibration[i].push_back(cal);
			}
		}
		//ApplyCalibration();
	} else {
		for (unsigned int i=0; i<device->capabilities->iChannelCount; i++)
		{
            device->calibration.push_back ( std::vector<struct DEV_CALIBRATION>() );
			for (unsigned int j=0; j<device->capabilities->iSensitivityLevelsCount; j++)
			{
				struct DEV_CALIBRATION cal = {0, 1.0};
				device->calibration[i].push_back(cal);
			}
		}
	}
	ConnectDevice();
}

void TfrmMain::ConnectDevice(void)
{
	frmWait->Show(_("Connecting to input device..."));

	if (device->Connect())
	{
		frmWait->Close();
		// update only GUI state if required
		frmControl->UpdateState(false);
		ShowMessage(_("Failed connecting to device."));
	}
	else
	{
		for (unsigned int i=0; i<device->capabilities->iChannelCount; i++)
		{
			device->EnableChannel(i, device->settings.pChannelEnable[i]);
			device->SetSensitivity(i, device->settings.pSensitivity[i]);
			float fDivVal = (float)pow(2, device->capabilities->iBitsPerSample) / (float)10 *
				device->capabilities->fSensitivityLevels[device->settings.pSensitivity[i]] *
				device->calibration[i][device->settings.pSensitivity[i]].gain;
			display->SetRange(i, fDivVal * 5.0);
			for (unsigned int j=0; j<device->capabilities->iCouplingTypeCount; j++)
			{
				if (device->capabilities->iCouplingType[j] == device->settings.eCouplingType[i])
				{
					device->SetCouplingType(i, j);
					break;
				}
			}
		}
		frmControl->UpdateState(true);
//		ToolBarMode->Enabled = true;
		frmWait->Close();
	}
	current = device->upcoming;
	bDataDivXUpdate = true;
}

void TfrmMain::DisconnectDevice(void)
{
	if (device)
	{
		device->SaveSettings(&device->settings);
		device->Disconnect();
	}
}

std::auto_ptr<Graphics::TBitmap> TfrmMain::CreatePlotBitmap(void)
{
	bool bStatState = frmMeasurements->Visible;
	if (bStatState)
	{
        // hide statistics window
		frmMeasurements->Visible = false;
		display->Update(Display::UPDATE_ALL);
	}
	frmMeasurements->Update();
	std::auto_ptr<Graphics::TBitmap> bitmap = display->CreateBitmap();
	if (bStatState)
	{
		frmMeasurements->Visible = bStatState;
		this->SetFocus();
	}
	return bitmap;
}

void __fastcall TfrmMain::ActionCopyPlotClipbrdExecute(TObject *Sender)
{
	// make sure display is repainted
	display->Update(Display::UPDATE_ALL);
	std::auto_ptr<Graphics::TBitmap> bitmap = CreatePlotBitmap();
	TClipboard *pCB = Clipboard();
	pCB->Assign(&(*bitmap));
	(void)pCB;	// avoid unused variable warning
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miMeasurementsClick(TObject *Sender)
{
	frmMeasurements->Show();
	ScopedLock<Mutex> lock(mutexDeviceData);

	unsigned int iExpectedFrameSize = device->capabilities->iBufferSizes[frmControl->cbBufferSize->ItemIndex];
	if (current.getEnabledChannel().dataOsc.size() == iExpectedFrameSize)
	{
		frmMeasurements->Process(current, CurrentChannel());
	}
}
//---------------------------------------------------------------------------

void TfrmMain::FirstRunDialog(void)
{
	AnsiString msg = _("No input device dll is selected.\n"
		"It seems like application is running for ther first time\n"
		"or configuration has been removed.\n"
		"Selecting device library is REQUIRED.");
	AnsiString title = _("No input device plugin selected");
	MessageBox(this->Handle, msg.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
	    ActionShowCommonSettingsExecute(NULL);
}

void __fastcall TfrmMain::ActionShowCommonSettingsExecute(TObject *Sender)
{
	AnsiString asPrevDevice = appSettings.frmMain.asInputDeviceDllName;
	std::list<HotKeyConf> hkConf = appSettings.hotKeyConf;	
	frmSettings->appSettings = &appSettings;
	frmSettings->ShowModal();
	if (appSettings.frmMain.bAlwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	if (appSettings.frmMain.asInputDeviceDllName != asPrevDevice)
		LoadDevice(asDevDllDir + "\\" + appSettings.frmMain.asInputDeviceDllName);

	if (hkConf != appSettings.hotKeyConf)
	{
		RegisterGlobalHotKeys();
	}		

	Timer->Interval = 1000/appSettings.Graph.iDisplayRefreshFreq;
	
	display->SetPlotLineWidth(appSettings.Graph.fPlotLineWidth);
	display->SetChannelColors(appSettings.GraphColors.traces);
	frmControl->tabOscControlTop->Invalidate();		
	frmPlotToolbar->SetTransparecy(appSettings.Graph.ToolboxTransparency);
	FFT::SetWindowType(appSettings.FFT.asWindowName);
	CLog::Instance()->LogToFile(appSettings.Logging.bLogToFile);
	bPaintUpdate = true;

	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".json" );	
	appSettings.Write(asConfigFile);	
}
//---------------------------------------------------------------------------

void TfrmMain::SetMode(E_MODE mode_)
{
	mode = mode_;
	bDataUpdate = true;
	if (mode == MODE_RECORDER)
	{
		device->Run(false);
	}
	display->SetMode(mode);
	if (frmControl)
		frmControl->SetMode(mode);

	current.SetMode(mode);
	if (device)
		device->upcoming.SetMode(mode);
}

void __fastcall TfrmMain::ActionExportPlotBitmapExecute(TObject *Sender)
{
	SaveDialog->DefaultExt = "png";
	SaveDialog->Filter = _("PNG (*.png)|*.png|Bitmap (*.bmp)|*.bmp");
	SaveDialog->FilterIndex = 1;
	if (DirectoryExists(appSettings.frmMain.asPlotPictureExportDir))
		SaveDialog->InitialDir = appSettings.frmMain.asPlotPictureExportDir;
	SaveDialog->FileName = SaveDialog->InitialDir +
		((SaveDialog->InitialDir!="")?("\\"):("")) + FormatDateTime("yyyymmdd_hhnnss_zzz", Now());
	if (SaveDialog->Execute())
	{
		appSettings.frmMain.asPlotPictureExportDir = ExtractFileDir(SaveDialog->FileName);
		// refresh display (save dialog may be still visible on low spec PC)
		display->Update(Display::UPDATE_ALL);
		std::auto_ptr<Graphics::TBitmap> bitmap = CreatePlotBitmap();
		int rc = WriteImage(SaveDialog->FileName, bitmap.get(), static_cast<ImageExportFormat>(SaveDialog->FilterIndex - 1));
		if (rc != 0)
		{
			AnsiString msg = _("Failed saving image file");
			MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionQuickPlotImageSaveExecute(TObject *Sender)
{
	if ((appSettings.frmMain.asPlotPictureExportDir == "") || (DirectoryExists(appSettings.frmMain.asPlotPictureExportDir) == false))
	{
		ActionExportPlotBitmapExecute(Sender);
	}
	else
	{
		AnsiString fileName = appSettings.frmMain.asPlotPictureExportDir + "\\" + FormatDateTime("yyyymmdd_hhnnss_zzz", Now()) + ".png";
		display->Update(Display::UPDATE_ALL);
		std::auto_ptr<Graphics::TBitmap> bitmap = CreatePlotBitmap();
		int rc = WriteImage(fileName, bitmap.get(), IMAGE_EXPORT_FORMAT_PNG);
		if (rc != 0)
		{
			AnsiString msg = _("Failed saving image file");
			MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionCalibrateSensitivityExecute(TObject *Sender)
{
	if (!frmCalibrateSensitivity->Visible)
		frmCalibrateSensitivity->Show();
	frmCalibrateSensitivity->BringToFront();
}
//---------------------------------------------------------------------------

int TfrmMain::CallbackCalculateCalibrationFn(float fVoltageHigh, int* const offset, float* const gain)
{
	float fZero, fCal, dummy;
	int status = display->GetCursorPhysical(Display::CURSOR2, dummy, fZero);
	status |= display->GetCursorPhysical(Display::CURSOR1, dummy, fCal);
	if (status)
		return status;
	// calculate offset
	int bitsZero =
		fZero/(device->capabilities->fSensitivityLevels[frmControl->trbarSensitivity->Position]*current.getChannel(CurrentChannel()).calibration.gain) +
		current.getChannel(CurrentChannel()).calibration.offset;
	*offset = 0.5 + bitsZero;

	// calculate gain
	int bitsCal = fCal/(device->capabilities->fSensitivityLevels[frmControl->trbarSensitivity->Position]*current.getChannel(CurrentChannel()).calibration.gain);
	bitsCal -= *offset;
	*gain = fVoltageHigh /
		( bitsCal * device->capabilities->fSensitivityLevels[frmControl->trbarSensitivity->Position] );
	return 0;
}

int TfrmMain::CallbackStoreCalibrationFn(const int offset, const float gain)
{
	int channel = frmControl->tabOscControlTop->TabIndex;
	int range = frmControl->trbarSensitivity->Position;
	device->calibration[channel][range].offset = offset;
	device->calibration[channel][range].gain = gain;
	device->StoreCalibration(channel, range, offset, gain);
	ApplyCalibration();
	return 0;
}

void TfrmMain::CallbackPlotToolbarClickFn(int tag)
{
	// map tag to button
	TToolButton *Button;
	switch (tag)
	{
	case 1:
		Button = btnMouseCursor;
		break;
	case 2:
		Button = btnMouseMove;
		break;
	case 3:
		Button = btnMouseZoomXY;
		break;
	case 4:
		Button = btnMouseZoomX;
		break;
	case 5:
		Button = btnMouseZoomY;
		break;
	case 6:
		Button = btnZoomClear;
		break;
	default:
		return;
	}
	Button->Action->Execute();
	if (Button->Grouped)
		Button->Down = true;
}

void TfrmMain::ApplyCalibration(void)
{
	frmControl->UpdateState(false);
}

void __fastcall TfrmMain::ActionExportDataExecute(TObject *Sender)
{
	Data *data = display->GetData();
	if (!data)
	{
    	return;
	}
	SaveDialog->DefaultExt = "json";
	SaveDialog->Filter = _("JSON files (*.json)|*.json|CSV files (*.csv)|*.csv");
	SaveDialog->FilterIndex = 1;
	if (DirectoryExists(appSettings.frmMain.asPlotDataExportDir))
		SaveDialog->InitialDir = appSettings.frmMain.asPlotDataExportDir;
	SaveDialog->FileName = SaveDialog->InitialDir +
		((SaveDialog->InitialDir!="")?("\\"):("")) + FormatDateTime("yyyymmdd_hhnnss", Now());
	if (SaveDialog->Execute())
	{
		enum Recorder::E_FMT format;
		if (SaveDialog->FilterIndex == 1)
		{
			format = Recorder::FMT_JSON;
			AddMruJsonItem(SaveDialog->FileName);
		}
		else
			format = Recorder::FMT_CSV;
		int status = Recorder::Store(SaveDialog->FileName.c_str(), format, *data);
		appSettings.frmMain.asPlotDataExportDir = ExtractFileDir(SaveDialog->FileName);

		AnsiString msg = _("Failed saving file");
		if (status)
			MessageBox(this->Handle, msg.c_str(),
				this->Caption.c_str(), MB_ICONEXCLAMATION);
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::FormShow(TObject *Sender)
{
	static bool once = false;
	if (!once)
	{
		once = true;
		// frmLog is already created - set callback to preview log entries
		CLog::Instance()->callbackLog = frmLog->OnLog;
		frmLog->callbackClose = CallbackCloseLogFn;

		frmControl->Parent = this->pnlRight;
		frmControl->Visible = true;
		frmControl->BringToFront();
		frmControl->tabOscControlTop->OnChange = tabOscControlTopChange;

		frmCalibrateSensitivity->callbackCalculate = CallbackCalculateCalibrationFn;
		frmCalibrateSensitivity->callbackStore = CallbackStoreCalibrationFn;

		UpdateDisplayVisibility();
        this->ActionViewPlotGridX->Checked = appSettings.GraphVisibility.bGridX;
       	this->ActionViewPlotGridY->Checked = appSettings.GraphVisibility.bGridY;
		this->ActionViewPlotGridLabelsX->Checked = appSettings.GraphVisibility.bGridLabelsX;
		this->ActionViewPlotGridLabelsY->Checked = appSettings.GraphVisibility.bGridLabelsY;
		this->ActionViewPlotCursors->Checked = appSettings.GraphVisibility.bCursor;
		this->ActionViewPlotGndMarkers->Checked = appSettings.GraphVisibility.bGndMarker;
		this->ActionViewPlotTriggerPosMarker->Checked = appSettings.GraphVisibility.bTriggerPosMarker;
		this->ActionViewPlotTriggerValMarker->Checked = appSettings.GraphVisibility.bTriggerValMarker;
		this->ActionViewPlotRangeValues->Checked = appSettings.GraphVisibility.bRangeValues;
		this->ActionViewPlotCursorValues->Checked = appSettings.GraphVisibility.bCursorValues;

		frmPlotToolbar->SetTransparecy(appSettings.Graph.ToolboxTransparency);

		// load device dlls
		asDevDllDir = ExtractFileDir(Application->ExeName) + "\\device";
		DeviceInterface::EnumerateDlls(asDevDllDir);

		Timer->Interval = 1000/appSettings.Graph.iDisplayRefreshFreq;
		display->ChangeSize(pnlCenter->Width, pnlCenter->Height);

		if (appSettings.frmMain.asInputDeviceDllName != "")
		{
			AnsiString dllname = asDevDllDir + "\\" + appSettings.frmMain.asInputDeviceDllName;
			for (unsigned int i=0; i<DeviceInterface::dlls.size(); i++)
			{
				if (DeviceInterface::dlls[i].name == dllname)
				{
					LoadDevice(dllname);
					break;
				}
			}
		}

		if (!device)
		{
			FirstRunDialog();
		}
		if (!device)
		{
			AnsiString msg = _("No input device dll is selected.\n"
				"Selecting input plugin is REQUIRED.\n"
				"Note: device plugins should be placed in \\device subdirectory of application.");
			AnsiString title = _("Execution aborted");
			MessageBox(this->Handle, msg.c_str(), title.c_str(), MB_ICONSTOP);
			Close();
			return;
		}
		SetMode(MODE_OSCILLOSCOPE);
		frmPlotToolbar->callbackClick = CallbackPlotToolbarClickFn;

		frmLog->SetLogLinesLimit(appSettings.Logging.iMaxUiLogLines);

		frmMeasurements->Left = appSettings.Measurements.iPosX;
		frmMeasurements->Top = appSettings.Measurements.iPosY;
		if (appSettings.Measurements.bVisible)
			frmMeasurements->Show();

		RegisterGlobalHotKeys();					
	}	
}
//---------------------------------------------------------------------------

/** \brief Set diaplay Y axis zoom & offset to fit trace into
*/
void __fastcall TfrmMain::ActionAutoFitYExecute(TObject *Sender)
{
	int channel_id = CurrentChannel();
	std::vector<float> *samples;
	Data::Channel& channel = current.getChannel(channel_id);
	if (mode == MODE_OSCILLOSCOPE)
		samples = &channel.dataOsc;
	else if (mode == MODE_SPECTRUM)
		samples = &channel.dataFFT;
	else if (mode == MODE_RECORDER)
		samples = &channel.dataRec;
	else
	{
		ShowMessage("unimplemented");
		return;
	}
	if (!samples->size())
		return;
	float fMin = std::numeric_limits<float>::max();
	float fMax = (-1.0) * fMin;

	std::vector<float>::const_iterator it;
	for (it = samples->begin(); it != samples->end(); ++it)
	{
		float val = *it;
		if (val < fMin)
			fMin = val;
		if (val > fMax)
			fMax = val;
	}

	if (fMin == fMax)
		return;

	int range = frmControl->trbarSensitivity->Position;
	float fRange = (float)pow(2, device->capabilities->iBitsPerSample) *
		device->capabilities->fSensitivityLevels[range] *
		device->calibration[channel_id][range].gain;

	float fZoomX, fZoomY;
	display->GetZoom(fZoomX, fZoomY);
	display->SetZoom(Display::OFFSET_LEAVE, Display::OFFSET_LEAVE,
		fZoomX, fRange/(fMax-fMin));
	UpdateZoomCfg();
	display->SetOffsetY((fMax + fMin)/2.0);
	bPaintUpdate = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionAutoFitXExecute(TObject *Sender)
{
	std::vector<float> *samples;
	Data* data = display->GetData();
	if (mode == MODE_OSCILLOSCOPE)
		samples = &data->getEnabledChannel().dataOsc;
	else if (mode == MODE_SPECTRUM)
		samples = &data->getEnabledChannel().dataFFT;
	else if (mode == MODE_RECORDER)
		samples = &data->getEnabledChannel().dataRec;
	else
	{
		ShowMessage("unimplemented");
		return;
	}
	if (!samples->size())
		return;

	float fZoomX, fZoomY;
	display->GetZoom(fZoomX, fZoomY);
	fZoomX = (float)pnlCenter->Width / samples->size();

	display->SetZoom(Display::OFFSET_CLEAR, Display::OFFSET_LEAVE, fZoomX, fZoomY);
	bPaintUpdate = true;
	UpdateZoomCfg();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionLoadJsonFileExecute(TObject *Sender)
{
	OpenDialog->DefaultExt = "json";
	OpenDialog->Filter = _("JSON files (*.json)|*.json|All files (*.*)|*.*");
	OpenDialog->FilterIndex = 1;
	if (DirectoryExists(appSettings.frmMain.asPlotDataExportDir))
		OpenDialog->InitialDir = appSettings.frmMain.asPlotDataExportDir;
	if (OpenDialog->Execute())
	{
		LoadJson(OpenDialog->FileName);
	}
}
//---------------------------------------------------------------------------

void TfrmMain::LoadJson(AnsiString filename)
{
	AddMruJsonItem(filename);
	int status = frmJsonViewer->LoadFile(filename);
	if (status != 0)
	{
		AnsiString msg = "Error loading JSON file";
		MessageBox(this->Handle, msg.c_str(),
			this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	bJsonViewerMode = true;
	frmControl->pnlOscControl->Enabled = true;
	ActionExportData->Enabled = false;
	UseData(frmJsonViewer->jsonData.data);
	frmControl->pnlOscControlBottom->Visible = false;
	frmJsonViewer->Parent = frmControl->pnlOscControl;
	frmJsonViewer->Visible = true;
	frmJsonViewer->callbackClose = CallbackFileViewerClose;
	frmControl->tabOscControlTop->Height = 22;
	frmControl->tabOscControlTop->OnChange(NULL);
}

void TfrmMain::CallbackFileViewerClose(void)
{
	bJsonViewerMode = false;
	bMs3ViewerMode = false;
	UpdateConnectionState();	// restore pnlOscControl enabled/disabled state
	ActionExportData->Enabled = true;
	ActionMs3Record->Enabled = true;	
	frmControl->pnlOscControlBottom->Visible = true;
	UseData(current);

	frmControl->tabOscControlTop->Height = 97;
	frmControl->tabOscControlTop->OnChange(NULL);
}

void TfrmMain::CallbackMs3ViewerDataUpdate(Data &ms3data)
{
	for (unsigned int i=0; i<ms3data.GetChannelCount(); i++) {
		filterChain.Process(ms3data.getChannel(i).dataOsc);
	}
	if (frmMeasurements->Visible)
		frmMeasurements->Process(ms3data, CurrentChannel());
	UseData(ms3data);
}

void TfrmMain::UseData(Data &data)
{
	unsigned int channel_cnt = data.GetChannelCount();
	if (frmControl->tabOscControlTop->Tabs->Count != static_cast<int>(channel_cnt))
	{
		frmControl->tabOscControlTop->Tabs->Clear();
		frmControl->cbTriggerSource->Items->Clear();
		for (unsigned int i=0; i<channel_cnt; i++)
		{
			frmControl->tabOscControlTop->Tabs->Add(GetChannelName(i));
			frmControl->cbTriggerSource->Items->Add(GetChannelName(i));
		}
		frmControl->tabOscControlTop->TabIndex = 0;

		display->SetNumberOfChannels(channel_cnt);
		display->SetPlotLineWidth(appSettings.Graph.fPlotLineWidth);
		display->SetChannelColors(appSettings.GraphColors.traces);

		for (int i=0; i<display->GetNumberOfChannels(); i++)
		{
			display->SetScopeChannelZoom(i, appSettings.Zoom.GetY(i));
		}
		display->SetZoom(Display::OFFSET_LEAVE, Display::OFFSET_LEAVE, appSettings.Zoom.fZoomX, appSettings.Zoom.GetY(CurrentChannel()));
		for (unsigned int i=0; i<channel_cnt; i++)
		{
            display->SetScopeChannelOffsetY(i, appSettings.PlotPosition.GetY(i));
        }		
	}

	if (mode == MODE_SPECTRUM)
	{
		CalculateSpectrum();
	}
	display->SetData(&data);
	display->Update(Display::UPDATE_ALL);	
}

void __fastcall TfrmMain::ActionShowDataFilterChainExecute(TObject *Sender)
{
	frmFilterSelector->ShowModal();
	if (frmFilterSelector->ModalResult == mrOk)
	{
		filterChain.Configure(frmFilterSelector->Get());
		bDataUpdate = true;
	}
}
//---------------------------------------------------------------------------

void TfrmMain::ReportError(int status, AnsiString description)
{
	if (status == 0)
		return;
	AnsiString asMsg = description + _("\nError code: ") + status;
	LOG(E_LOG_TRACE, "%s", asMsg.c_str());
	MessageBox(this->Handle, asMsg.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
}

void __fastcall TfrmMain::ActionMs3RecordExecute(TObject *Sender)
{
	if (ActionMs3Record->Checked)
	{
		SaveDialog->DefaultExt = "ms3";
		SaveDialog->Filter = _("MS3 files (*.ms3)|*.ms3");
		SaveDialog->FilterIndex = 1;
		if (DirectoryExists(appSettings.frmMain.asPlotDataExportDir))
			SaveDialog->InitialDir = appSettings.frmMain.asPlotDataExportDir;
		SaveDialog->FileName = SaveDialog->InitialDir +
			((SaveDialog->InitialDir!="")?("\\"):("")) + FormatDateTime("yyyymmdd_hhnnss", Now());
		if (SaveDialog->Execute())
		{
			if (FileExists(SaveDialog->FileName))
			{
				AnsiString msg = (AnsiString)_("File already exists, file will be overwritten.") +
					"\r\n" + _("Are you sure you want to continue?");
				if (MessageBox(this->Handle, msg.c_str(),
					this->Caption.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
				{
					ActionMs3Record->Checked = false;
					return;
				}
			}
			AddMruMs3Item(SaveDialog->FileName);			
			appSettings.frmMain.asPlotDataExportDir = ExtractFileDir(SaveDialog->FileName);
			int status = Ms3Recorder::Instance().SetMode(Ms3Recorder::MODE_WRITE, SaveDialog->FileName);
			ReportError(status, _("Failed to open MS3 file for writing"));
			if (status)
				ActionMs3Record->Checked = false;
			else
				LOG(E_LOG_TRACE, "Recording data to %s file started", SaveDialog->FileName.c_str());
		}
		else
		{
			ActionMs3Record->Checked = false;
		}
	}
	else
	{
		int status = Ms3Recorder::Instance().SetMode(Ms3Recorder::MODE_IDLE);
		ReportError(status, _("Failed to close MS3 recorder/player."));
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionMs3LoadExecute(TObject *Sender)
{
	if (ActionMs3Record->Checked)
		ActionMs3Record->Execute();

	OpenDialog->DefaultExt = "ms3";
	OpenDialog->Filter = _("MS3 files (*.ms3)|*.ms3|All files (*.*)|*.*");
	OpenDialog->FilterIndex = 1;
	if (DirectoryExists(appSettings.frmMain.asPlotDataExportDir))
		OpenDialog->InitialDir = appSettings.frmMain.asPlotDataExportDir;
	if (OpenDialog->Execute())
	{
		LoadMs3(OpenDialog->FileName);
	}
}
//---------------------------------------------------------------------------

void TfrmMain::LoadMs3(AnsiString filename)
{
	AddMruMs3Item(filename);
	int status = frmMs3Viewer->LoadFile(filename, CallbackMs3ViewerDataUpdate);
	if (status != 0)
	{
		AnsiString msg = _("Error loading MS3 file");
		MessageBox(this->Handle, msg.c_str(),
			this->Caption.c_str(), MB_ICONEXCLAMATION);
		bMs3ViewerMode = false;
		ActionMs3Record->Enabled = true;
		return;
	}
	bMs3ViewerMode = true;
	frmControl->pnlOscControl->Enabled = true;
	ActionMs3Record->Enabled = false;

	frmControl->pnlOscControlBottom->Visible = false;
	frmMs3Viewer->Parent = frmControl->pnlOscControl;
	frmMs3Viewer->Visible = true;
	frmControl->tabOscControlTop->Height = 22;
	frmControl->tabOscControlTop->OnChange(NULL);
	frmMs3Viewer->callbackClose = CallbackFileViewerClose;
}

void __fastcall TfrmMain::ActionMs3FileRepairExecute(TObject *Sender)
{
	OpenDialog->DefaultExt = "ms3";
	OpenDialog->Filter = _("MS3 files (*.ms3)|*.ms3|All files (*.*)|*.*");
	OpenDialog->FilterIndex = 1;
	if (DirectoryExists(appSettings.frmMain.asPlotDataExportDir))
		OpenDialog->InitialDir = appSettings.frmMain.asPlotDataExportDir;
	if (OpenDialog->Execute())
	{
		enum TfrmMs3FileRepair::E_STATUS status;
		frmMs3FileRepair->Execute(OpenDialog->FileName, status);
		MessageBox(this->Handle, frmMs3FileRepair->GetStatusDescription(status).c_str(),
			this->Caption.c_str(), MB_ICONINFORMATION);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::edIntervalExit(TObject *Sender)
{
	float interval = StrToFloatDef(frmControl->edInterval->Text, -1.0);
	if (interval < Settings::fRecorderMinInterval || interval > Settings::fRecorderMaxInterval)
	{
		AnsiString msg = _("Recorder interval outside allowed range!");
		MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(),
			MB_ICONEXCLAMATION);
	}
	else
	{
		appSettings.Recorder.fInterval = interval;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tabOscControlTopChange(TObject *Sender)
{
	int channel = frmControl->tabOscControlTop->TabIndex;
	display->SetCurrentChannel(channel);
	bPaintUpdate = true;
	if (bJsonViewerMode == false)
	{
        frmControl->chbChannelEnable->Checked = device->settings.pChannelEnable[channel];
		frmControl->trbarSensitivity->Position = device->settings.pSensitivity[channel];
		int range = frmControl->trbarSensitivity->Position;
		float fDivVal = (float)pow(2, device->capabilities->iBitsPerSample) / (float)10 *
			device->capabilities->fSensitivityLevels[range] *
			device->calibration[channel][range].gain;
		frmControl->lblSensitivityVal->Caption =
			ValueStringFormatter::VoltageToStr(fDivVal, 1).c_str();
		frmControl->lblSensitivityVal->Caption = frmControl->lblSensitivityVal->Caption + "/div";
		TNotifyEvent callback;
		callback = frmControl->cbCouplingType->OnChange;
		frmControl->cbCouplingType->OnChange = NULL;
		for (unsigned int i=0; i<device->capabilities->iCouplingTypeCount; i++)
		{
			if (device->capabilities->iCouplingType[i] == device->settings.eCouplingType[channel])
			{
				frmControl->cbCouplingType->ItemIndex = i;
				break;
			}
		}
		frmControl->cbCouplingType->OnChange = callback;
	}
	else
	{
    	frmJsonViewer->SetCurrentChannel(channel);
	}

	if (mode == MODE_SPECTRUM)
	{
		CalculateSpectrum();
	}

	if (frmMeasurements->Visible)
	{
		frmMeasurements->Process(current, CurrentChannel());
    }
}
//---------------------------------------------------------------------------

int TfrmMain::CurrentChannel(void)
{
	return frmControl->tabOscControlTop->TabIndex;
}


void TfrmMain::UpdateDisplayVisibility(void)
{
	display->SetVisibility(Display::GRID_X, appSettings.GraphVisibility.bGridX);
	display->SetVisibility(Display::GRID_Y, appSettings.GraphVisibility.bGridY);
	display->SetVisibility(Display::GRID_LABELS_X, appSettings.GraphVisibility.bGridLabelsX);
	display->SetVisibility(Display::GRID_LABELS_Y, appSettings.GraphVisibility.bGridLabelsY);
	display->SetVisibility(Display::CURSOR, appSettings.GraphVisibility.bCursor);
	display->SetVisibility(Display::GND_MARKER, appSettings.GraphVisibility.bGndMarker);
	display->SetVisibility(Display::RANGE_VALUES, appSettings.GraphVisibility.bRangeValues);
	display->SetVisibility(Display::CURSOR_VALUES, appSettings.GraphVisibility.bCursorValues);
    // show trigger time/value markers only in normal or auto mode (do not show if not used)
	if (device && (device->upcoming.trigger_mode == TRIGGER_MODE_SINGLE || device->upcoming.trigger_mode == TRIGGER_MODE_NORMAL || device->upcoming.trigger_mode == TRIGGER_MODE_AUTO))
	{
		if ((device->upcoming.trigger_type == TRIGGER_SLOPE_UP || device->upcoming.trigger_type == TRIGGER_SLOPE_DOWN) &&
			device->hasTriggerPositionControl())
		{
			display->SetVisibility(Display::TRIGGER_POS_MARKER, appSettings.GraphVisibility.bTriggerPosMarker);
		}
		else
		{
			display->SetVisibility(Display::TRIGGER_POS_MARKER, false);
		}
		if (device->upcoming.trigger_type == TRIGGER_SLOPE_UP || device->upcoming.trigger_type == TRIGGER_SLOPE_DOWN)
		{
			display->SetVisibility(Display::TRIGGER_VAL_MARKER, appSettings.GraphVisibility.bTriggerValMarker);
		}
		else
		{
			display->SetVisibility(Display::TRIGGER_VAL_MARKER, false);
		}
	}
	else
	{
		display->SetVisibility(Display::TRIGGER_POS_MARKER, false);
		display->SetVisibility(Display::TRIGGER_VAL_MARKER, false);
	}
	bPaintUpdate = true;	
}

void __fastcall TfrmMain::ActionViewPlotExecute(TObject *Sender)
{
	// plot elements visibility changed
	appSettings.GraphVisibility.bGridX = this->ActionViewPlotGridX->Checked;
	appSettings.GraphVisibility.bGridY = this->ActionViewPlotGridY->Checked;
	appSettings.GraphVisibility.bGridLabelsX = this->ActionViewPlotGridLabelsX->Checked;
	appSettings.GraphVisibility.bGridLabelsY = this->ActionViewPlotGridLabelsY->Checked;
	appSettings.GraphVisibility.bCursor = this->ActionViewPlotCursors->Checked;
	appSettings.GraphVisibility.bGndMarker = this->ActionViewPlotGndMarkers->Checked;
	appSettings.GraphVisibility.bTriggerPosMarker = this->ActionViewPlotTriggerPosMarker->Checked;	
	appSettings.GraphVisibility.bTriggerValMarker = this->ActionViewPlotTriggerValMarker->Checked;	
	appSettings.GraphVisibility.bRangeValues = this->ActionViewPlotRangeValues->Checked;
	appSettings.GraphVisibility.bCursorValues = this->ActionViewPlotCursorValues->Checked;

	UpdateDisplayVisibility();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormMouseWheelDown(TObject *Sender, TShiftState Shift,
	  TPoint &MousePos, bool &Handled)
{
	TPoint P = PaintBoxCenter->ScreenToClient(Mouse->CursorPos);
	if (P.x >= 0 && P.y >= 0 && P.x < PaintBoxCenter->Width && P.y < PaintBoxCenter->Height)
	{
		Handled = true;
		float fZoomX, fZoomY;
		display->GetZoom(fZoomX, fZoomY);
		float fDivY = display->GetDivY();
		if (Shift.Contains(ssShift))
		{
			fZoomY /= fZoomChangeGrid;
		}
		else
		{
			fZoomY = fZoomY * fDivY/GetNextRoundHigherValue(fDivY);
		}
		display->SetZoom(Display::OFFSET_LEAVE, Display::OFFSET_KEEP_GND_POSITION,
			fZoomX, fZoomY);
		UpdateZoomCfg();
		bPaintUpdate = true;
		return;
	}
	Handled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormMouseWheelUp(TObject *Sender, TShiftState Shift,
	  TPoint &MousePos, bool &Handled)
{
	TPoint P = PaintBoxCenter->ScreenToClient(Mouse->CursorPos);
	if (P.x >= 0 && P.y >= 0 && P.x < PaintBoxCenter->Width && P.y < PaintBoxCenter->Height)
	{
		Handled = true;
		float fZoomX, fZoomY;
		display->GetZoom(fZoomX, fZoomY);
		float fDivY = display->GetDivY();
		if (Shift.Contains(ssShift))
		{
			fZoomY *= fZoomChangeGrid;
		}
		else
		{
			fZoomY = fZoomY * fDivY/GetNextRoundLowerValue(fDivY);
		}
		display->SetZoom(Display::OFFSET_LEAVE, Display::OFFSET_KEEP_GND_POSITION,
			fZoomX, fZoomY);
		UpdateZoomCfg();
		bPaintUpdate = true;
		return;
	}
	Handled = false;	
}
//---------------------------------------------------------------------------

void TfrmMain::obsUpdate(Observable* o, Argument * arg)
{
	Display* display = dynamic_cast<Display*>(o);
	if (display)
	{
		DisplayNotifyArgument* darg = dynamic_cast<DisplayNotifyArgument*>(arg);
		assert(darg);
		if (darg->type == DisplayNotifyArgument::SCALE)
		{
			float fZoomX, fZoomY;
			display->GetZoom(fZoomX, fZoomY);
			lblZoomX->Caption = (AnsiString)"X: " + (ValueStringFormatter::ValToStr(fZoomX, 3) + "x").c_str();
			lblZoomY->Caption = (AnsiString)"Y: " + (ValueStringFormatter::ValToStr(fZoomY, 3) + "x").c_str();

			if (mode == MODE_SPECTRUM)
				lblXscale->Caption = ValueStringFormatter::FreqToStr(display->GetDivX(), 2).c_str();
			else
				lblXscale->Caption = ValueStringFormatter::TimeToStr(display->GetDivX(), 2).c_str();
			lblYscale->Caption = ValueStringFormatter::VoltageToStr(display->GetDivY(), 2).c_str();
			lblXscale->Caption = lblXscale->Caption + "/div";
			lblYscale->Caption = lblYscale->Caption + "/div";
		}
	}
}

void __fastcall TfrmMain::miStatisticsClick(TObject *Sender)
{
	static int id = 1;
	TfrmStatistics *stat = new TfrmStatistics((TComponent*)NULL, id++, &CallbackGetCursorCoordinates);
	stat->Show();
	stat->BringToFront();
}
//---------------------------------------------------------------------------

void TfrmMain::CallbackGetCursorCoordinates(enum E_MODE &mode_,
	float &X1, float &Y1, float &X2, float &Y2)
{
	mode_ = mode;
	display->GetCursorPhysical(Display::CURSOR1, X1, Y1);
	display->GetCursorPhysical(Display::CURSOR2, X2, Y2);
}

void __fastcall TfrmMain::miDeviceConnectClick(TObject *Sender)
{
	miDeviceConnect->Enabled = false;
	ConnectDevice();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miDeviceDisconnectClick(TObject *Sender)
{
	miDeviceDisconnect->Enabled = false;
	DisconnectDevice();	
}
//---------------------------------------------------------------------------


void TfrmMain::UpdateMruMenu(void)
{
	TMenuItem *item;

	miLoadRecentJson->Clear();
	miLoadRecentJson->AutoHotkeys = maManual;
	if (appSettings.MRUjson.empty())
	{
		item = new TMenuItem(Menu);
		item->AutoHotkeys = maManual;
		item->Caption = "- empty -";
		item->Enabled = false;
		miLoadRecentJson->Insert(0, item);
	}
	for (unsigned int i=0; i<appSettings.MRUjson.size(); i++)
	{
		item = new TMenuItem(Menu);
		item->AutoHotkeys = maManual;
		item->Caption = appSettings.MRUjson[i];
		item->OnClick = MruJsonClick;
		miLoadRecentJson->Insert(0, item);
	}

	miLoadRecentMs3->Clear();
	miLoadRecentMs3->AutoHotkeys = maManual;
	if (appSettings.MRUms3.empty())
	{
		item = new TMenuItem(Menu);
		item->AutoHotkeys = maManual;
		item->Caption = "- empty -";
		item->Enabled = false;
		miLoadRecentMs3->Insert(0, item);
	}
	for (unsigned int i=0; i<appSettings.MRUms3.size(); i++)
	{
		item = new TMenuItem(Menu);
		item->AutoHotkeys = maManual;
		item->Caption = appSettings.MRUms3[i];
		item->OnClick = MruMs3Click;
		miLoadRecentMs3->Insert(0, item);
	}
}

void TfrmMain::AddMruJsonItem(AnsiString filename)
{
	appSettings.AddMruJson(filename);
}

void __fastcall TfrmMain::MruJsonClick(TObject *Sender)
{
	TMenuItem *item = dynamic_cast<TMenuItem*>(Sender);
	LoadJson(item->Caption);
}

void TfrmMain::AddMruMs3Item(AnsiString filename)
{
	appSettings.AddMruMs3(filename);
}

void __fastcall TfrmMain::MruMs3Click(TObject *Sender)
{
	TMenuItem *item = dynamic_cast<TMenuItem*>(Sender);
	LoadMs3(item->Caption);
}

void __fastcall TfrmMain::File1Click(TObject *Sender)
{
	UpdateMruMenu();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::WMXButtonDown(TWMMouse& msg)
{

}

void TfrmMain::UpdateZoomCfg(void)
{
	if (display == NULL)
		return;
	if (mode == MODE_OSCILLOSCOPE)
	{
		float fZoomX, fZoomY;
		display->GetZoom(fZoomX, fZoomY);
		int channel_id = CurrentChannel();
		appSettings.Zoom.fZoomX = fZoomX;
		appSettings.Zoom.SetY(channel_id, fZoomY);		
	}
}

void TfrmMain::RegisterGlobalHotKeys(void)
{
	int rc = hotKeys.RegisterGlobal(appSettings.hotKeyConf, Handle);
	if (rc > 0)
	{
		AnsiString msg;
		msg.sprintf("Failed to register %d global hotkey(s), see log for details", rc);
		MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONINFORMATION);
	}
}

void __fastcall TfrmMain::WMHotKey(TWMHotKey &Message)
{
	TForm::Dispatch(&Message);
	const HotKeyConf* cfg = hotKeys.FindGlobal(Message.HotKey);
	if (IsWin7OrLater() == false)
	{
		// MOD_NOREPEAT not supported, use own antirepeat
		if (cfg == lastHotkey)
		{
			// anti-repeat
			return;
		}
		lastHotkey = cfg;
	}
	if (cfg)
	{
		ExecAction(cfg->action);
		if (IsWin7OrLater() == false)
		{
			tmrAntirepeat->Enabled = false;
			tmrAntirepeat->Enabled = true;
		}
	}
}

void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
#if 0
	LOG("Key = %d, CTRL = %d, SHIFT = %d, ALT = %d\n",
		Key,
		Shift.Contains(ssCtrl), Shift.Contains(ssShift), Shift.Contains(ssAlt));
#endif
	const HotKeyConf* cfg = hotKeys.Find(appSettings.hotKeyConf, Key, Shift, false);
	if (cfg == lastHotkey)
	{
		// anti-repeat
		return;
	}
	lastHotkey = cfg;
	if (cfg)
	{
		ExecAction(cfg->action);
		tmrAntirepeat->Enabled = false;
		tmrAntirepeat->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrAntirepeatTimer(TObject *Sender)
{
	tmrAntirepeat->Enabled = false;
	lastHotkey = NULL;
}
//---------------------------------------------------------------------------

void TfrmMain::ExecAction(const struct Action& action)
{
	switch (action.type)
	{
	case Action::TYPE_NONE:
		break;
	case Action::TYPE_MANUAL_TRIGGER:
		frmControl->ManualTrigger();
		break;
	case Action::TYPE_SAVE_GRAPH_IMAGE:
		ActionQuickPlotImageSave->Execute();	
		break;
	case Action::TYPE_TOOL_CURSOR:
		ActionChangeMouseModeCursor->Execute();
		break;
	case Action::TYPE_TOOL_MOVE:
		ActionChangeMouseModeOffset->Execute();
		break;
	case Action::TYPE_TOOL_ZOOM:
		ActionChangeMouseModeZoom->Execute();
		break;
	case Action::TYPE_TOOL_ZOOM_X:
		ActionChangeMouseModeZoomX->Execute();
		break;
	case Action::TYPE_TOOL_ZOOM_Y:
		ActionChangeMouseModeZoomY->Execute();
		break;
	case Action::TYPE_TOOL_ZOOM_CLEAR:
		ActionZoomClear->Execute();
		break;
	case Action::TYPE_TOOL_ZOOM_FIT_X:
		ActionAutoFitX->Execute();
		break;
	case Action::TYPE_TOOL_ZOOM_FIT_Y:
		ActionAutoFitY->Execute();
		break;

	default:
		break;
	}
}


