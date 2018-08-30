//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "AboutUnit.h"
#include "Utilities.h"

//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TAboutBox *AboutBox;

//--------------------------------------------------------------------- 
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
	: TfrmCommon(AOwner)
{
	lblVersion->Caption = GetFileVer(Application->ExeName);
	lblBuildTimestamp->Caption = (AnsiString)__DATE__ + ", " __TIME__;

#ifdef __CODEGUARD__
	lblCodeGuardState->Caption = _("YES");
	btnCodeGuardTest->Visible = true;
#else
	lblCodeGuardState->Caption = _("NO");
#endif

#ifdef _DEBUG
	lblDebugState->Caption = _("YES");
#else
	lblDebugState->Caption = _("NO");
#endif

#if 0
	TP_Ignore(this, "lblVersion");
	TP_Ignore(this, "lblBuildTimestamp");
	TP_Ignore(this, "lblInfo2");
#endif

	lblExeLocation->Caption = Application->ExeName;
}
//---------------------------------------------------------------------
void __fastcall TAboutBox::lblInfo2Click(TObject *Sender)
{
	ShellExecute(NULL, "open", lblInfo2->Caption.c_str(), NULL, NULL, SW_SHOWNORMAL);	
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::OKButtonClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::btnCodeGuardTestClick(TObject *Sender)
{
	char test1[3];
	char test2[3];
	char test3[3];	// safe spot for access overrun
	(void)test1;
	(void)test3;
	strcpy(test2, "TEST");	
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
        Close();
    }	
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::lblExeLocationClick(TObject *Sender)
{
	ShellExecute(NULL, "explore", ExtractFileDir(Application->ExeName).c_str(), NULL, NULL, SW_SHOWNORMAL);	
}
//---------------------------------------------------------------------------
 
