//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PlotToolbarUnit.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmPlotToolbar *frmPlotToolbar;
//---------------------------------------------------------------------------
__fastcall TfrmPlotToolbar::TfrmPlotToolbar(TComponent* Owner)
	: TfrmCommon(Owner)
{
	callbackClick = NULL;
}
//---------------------------------------------------------------------------


HRGN __fastcall TfrmPlotToolbar::CreateRegion(Graphics::TBitmap* Bmp)
{
	Bmp->PixelFormat = pf24bit;

	// Create a region of the whole bitmap
	// later we will take the transparent
	// bits away
	HRGN Result = ::CreateRectRgn(0, 0, Bmp->Width, Bmp->Height);

	int Y = 0;
	RGBTRIPLE* Row = (RGBTRIPLE*)Bmp->ScanLine[Y];
	TRGBTriple TransparentColor = Row[0];

	// Loop down the bitmap
	for (Y = 0; Y <= Bmp->Height - 1;)
	{
		int X,
			StartX = -1;	// start of transparent fragment not found yet

		// Loop across the row
        for (X = 0; X <= Bmp->Width; X++)
        {
            // Check for transparency by comparing the color
			if((X != Bmp->Width)&&
					(Row[X].rgbtRed == TransparentColor.rgbtRed) &&
                    (Row[X].rgbtGreen == TransparentColor.rgbtGreen) &&
                    (Row[X].rgbtBlue == TransparentColor.rgbtBlue))
            {
				// is transparent
				if (StartX == -1)
                    StartX = X;	// start of transparent fragment found
            }
            else
            {
				// not transparent
				if (StartX > -1)
				{
					// end of transparent fragment found, add it to excluded
					HRGN Excl = ::CreateRectRgn(StartX, Y, X, Y + 1);
                    try
                    {
                        // Remove the exclusion from our original region
                        ::CombineRgn(Result, Result, Excl, RGN_DIFF);
						// Reset StartX so we can start searching
                        // for the next transparent area
                        StartX = -1;
                    }
                    __finally
                    {
                        ::DeleteObject(Excl);
                    }
                }
            }
		}

		++Y;
		if (Y < Bmp->Height)
			Row = (RGBTRIPLE*)Bmp->ScanLine[Y];		
    }

    return Result;
}
void __fastcall TfrmPlotToolbar::FormCreate(TObject *Sender)
{
	DoubleBuffered = true;

	Graphics::TBitmap* Bmp = new Graphics::TBitmap();
	try
	{
		Bmp->Assign(imgShape->Picture->Bitmap);
		HRGN FRegion = CreateRegion(imgShape->Picture->Bitmap);
		SetWindowRgn(Handle, FRegion, true);
	}
	__finally
	{
		delete Bmp;
	}	
}
//---------------------------------------------------------------------------
void __fastcall TfrmPlotToolbar::imgShapeClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlotToolbar::ShapeAreaMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	Close();
/*
	if (Sender == ShapeAreaCursor)
		ShowMessage("cursor");
	if (Sender == ShapeAreaHand)
		ShowMessage("hand");
	if (Sender == ShapeAreaZoom)
		ShowMessage("zoom");
	if (Sender == ShapeAreaZoomX)
		ShowMessage("zoomX");
	if (Sender == ShapeAreaZoomY)
		ShowMessage("zoomY");
	if (Sender == ShapeAreaZoomClear)
		ShowMessage("zoomClear");
*/
	if (callbackClick)
		callbackClick((dynamic_cast<TShape*>(Sender))->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPlotToolbar::ShapeAreaMouseEnter(TObject *Sender)
{
	if (Sender == ShapeAreaCursor)
		imgHighlightCursor->Visible = true;
	if (Sender == ShapeAreaHand)
		imgHighlightHand->Visible = true;
	if (Sender == ShapeAreaZoom)
		imgHighlightZoom->Visible = true;
	if (Sender == ShapeAreaZoomX)
		imgHighlightZoomX->Visible = true;
	if (Sender == ShapeAreaZoomY)
		imgHighlightZoomY->Visible = true;
	if (Sender == ShapeAreaZoomClear)
		imgHighlightZoomClear->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPlotToolbar::ShapeAreaMouseLeave(TObject *Sender)
{
	imgHighlightCursor->Visible = false;
	imgHighlightHand->Visible = false;
	imgHighlightZoom->Visible = false;
	imgHighlightZoomX->Visible = false;
	imgHighlightZoomY->Visible = false;
	imgHighlightZoomClear->Visible = false;
}
//---------------------------------------------------------------------------

void TfrmPlotToolbar::ShowAt(int X, int Y)
{
	Left = X - imgShape->Left - imgShape->Width/2;
	Top = Y - imgShape->Top - imgShape->Height/2;
	Show();
	BringToFront();
}

void TfrmPlotToolbar::SetTransparecy(int alpha)
{
    assert (alpha >= 0 && alpha <= 255);
	typedef DWORD (WINAPI *FWINLAYER)(HWND hwnd, DWORD crKey, BYTE bAlpha, DWORD dwFlags);
	HINSTANCE hDll = LoadLibrary("user32.dll");
	FWINLAYER setLayeredWindowAttributes = (FWINLAYER)GetProcAddress(hDll,"SetLayeredWindowAttributes");

	if(setLayeredWindowAttributes != NULL)
	{
		SetWindowLong(this->Handle, GWL_EXSTYLE, WS_EX_LAYERED);
		setLayeredWindowAttributes(this->Handle, 0, alpha, 2);
	}
}
