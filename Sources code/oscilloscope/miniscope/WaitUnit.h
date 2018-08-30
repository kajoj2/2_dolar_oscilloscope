//---------------------------------------------------------------------------

#ifndef WaitUnitH
#define WaitUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmWait : public TfrmCommon
{
__published:	// IDE-managed Components
	TBevel *Bevel;
	TLabel *Label1;
private:	// User declarations
public:		// User declarations
	__fastcall TfrmWait(TComponent* Owner);
	void Show(AnsiString asText);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmWait *frmWait;
//---------------------------------------------------------------------------
#endif
