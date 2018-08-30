//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include "CalibrateSensitivityUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCalibrateSensitivity *frmCalibrateSensitivity;
//---------------------------------------------------------------------------
__fastcall TfrmCalibrateSensitivity::TfrmCalibrateSensitivity(TComponent* Owner)
	: TfrmCommon(Owner), callbackCalculate(NULL), callbackStore(NULL)
{
	edVoltage->Text = FloatToStrF(1.0, ffFixed, 6, 1);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCalibrateSensitivity::btnCalibrateClick(TObject *Sender)
{
	if (!callbackCalculate)
	{
		ShowMessage("No callbackCalculate set!");
		return;
	}
	edOffset->Text = "";
	edGain->Text = "";
	float fValH = StrToFloatDef(edVoltage->Text, 0.0);
	if (fValH == 0)
	{
		ShowMessage(_("Incorrect entered value of voltage at left cursor"));
		return;
	}
	int offset;
	float gain;
	int status = callbackCalculate(fValH, &offset, &gain);
	if (status)
	{
		ShowMessage(_("Calibration calculation failed"));
		return;
	}
	edOffset->Text = IntToStr(offset);
	edGain->Text = FloatToStrF(gain, ffFixed, 6, 4);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCalibrateSensitivity::btnStoreCalibrationClick(
	  TObject *Sender)
{
	if (!callbackStore)
	{
		ShowMessage("No callbackStore set!");
		return;
	}
	int offset;
	float gain;
	int n = sscanf(edOffset->Text.c_str(), "%d", &offset);
	gain = StrToFloatDef(edGain->Text, 0.0);
	if (n == 0)
	{
		ShowMessage(_("Incorrect offset value"));
		return;
	}
	if ( gain < 0.66 || gain > 1.5 )
	{
		AnsiString msg;
		msg.sprintf("Calculated calibration coefficient value (= %f) seems to be outside of reasonable range.\n"
			"Are you sure you want to store it?", gain);
		if (MessageBox(this->Handle, msg.c_str(),
			this->Caption.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
		{
			return;
		}
	}
	int status = callbackStore(offset, gain);
	if (status)
	{
		ShowMessage(_("Calibration storing failed"));
		return;
	}
}
//---------------------------------------------------------------------------

void TfrmCalibrateSensitivity::SetCurrentCalibration(const int offset, const float gain)
{
	edOffset->Text = IntToStr(offset);
	edGain->Text = FloatToStrF(gain, ffFixed, 6, 4);
}

