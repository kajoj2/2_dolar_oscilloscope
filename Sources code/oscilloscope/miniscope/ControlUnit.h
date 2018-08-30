//---------------------------------------------------------------------------

#ifndef ControlUnitH
#define ControlUnitH
//---------------------------------------------------------------------------
#include "Mode.h"
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

class DeviceInterface;
class Display;
class Mutex;
class Data;

class TfrmControl : public TfrmCommon
{
__published:	// IDE-managed Components
	TPanel *pnlOscControl;
	TPanel *pnlOscControlBottom;
	TLabel *lblSampling;
	TLabel *lblSamplingVal;
	TLabel *lblTriggerType;
	TLabel *lblTriggerLevel;
	TLabel *lblTriggerLevelVal;
	TLabel *lblBuffer;
	TLabel *lblInterval;
	TLabel *lblSeconds;
	TLabel *lblTriggerSource;
	TLabel *lblTriggerPosition;
	TLabel *lblTriggerPositionVal;
	TTrackBar *trbarSampling;
	TComboBox *cbTriggerType;
	TTrackBar *trbarTriggerLevel;
	TButton *btnStart;
	TButton *btnStop;
	TButton *btnManualTrigger;
	TComboBox *cbBufferSize;
	TProgressBar *ProgressBarFrameRx;
	TEdit *edInterval;
	TButton *btnRecorderStart;
	TButton *btnRecorderStop;
	TButton *btnRecorderClear;
	TComboBox *cbTriggerSource;
	TTrackBar *trbarTriggerPosition;
	TTabControl *tabOscControlTop;
	TLabel *lblSensitivity;
	TLabel *lblSensitivityVal;
	TLabel *lblCouplingType;
	TTrackBar *trbarSensitivity;
	TComboBox *cbCouplingType;
	TLabel *lblTriggerMode;
	TComboBox *cbTriggerMode;
	TTimer *tmrRecorder;
	TCheckBox *chbChannelEnable;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall TrackbarKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall cbTriggerTypeChange(TObject *Sender);
	void __fastcall cbBufferSizeChange(TObject *Sender);
	void __fastcall cbCouplingTypeChange(TObject *Sender);
	void __fastcall cbTriggerModeChange(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tabOscControlTopDrawTab(TCustomTabControl *Control,
          int TabIndex, const TRect &Rect, bool Active);
	void __fastcall btnManualTriggerClick(TObject *Sender);
	void __fastcall btnRecorderStartClick(TObject *Sender);
	void __fastcall btnRecorderStopClick(TObject *Sender);
	void __fastcall tmrRecorderTimer(TObject *Sender);
	void __fastcall btnRecorderClearClick(TObject *Sender);
	void __fastcall chbChannelEnableClick(TObject *Sender);
	void __fastcall chbChannelEnableKeyPress(TObject *Sender, char &Key);	
private:	// User declarations
	void UpdateTrackBar(TObject *Sender);
	void __fastcall TrackBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	Mutex *mutexDeviceData;
	Data *current; 	
	DeviceInterface *device;                ///< pointer to object that interfaces with data acquisision hardware
	Display *display;
	bool *bDataUpdate;
	bool *bPaintUpdate;
	/** \brief Change device sensitivity/gain
	*/
	void UpdateSensitivity(bool bUpdateDevice = true);
	/** \brief Change device sampling rate
	*/
	void UpdateSampling(bool bUpdateDevice = true);
	/** \brief Update device triggering voltage level
	*/
    void UpdateTriggerLevel(void);
	/** \brief Update device triggering position (pre-/post-trigger)
	*/
	void UpdateTriggerPosition(void);

	void UpdateTriggerMode(bool bUpdateDevice = true);
	/** \brief Change device trigger type and update GUI when trigger type is changed by user
	*/
	void UpdateTriggerType(bool bUpdateDevice = true);
	/** \brief Update channel enable/disable state
	*/
	void UpdateChannelEnable(void);
	/** \brief Change device coupling (AC/DC/...) type and update GUI when coupling type is changed by user
	*/
	void UpdateCouplingType(void);
	/** \brief Change size of device acquisition buffer
	*/
	void UpdateBufferSize(void);
	
	typedef void(__closure *CallbackOnUpdateTriggerMode)(void);
public:		// User declarations
	__fastcall TfrmControl(TComponent* Owner);

	void Configure(Mutex *mutexDeviceData, Data *current, DeviceInterface *device, Display *display,
		bool *bDataUpdate, bool *bPaintUpdate);
	void SetCapabilities(const struct S_SCOPE_CAPABILITIES* const capabilities);		
	void UpdateState(bool bUpdateDevice);
	void SetMode(const enum E_MODE m);
	void ManualTrigger(void);

	CallbackOnUpdateTriggerMode OnUpdateTriggerMode;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmControl *frmControl;
//---------------------------------------------------------------------------
#endif
