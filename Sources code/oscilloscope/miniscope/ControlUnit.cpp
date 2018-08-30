//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ControlUnit.h"
#include "Settings.h"
#include "MyTrackBar.h"
#include "device\MiniscopeDeviceInterface.h"
#include "display\ValueStringFormatter.h"
#include "display\Display.h"
#include "Rounding.h"
#include "CalibrateSensitivityUnit.h"
#include "Log.h"
#include <Mutex.h>
#include <ScopedLock.h>

#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmControl *frmControl = NULL;
extern Settings appSettings;
//---------------------------------------------------------------------------
__fastcall TfrmControl::TfrmControl(TComponent* Owner)
	: TfrmCommon(Owner), mutexDeviceData(NULL), current(NULL),
	device(NULL), display(NULL), bDataUpdate(NULL), bPaintUpdate(NULL),
	OnUpdateTriggerMode(NULL)
{
	tabOscControlTop->Tabs->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmControl::FormCreate(TObject *Sender)
{
	edInterval->Text = AnsiString::FloatToStrF(appSettings.Recorder.fInterval, AnsiString::sffGeneral, 1, 3);
	((TMyTrackBar*)trbarSensitivity)->OnMouseUp = TrackBarMouseUp;
	((TMyTrackBar*)trbarSampling)->OnMouseUp = TrackBarMouseUp;
	((TMyTrackBar*)trbarTriggerLevel)->OnMouseUp = TrackBarMouseUp;
	((TMyTrackBar*)trbarTriggerPosition)->OnMouseUp = TrackBarMouseUp;
}
//---------------------------------------------------------------------------

void TfrmControl::Configure(Mutex *mutexDeviceData, Data *current,
		DeviceInterface *device, Display *display,
		bool *bDataUpdate, bool *bPaintUpdate)
{
	this->mutexDeviceData = mutexDeviceData;
	this->current = current;
	this->device = device;
	this->display = display;
	this->bDataUpdate = bDataUpdate;
	this->bPaintUpdate = bPaintUpdate;
}                         

void TfrmControl::SetCapabilities(const struct S_SCOPE_CAPABILITIES* const capabilities)
{
	if (capabilities->iSensitivityLevelsCount > 1) {
		lblSensitivity->Visible = true;
		lblSensitivityVal->Visible = true;
		trbarSensitivity->Visible = true;
	} else {
		lblSensitivity->Visible = false;
		lblSensitivityVal->Visible = false;
		trbarSensitivity->Visible = false;
	}
	trbarSensitivity->Max = capabilities->iSensitivityLevelsCount - 1;
	trbarSensitivity->Position = trbarSensitivity->Max;

	if (device->hasChannelEnable())
	{
		chbChannelEnable->Visible = true;
	}
	else
	{
    	chbChannelEnable->Visible = false;
	}

	if (capabilities->iSamplingPeriodCount > 1) {
		lblSampling->Visible = true;
		lblSamplingVal->Visible = true;
		trbarSampling->Visible = true;
	} else {
		lblSampling->Visible = false;
		lblSamplingVal->Visible = false;
		trbarSampling->Visible = false;
	}
	trbarSampling->Max = capabilities->iSamplingPeriodCount - 1;
	trbarSampling->Position = trbarSampling->Max;

	//trbarTriggerLevel->Min = pow(2, capabilities->iBitsPerSample)/(-2);
	//trbarTriggerLevel->Max = pow(2, capabilities->iBitsPerSample)/2 - 1;
	trbarTriggerLevel->Min = -100;
	trbarTriggerLevel->Max = 100;
	trbarTriggerLevel->Position = 0;

	cbTriggerMode->Clear();
	for (unsigned int i=0; i<capabilities->iTriggerModeCount; i++) {
		AnsiString name = DeviceInterface::GetTriggerModeName(capabilities->iTriggerModes[i]);
		cbTriggerMode->Items->Add(name);
	}
	if (capabilities->iTriggerModeCount > 0) {
		cbTriggerMode->ItemIndex = 0;
	}

	cbTriggerType->Clear();
	for (unsigned int i=0; i<capabilities->iTriggerTypeCount; i++) {
		AnsiString name = DeviceInterface::GetTriggerTypeName(capabilities->iTriggerTypes[i]);
		switch (capabilities->iTriggerTypes[i]) {
		case TRIGGER_SLOPE_UP:
			cbTriggerType->Items->Add(name);
			break;
		case TRIGGER_SLOPE_DOWN:
			cbTriggerType->Items->Add(name);
			break;
		default:
			break;
		}
	}
	if (capabilities->iTriggerTypeCount > 0) {
		cbTriggerType->ItemIndex = 0;
	}

	cbCouplingType->Clear();
	for (unsigned int i=0; i<capabilities->iCouplingTypeCount; i++) {
		AnsiString name = DeviceInterface::GetCouplingTypeName(capabilities->iCouplingType[i]);
		switch (capabilities->iCouplingType[i]) {
		case COUPLING_GND:
			cbCouplingType->Items->Add(name);
			break;
		case COUPLING_DC:
			cbCouplingType->Items->Add(name);
			break;
		case COUPLING_AC:
			cbCouplingType->Items->Add(name);
			break;
		default:
			break;
		}
	}
	if (capabilities->iCouplingTypeCount > 0) {
		cbCouplingType->ItemIndex = 0;
	}

	cbBufferSize->Clear();
	for (unsigned int i=0; i<capabilities->iBufferSizeCount; i++) {
		cbBufferSize->Items->Add(capabilities->iBufferSizes[i]);
	}
	if (capabilities->iBufferSizeCount > 0) {
		cbBufferSize->ItemIndex = 0;
	}
}

void TfrmControl::SetMode(const enum E_MODE m)
{
	if (m != MODE_RECORDER)
	{
		lblTriggerType->Visible = true;
		cbTriggerType->Visible = true;

		btnRecorderStart->Visible = false;
		btnRecorderStop->Visible = false;
		btnRecorderClear->Visible = false;
		lblInterval->Visible = false;
		edInterval->Visible = false;
		lblSeconds->Visible = false;

		btnRecorderStart->Enabled = true;
		btnRecorderStop->Enabled = false;

		lblTriggerMode->Visible = true;
		cbTriggerMode->Visible = true;		
		UpdateTriggerMode(false);

		btnStart->Visible = true;
		btnStop->Visible = true;

		tmrRecorder->Enabled = false;
	}
	else
	{
		lblTriggerType->Visible = false;
		cbTriggerType->Visible = false;
		btnManualTrigger->Visible = false;
		btnRecorderStart->Visible = true;
		btnRecorderStop->Visible = true;
		btnRecorderClear->Visible = true;
		lblInterval->Visible = true;
		edInterval->Visible = true;
		lblSeconds->Visible = true;

		lblTriggerMode->Visible = false;
		cbTriggerMode->Visible = false;
		trbarTriggerLevel->Visible = false;
		lblTriggerLevel->Visible = false;
		lblTriggerLevelVal->Visible = false;
		lblTriggerSource->Visible = false;
		cbTriggerSource->Visible = false;

		btnStart->Visible = false;
		btnStop->Visible = false;

		btnStart->Enabled = true;
		btnStop->Enabled = false;
		device->Run(false);
	}
}

void TfrmControl::UpdateState(bool bUpdateDevice)
{
	if (bUpdateDevice == false)
	{
		UpdateTriggerMode(false);
		UpdateTriggerType(false);
		UpdateSensitivity(false);
		UpdateSampling(false);
	}
	else
	{
		UpdateTriggerMode();
		UpdateTriggerType();
		UpdateBufferSize();
		UpdateSensitivity();
		UpdateCouplingType();
		UpdateSampling();
		UpdateTriggerLevel();
		UpdateTriggerPosition();
	}
}

void __fastcall TfrmControl::TrackbarKeyUp(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	UpdateTrackBar(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::TrackBarMouseUp(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y)
{
	UpdateTrackBar(Sender);
}

void TfrmControl::UpdateTrackBar(TObject *Sender)
{
	if (Sender == trbarSensitivity)
	{
		UpdateSensitivity();
	}
	else if (Sender == trbarSampling)
	{
		UpdateSampling();
	}
	else if (Sender == trbarTriggerLevel)
	{
		UpdateTriggerLevel();
	}
	else if (Sender == trbarTriggerPosition)
	{
    	UpdateTriggerPosition();
	}
}


void TfrmControl::UpdateSensitivity(bool bUpdateDevice)
{
	int channel = tabOscControlTop->TabIndex;
	int range = trbarSensitivity->Position;
	if (bUpdateDevice)
		device->SetSensitivity(channel, range);
	// update range caption, lblSensitivityVal
	float fDivVal = (float)pow(2, device->capabilities->iBitsPerSample) / (float)10 *
		device->capabilities->fSensitivityLevels[range] *
		device->calibration[channel][range].gain;
	lblSensitivityVal->Caption =
		ValueStringFormatter::VoltageToStr(fDivVal, 1).c_str();
	lblSensitivityVal->Caption = lblSensitivityVal->Caption + "/div";
	device->upcoming.getChannel(channel).fDivY = device->capabilities->fSensitivityLevels[range];
	device->upcoming.getChannel(channel).bDivYChanged = true;
	device->upcoming.getChannel(channel).calibration = device->calibration[channel][range];

	if (display)
	{
		display->SetRange(channel, fDivVal * 5.0);
		//display->SetDiv(device->capabilities->fSamplingPeriod[trbarSampling->Position]);
		*bDataUpdate = true;
	}
	frmCalibrateSensitivity->SetCurrentCalibration(
		device->calibration[channel][range].offset,
		device->calibration[channel][range].gain);
}

void TfrmControl::UpdateSampling(bool bUpdateDevice)
{
	if (bUpdateDevice)
		device->SetSampling(trbarSampling->Position);
	float fSamplVal = (float)1.0 / device->capabilities->fSamplingPeriod[trbarSampling->Position];
	lblSamplingVal->Caption =
		ValueStringFormatter::FreqToStr(fSamplVal, 1).c_str();
}

void TfrmControl::UpdateTriggerLevel(void)
{
	int val = trbarTriggerLevel->Position;
	device->SetTriggerLevel(val);
	lblTriggerLevelVal->Caption = IntToStr(val) + "%";
	display->SetTriggerValue(val);
}

void TfrmControl::UpdateTriggerPosition(void)
{
	int pos = trbarTriggerPosition->Position;
	device->SetTriggerPosition(pos);
	lblTriggerPositionVal->Caption = IntToStr(pos) + "%";
	display->SetTriggerPosition(pos);
}

void TfrmControl::UpdateTriggerMode(bool bUpdateDevice)
{
	int id = cbTriggerMode->ItemIndex;
	enum E_TRIGGER_MODE mode = device->capabilities->iTriggerModes[id];
	if (bUpdateDevice)
		device->SetTriggerMode(id);

	switch (mode)
	{
	case TRIGGER_MODE_SINGLE:
	case TRIGGER_MODE_NORMAL:
	case TRIGGER_MODE_AUTO:
		trbarTriggerLevel->Visible = true;
		lblTriggerLevel->Visible = true;
		lblTriggerLevelVal->Visible = true;
		if (device->hasTriggerPositionControl())
		{
			trbarTriggerPosition->Visible = true;
			lblTriggerPosition->Visible = true;
			lblTriggerPositionVal->Visible = true;
		}
		lblTriggerSource->Visible = true;
		cbTriggerSource->Visible = true;
		lblTriggerType->Visible = true;
		cbTriggerType->Visible = true;
		btnManualTrigger->Visible = true;
		break;
	case TRIGGER_MODE_CONTINUOUS:
		trbarTriggerLevel->Visible = false;
		lblTriggerLevel->Visible = false;
		lblTriggerLevelVal->Visible = false;
		trbarTriggerPosition->Visible = false;
		lblTriggerPosition->Visible = false;
		lblTriggerPositionVal->Visible = false;
		lblTriggerSource->Visible = false;
		cbTriggerSource->Visible = false;
		lblTriggerType->Visible = false;
		cbTriggerType->Visible = false;
		btnManualTrigger->Visible = false;
		break;
	default:
		break;
	}

	if (OnUpdateTriggerMode)
		OnUpdateTriggerMode();
}

void TfrmControl::UpdateTriggerType(bool bUpdateDevice)
{
	enum E_TRIGGER_TYPE type = device->capabilities->iTriggerTypes[cbTriggerType->ItemIndex];
	int channel = cbTriggerSource->ItemIndex;
	if (bUpdateDevice)
		device->SetTriggerType(channel, cbTriggerType->ItemIndex);
	display->SetTriggerChannel(channel);
	if ((type == TRIGGER_SLOPE_UP || type == TRIGGER_SLOPE_DOWN) && device->hasTriggerPositionControl())
	{
		display->SetVisibility(Display::TRIGGER_POS_MARKER, appSettings.GraphVisibility.bTriggerPosMarker);
	}
	else
	{
		display->SetVisibility(Display::TRIGGER_POS_MARKER, false);
	}
	if (type == TRIGGER_SLOPE_UP || type == TRIGGER_SLOPE_DOWN)
	{
		display->SetVisibility(Display::TRIGGER_VAL_MARKER, appSettings.GraphVisibility.bTriggerValMarker);
	}
	else
	{
		display->SetVisibility(Display::TRIGGER_VAL_MARKER, false);
	}
	*bPaintUpdate = true;
}

void TfrmControl::UpdateChannelEnable(void)
{
	int channel = tabOscControlTop->TabIndex;
	device->EnableChannel(channel, chbChannelEnable->Checked);
}

void TfrmControl::UpdateCouplingType(void)
{
	int channel = tabOscControlTop->TabIndex;
	device->SetCouplingType(channel, cbCouplingType->ItemIndex);
}

void TfrmControl::UpdateBufferSize(void)
{
	if (!cbBufferSize->Items->Count)
		return;
	device->SetBufferSize(cbBufferSize->ItemIndex);
}

void __fastcall TfrmControl::cbTriggerTypeChange(TObject *Sender)
{
	UpdateTriggerType(true);	
}
//---------------------------------------------------------------------------
void __fastcall TfrmControl::cbBufferSizeChange(TObject *Sender)
{
	UpdateBufferSize();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmControl::cbCouplingTypeChange(TObject *Sender)
{
	UpdateCouplingType();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::chbChannelEnableClick(TObject *Sender)
{
	UpdateChannelEnable();
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::chbChannelEnableKeyPress(TObject *Sender,
	  char &Key)
{
	UpdateChannelEnable();
}

//---------------------------------------------------------------------------
void __fastcall TfrmControl::cbTriggerModeChange(TObject *Sender)
{
    UpdateTriggerMode(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::btnStartClick(TObject *Sender)
{
	btnStart->Enabled = false;
	btnStop->Enabled = true;
	device->Run(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::btnStopClick(TObject *Sender)
{
	btnStart->Enabled = true;
	btnStop->Enabled = false;
	device->Run(false);
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::tabOscControlTopDrawTab(TCustomTabControl *Control,
      int TabIndex, const TRect &Rect, bool Active)
{
	TCanvas *pCanvas = Control->Canvas;
	if (display)
	{
		RECT rect = static_cast<RECT>(Rect);
		rect.top = Rect.Bottom - 4;
		FillRect(pCanvas->Handle, &rect, CreateSolidBrush(display->GetChannelColor(TabIndex)));
	}
	pCanvas->Brush->Color = clBtnFace;
	RECT R = static_cast<RECT>(Rect);
	AnsiString text(tabOscControlTop->Tabs->Strings[TabIndex]);
	DrawText(pCanvas->Handle, text.c_str(), text.Length(), &R,
			 DT_CENTER | DT_TOP | DT_SINGLELINE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::btnManualTriggerClick(TObject *Sender)
{
	ManualTrigger();
}
//---------------------------------------------------------------------------


void __fastcall TfrmControl::btnRecorderStartClick(TObject *Sender)
{
	float interval = StrToFloatDef(edInterval->Text, -1.0);
	if (interval < Settings::fRecorderMinInterval || interval > Settings::fRecorderMaxInterval)
	{
		AnsiString msg = _("Recorder interval outside allowed range!");
		MessageBox(this->Handle, msg.c_str(), Application->Title.c_str(),
			MB_ICONEXCLAMATION);
		//edInterval->Text = AnsiString::FloatToStrF(appSettings.fRecorderInterval, AnsiString::sffGeneral, 1, 3);
		return;
	}
	appSettings.Recorder.fInterval = interval;
	current->fRecorderInterval = interval;
	display->SetData(current);

	tmrRecorder->Interval = interval * 1000.0;


	tmrRecorder->Enabled = true;
	btnRecorderStart->Enabled = false;
	edInterval->Enabled = false;
	btnRecorderStop->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::btnRecorderStopClick(TObject *Sender)
{
	tmrRecorder->Enabled = false;
	btnRecorderStart->Enabled = true;
	edInterval->Enabled = true;
	btnRecorderStop->Enabled = false;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::btnRecorderClearClick(TObject *Sender)
{
	ScopedLock<Mutex> lock(*mutexDeviceData);
	for (unsigned int i=0; i<current->GetChannelCount(); i++)
	{
		Data::Channel& channel = current->getChannel(i);
		channel.dataRec.clear();
	}
	*bDataUpdate = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmControl::tmrRecorderTimer(TObject *Sender)
{
	assert(mode == MODE_RECORDER);
	// execute manual trigger
	if (device)
		device->ManualTrigger();
	/** \todo check if previous sample already received */	
}
//---------------------------------------------------------------------------

void TfrmControl::ManualTrigger(void)
{
	int status = device->ManualTrigger();
	if (status)
		LOG(E_LOG_TRACE, "ManualTrigger() failed with status %d", status);
	else
	{
		ProgressBarFrameRx->Visible = true;
		btnStart->Enabled = false;
		btnStop->Enabled = true;
	}
}



