/** \brief Statistics for time periods manually marked on the plot
*/

#include <vcl.h>
#pragma hdrstop

#include "StatisticsUnit.h"
#include "ValueStringFormatter.h"
#include <assert.h>
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmStatistics *frmStatistics;
//---------------------------------------------------------------------------
__fastcall TfrmStatistics::TfrmStatistics(TComponent* Owner, int id,
	GetCursorCoordinates getCursorCoordinatesCb)
	: TfrmCommon(Owner),
	getCursorCoordinatesCb(getCursorCoordinatesCb)
{
	assert(getCursorCoordinatesCb);
	Caption = (AnsiString)_("Statistics window ") + id;
}
//---------------------------------------------------------------------------
void __fastcall TfrmStatistics::btnAddCurrentSegmentClick(TObject *Sender)
{
	enum E_MODE mode;
	struct Segment segment;
	getCursorCoordinatesCb(mode, segment.X1, segment.Y1, segment.X2, segment.Y2);
	if (mode != MODE_OSCILLOSCOPE && mode != MODE_RECORDER)
	{
		AnsiString msg = _("Oscilloscope or recorder mode required.");
		AnsiString title = Caption;
		MessageBox(this->Handle, msg.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
		return;
	}
	segments.push_back(segment);
	UpdateData();	
}
//---------------------------------------------------------------------------

void TfrmStatistics::UpdateData(void)
{
	lvSegments->Items->Count = segments.size();
	lvSegments->Invalidate();
	float sum = 0.0f, avg;
	for (unsigned int i=0; i<segments.size(); i++)
	{
		struct Segment &segment = segments[i];
		sum += fabs(segment.X1 - segment.X2);
	}
	avg = sum / segments.size();
	edSum->Text = ValueStringFormatter::TimeToStr(sum, 3).c_str();
	edAverage->Text = ValueStringFormatter::TimeToStr(avg, 3).c_str();
}

void __fastcall TfrmStatistics::lvSegmentsData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	Item->Caption = id+1;
	struct Segment &segment = segments[id];
	Item->SubItems->Add(ValueStringFormatter::TimeToStr(segment.X1, 3).c_str());
	Item->SubItems->Add(ValueStringFormatter::TimeToStr(segment.X2, 3).c_str());
	Item->SubItems->Add(ValueStringFormatter::TimeToStr(fabs(segment.X1 - segment.X2), 3).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TfrmStatistics::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;	
}
//---------------------------------------------------------------------------
void __fastcall TfrmStatistics::btnDeleteSelectedClick(TObject *Sender)
{
	if (!lvSegments->Selected)
		return;
	int id = lvSegments->Selected->Index;
	segments.erase(segments.begin()+id);
	UpdateData();
}
//---------------------------------------------------------------------------
