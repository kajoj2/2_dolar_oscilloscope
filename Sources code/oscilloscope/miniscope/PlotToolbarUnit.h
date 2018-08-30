//---------------------------------------------------------------------------

#ifndef PlotToolbarUnitH
#define PlotToolbarUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TfrmPlotToolbar : public TfrmCommon
{
__published:	// IDE-managed Components
	TImage *imgShape;
	TImage *imgCursor;
	TImage *imgHand;
	TImage *imgZoom;
	TImage *imgZoomClear;
	TImage *imgZoomX;
	TImage *imgZoomY;
	TShape *ShapeAreaCursor;
	TImage *imgHighlightCursor;
	TImage *imgHighlightHand;
	TImage *imgHighlightZoomClear;
	TImage *imgHighlightZoomY;
	TImage *imgHighlightZoomX;
	TImage *imgHighlightZoom;
	TShape *ShapeAreaHand;
	TShape *ShapeAreaZoomClear;
	TShape *ShapeAreaZoomY;
	TShape *ShapeAreaZoomX;
	TShape *ShapeAreaZoom;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall imgShapeClick(TObject *Sender);
	void __fastcall ShapeAreaMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ShapeAreaMouseEnter(TObject *Sender);
	void __fastcall ShapeAreaMouseLeave(TObject *Sender);
private:	// User declarations
	HRGN __fastcall CreateRegion(Graphics::TBitmap* Bmp);
	typedef void (__closure *CallbackClick)(int tag);
public:		// User declarations
	__fastcall TfrmPlotToolbar(TComponent* Owner);
	void ShowAt(int X, int Y);
	CallbackClick callbackClick;
	/** \brief Set transparency level is supported by OS
		\param level transparency level, 0...255, 0 = complete transparency
	*/
	void SetTransparecy(int alpha);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPlotToolbar *frmPlotToolbar;
//---------------------------------------------------------------------------
#endif
