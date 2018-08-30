//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WaitUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmWait *frmWait;
//---------------------------------------------------------------------------
__fastcall TfrmWait::TfrmWait(TComponent* Owner)
	: TfrmCommon(Owner)
{
}
//---------------------------------------------------------------------------

void TfrmWait::Show(AnsiString asText)
{
	Label1->Caption = asText;
	TForm::Show();
	BringToFront();
	Update();
}

