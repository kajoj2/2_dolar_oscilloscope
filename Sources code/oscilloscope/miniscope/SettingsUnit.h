/** \file
    \brief Common settings window header file
*/

//---------------------------------------------------------------------------
#ifndef SettingsUnitH
#define SettingsUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "Settings.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Graphics.hpp>
#include <Dialogs.hpp>

class TfrmHotkeys;

/** \brief Common application settings manipulation window
*/
class TfrmSettings : public TfrmCommon
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TTabSheet *tsScopePlugins;
	TTabSheet *tsOther;
	TLabel *lblUiCapacity;
	TCheckBox *chbAlwaysOnTop;
	TComboBox *cmbMaxUiLogLines;
	TPanel *pnlBottom;
	TPanel *pnlBottomRight;
	TButton *btnApply;
	TButton *btnCancel;
	TListView *lvScopePlugins;
	TImageList *imglDllList;
	TPanel *pnlScopePluginsHelp;
	TImage *imgSelected;
	TLabel *lblDllInfo2;
	TTabSheet *tsGraph;
	TLabel *lblRefreshingFreq;
	TComboBox *cbGraphRefreshingFreq;
	TLabel *lblDllInfo3;
	TLabel *lblPlotToolbarTransparency;
	TTrackBar *tbarPlotToolbarTransparency;
	TLabel *lblTransparencyMax;
	TLabel *lblTransparencyNone;
	TLabel *lblMs3RecorderFileSizeLimit;
	TEdit *edMs3RecorderFileSizeLimit;
	TTabSheet *tsFFT;
	TLabel *lblFftSamplesLimit;
	TComboBox *cbFftSamplesLimit;
	TLabel *lblFftWindowType;
	TComboBox *cbFftWindowType;
	TMemo *memoFftWindowDescription;
	TTabSheet *tsGraphColors;
	TPanel *pnlGraphColorsBottom;
	TListView *lvGraphTraceColors;
	TButton *btnAddGraphTraceColor;
	TButton *btnRemoveGraphTraceColor;
	TButton *btnChangeGraphTraceColor;
	TButton *btnDefaultSetGraphTraceColor;
	TColorDialog *ColorDialog;
	TCheckBox *chbLogToFile;
	TCheckBox *chbSmoothTraceMoving;
	TButton *btnRefreshDllList;
	TCheckBox *chbZoomKeepRound;
	TLabel *lblPlotLineWidth;
	TEdit *edGraphPlotLineWidth;
	TTabSheet *tsHotkeys;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnApplyClick(TObject *Sender);
	void __fastcall chbAlwaysOnTopClick(TObject *Sender);
	void __fastcall cmbMaxUiLogLinesChange(TObject *Sender);
	void __fastcall lvScopePluginsData(TObject *Sender, TListItem *Item);
	void __fastcall cbGraphRefreshingFreqChange(TObject *Sender);
	void __fastcall tbarPlotToolbarTransparencyChange(TObject *Sender);
	void __fastcall cbFftSamplesLimitChange(TObject *Sender);
	void __fastcall cbFftWindowTypeChange(TObject *Sender);
	void __fastcall btnAddGraphTraceColorClick(TObject *Sender);
	void __fastcall lvGraphTraceColorsDrawItem(TCustomListView *Sender,
          TListItem *Item, TRect &Rect, TOwnerDrawState State);
	void __fastcall btnChangeGraphTraceColorClick(TObject *Sender);
	void __fastcall btnRemoveGraphTraceColorClick(TObject *Sender);
	void __fastcall btnDefaultSetGraphTraceColorClick(TObject *Sender);
	void __fastcall btnRefreshDllListClick(TObject *Sender);
private:	// User declarations
	TfrmHotkeys *frmHotkeys;
	void SetFftWindowDescription(void);
	void EditGraphTraceColor(void);
public:		// User declarations
	__fastcall TfrmSettings(TComponent* Owner);
	Settings *appSettings;
	Settings tmpSettings;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
#endif
