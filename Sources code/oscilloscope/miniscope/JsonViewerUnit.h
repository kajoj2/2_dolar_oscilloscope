//---------------------------------------------------------------------------

#ifndef JsonViewerUnitH
#define JsonViewerUnitH

#include "CommonUnit.h"
#include "device/Data.h"

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include <vector>

class TfrmJsonViewer : public TfrmCommon
{
__published:	// IDE-managed Components
	TLabel *lblSensitivity;
	TLabel *lblSampling;
	TLabel *lblSensitivityVal;
	TLabel *lblSamplingVal;
	TLabel *lblTriggerType;
	TLabel *lblCouplingType;
	TLabel *lblTimestamp;
	TPanel *pnlBottom;
	TButton *btnExitViewer;
	TLabel *lblTriggerTypeVal;
	TLabel *lblCouplingTypeVal;
	TLabel *lblTimestampVal;
	TLabel *lblSamplesCount;
	TLabel *lblSamplesCountVal;
	TEdit *edFilename;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnExitViewerClick(TObject *Sender);
private:	// User declarations
	typedef void (__closure *CallbackClose)(void);
	int current_channel;
	void UpdateChannel(void);
public:		// User declarations
	__fastcall TfrmJsonViewer(TComponent* Owner);
	int LoadFile(AnsiString filename);
	CallbackClose callbackClose;
	struct JsonData
	{
		Data data;
		AnsiString asTimestamp;
		int iTriggerType;
		int iTriggerSource;
	} jsonData;
	void SetCurrentChannel(int channel);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmJsonViewer *frmJsonViewer;
//---------------------------------------------------------------------------
#endif
