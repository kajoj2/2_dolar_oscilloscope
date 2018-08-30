//---------------------------------------------------------------------------

#ifndef Ms3ViewerUnitH
#define Ms3ViewerUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <vector>
#include "device/Data.h"
//---------------------------------------------------------------------------

class Display;
class Data;

class TfrmMs3Viewer : public TfrmCommon
{
__published:	// IDE-managed Components
	TLabel *lblSampling;
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
	TLabel *lblFrame;
	TLabel *lblFrameVal;
	TButton *btnPrevFrame;
	TButton *btnNextFrame;
	TBitBtn *btnFrameList;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnExitViewerClick(TObject *Sender);
	void __fastcall btnPrevFrameClick(TObject *Sender);
	void __fastcall btnNextFrameClick(TObject *Sender);
	void __fastcall btnFrameListClick(TObject *Sender);
private:	// User declarations
	friend class TfrmMs3FrameList;
	typedef void (__closure *CallbackClose)(void);
	typedef void (__closure *CallbackUpdate)(Data &ms3data);
	Data data;
	unsigned int frame_cnt;
	AnsiString asCurrentFile;
	CallbackUpdate callbackUpdate;
	int LoadFrames(int first, int count);
	unsigned int iCurrentChannel;
public:		// User declarations
	__fastcall TfrmMs3Viewer(TComponent* Owner);
	CallbackClose callbackClose;
	int LoadFile(AnsiString asFileName, CallbackUpdate cbUpdate);
	void SetCurrentChannel(unsigned int channel);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMs3Viewer *frmMs3Viewer;
//---------------------------------------------------------------------------
#endif
