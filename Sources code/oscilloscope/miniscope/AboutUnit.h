/** \brief "About program" window header file
*/

//----------------------------------------------------------------------------
#ifndef AboutUnitH
#define AboutUnitH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <ExtCtrls.hpp>
//----------------------------------------------------------------------------

#include "CommonUnit.h"

class TAboutBox : public TfrmCommon
{
__published:
	TPanel *Panel1;
	TImage *ProgramIcon;
	TLabel *ProductName;
	TLabel *Copyright;
	TLabel *Comments;
	TButton *OKButton;
	TLabel *lblVersion;
	TLabel *lblInfo1;
	TLabel *lblInfo2;
	TLabel *lblInfo3;
	TLabel *lblBuilt;
	TLabel *lblBuildTimestamp;
	TLabel *lblCodeguard;
	TLabel *lblCodeGuardState;
	TLabel *lblCodeGuardInfo;
	TLabel *lblDebug;
	TLabel *lblDebugState;
	TButton *btnCodeGuardTest;
	TMemo *Memo;
	TLabel *lblThirdPartyInfo;
	TLabel *lblLocation;
	TLabel *lblExeLocation;
	void __fastcall lblInfo2Click(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall btnCodeGuardTestClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall lblExeLocationClick(TObject *Sender);
private:
public:
	virtual __fastcall TAboutBox(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TAboutBox *AboutBox;
//----------------------------------------------------------------------------
#endif
