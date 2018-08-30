//---------------------------------------------------------------------------

#ifndef Ms3FileRepairUnitH
#define Ms3FileRepairUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TfrmMs3FileRepair : public TfrmCommon
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnClose;
	TLabel *lblInfo;
	TProgressBar *ProgressBar;
	TButton *btnCreateFixedFile;
	TLabel *lblErrorType;
	TLabel *lblErrorTypeVal;
	TSaveDialog *SaveDialog;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnCreateFixedFileClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	__fastcall Show(void);
	__fastcall ShowModal(void);
	friend DWORD WINAPI ThreadRepairProc(LPVOID data);
public:		// User declarations
	__fastcall TfrmMs3FileRepair(TComponent* Owner);
	enum E_STATUS
	{
		E_FIXED = 0,
		E_IN_PROGRESS,
		E_ERRORFIXING,	///< error occured during fixing
		E_ABORTED,		///< error possible to fix, but form closed
		E_NOTHINGTODO,	///< file is not damaged
		E_CANTFIX		///< type of problem not possible to fix or unhandled
	};
	/** \brief Try to repair specified file
	*/
	void Execute(AnsiString filename, enum E_STATUS &status);
	AnsiString GetStatusDescription(enum E_STATUS status);
private:
	struct S_FIX_STATUS
	{
		enum E_STATUS *status;
		int progress;
		AnsiString filename;
		AnsiString filename_output;
	} fix_status;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMs3FileRepair *frmMs3FileRepair;
//---------------------------------------------------------------------------
#endif
