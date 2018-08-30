//---------------------------------------------------------------------------

#ifndef StatisticsUnitH
#define StatisticsUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

#include "CommonUnit.h"
#include "Mode.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <vector>

class TfrmStatistics : public TfrmCommon
{
__published:	// IDE-managed Components
	TPanel *pnlToolbar;
	TButton *btnAddCurrentSegment;
	TPanel *pnlCenter;
	TListView *lvSegments;
	TButton *btnDeleteSelected;
	TLabel *lblSum;
	TLabel *lblAverage;
	TEdit *edSum;
	TEdit *edAverage;
	void __fastcall btnAddCurrentSegmentClick(TObject *Sender);
	void __fastcall lvSegmentsData(TObject *Sender, TListItem *Item);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnDeleteSelectedClick(TObject *Sender);
private:	// User declarations
	typedef void (__closure *GetCursorCoordinates)(enum E_MODE &mode,
		float &X1, float &Y1, float &X2, float &Y2);
	GetCursorCoordinates getCursorCoordinatesCb;
	struct Segment
	{
		float X1, Y1, X2, Y2;
	};
	std::vector<struct Segment> segments;
	void UpdateData(void);
public:		// User declarations
	__fastcall TfrmStatistics(TComponent* Owner, int id,
		GetCursorCoordinates getCursorCoordinatesCb);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmStatistics *frmStatistics;
//---------------------------------------------------------------------------
#endif
