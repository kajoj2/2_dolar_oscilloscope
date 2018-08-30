//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MeasurementsUnit.h"
#include "display\ValueStringFormatter.h"
#include "device\Data.h"
#include "Utilities.h"
#include <time.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMeasurements *frmMeasurements;
//---------------------------------------------------------------------------
__fastcall TfrmMeasurements::TfrmMeasurements(TComponent* Owner)
	: TfrmCommon(Owner)
{
	memo->DoubleBuffered = true;
}
//---------------------------------------------------------------------------

void TfrmMeasurements::Process(const Data &data, int channel)
{
	if (const_cast<Data&>(data).getChannel(channel).dataOsc.empty())
		return;
	/** \todo spectrum, osc -> dataOsc, recorder -> dataRec? */
	AnsiString title = _("Ch. %s stats");
	this->Caption.sprintf(title.c_str(), GetChannelName(channel).c_str());
	measurements.Process(data, channel);

	char szTime[64] = {0};
	AnsiString asTime;
	if (strftime(szTime, sizeof(szTime), "%T", localtime(&data.timestamp.time)) > 0)
	{
		asTime.sprintf("%s.%03u", szTime, data.timestamp.millitm);
	}
	AnsiString asFreq;
	if (measurements.bFreqValid)
		asFreq = ValueStringFormatter::FreqToStr(measurements.fFreq, 3).c_str();
	else
		asFreq = "----";
	AnsiString text;
	text.sprintf("Time: %12s\r\n"
				 "Vmin: %12s\r\n"
				 "Vmax: %12s\r\n"
				 "Vpp:  %12s\r\n"
				 "Vavg: %12s\r\n"
				 "RMS:  %12s\r\n"
				 "Freq: %12s"
				,
				asTime.c_str(),
				ValueStringFormatter::VoltageToStr(measurements.fMin, 3).c_str(),

				ValueStringFormatter::VoltageToStr(measurements.fMax, 3).c_str(),
				ValueStringFormatter::VoltageToStr(measurements.fMax-measurements.fMin, 3).c_str(),
				ValueStringFormatter::VoltageToStr(measurements.fAvg, 3).c_str(),
				ValueStringFormatter::VoltageToStr(measurements.fRms, 3).c_str(),
				asFreq.c_str()
	);
	memo->Text = text;
}

