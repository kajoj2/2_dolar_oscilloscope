/** \file Main.h
    \brief Application main form header file
*/

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include "display/Display.h"
#include "device/Data.h"
#include "CommonUnit.h"
#include "Mutex.h"
#include "Observer.h"
#include <ActnList.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <ToolWin.hpp>
#include <StdActns.hpp>
#include <Tabs.hpp>
#include <CategoryButtons.hpp>
#include <memory>


class DeviceInterface;
struct S_SCOPE_CAPABILITIES;
struct HotKeyConf;
struct Action;

//---------------------------------------------------------------------------
/** \brief Main application window
*/
class TfrmMain : public TfrmCommon, Observer
{
__published:	// IDE-managed Components
	TPanel *pnlCenter;
	TTimer *Timer;
	TStatusBar *StatusBar;
	TMainMenu *Menu;
	TMenuItem *File1;
	TMenuItem *miHelp;
	TMenuItem *E1;
	TMenuItem *miSettings;
	TImageList *imglToolbar;
	TCoolBar *CoolBar;
	TToolBar *ToolBarMode;
	TToolButton *btnModeScope;
	TToolButton *btnModeSpectrum;
	TToolButton *btnModeRecorder;
	TToolBar *ToolBarMouse;
	TToolButton *btnMouseCursor;
	TToolButton *btnMouseZoomXY;
	TToolButton *btnMouseMove;
	TActionList *ActionList;
	TAction *ActionChangeMouseModeCursor;
	TAction *ActionChangeMouseModeOffset;
	TAction *ActionChangeMouseModeZoom;
	TToolButton *btnMouseZoomX;
	TToolButton *btnMouseZoomY;
	TAction *ActionChangeMouseModeZoomX;
	TAction *ActionChangeMouseModeZoomY;
	TToolButton *btnZoomClear;
	TAction *ActionZoomClear;
	TAction *ActionModeScope;
	TAction *ActionModeSpectrum;
	TMenuItem *miAbout;
	TMenuItem *miCommonSettings;
	TMenuItem *miDeviceSettings;
	TMenuItem *miView;
	TMenuItem *miViewLog;
	TTimer *tmrUpdateConnState;
	TPaintBox *PaintBoxCenter;
	TAction *ActionCopyPlotClipbrd;
	TMenuItem *miCopyPlotToClipbrd;
	TPanel *pnlZoomInfo;
	TLabel *lblZoomX;
	TLabel *lblZoomY;
	TMenuItem *miTools;
	TMenuItem *miMeasurements;
	TAction *ActionShowCommonSettings;
	TAction *ActionModeRecorder;
	TMenuItem *miExportPlotBitmap;
	TAction *ActionExportPlotBitmap;
	TSaveDialog *SaveDialog;
	TAction *ActionCalibrateSensitivity;
	TMenuItem *miCalibrateSensitivity;
	TAction *ActionExportData;
	TMenuItem *miExportcurrentdatatoJSONfile;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *btinAutoFitY;
	TAction *ActionAutoFitY;
	TAction *ActionAutoFitX;
	TToolButton *btnAutoFitX;
	TMenuItem *miLoadJsonFile;
	TAction *ActionLoadJsonFile;
	TPanel *pnlRight;
	TFileExit *FileExit1;
	TMenuItem *miExit;
	TMenuItem *N1;
	TOpenDialog *OpenDialog;
	TAction *ActionShowDataFilterChain;
	TMenuItem *Datafilterchain1;
	TAction *ActionMs3Record;
	TMenuItem *RecorddatatoMS3file1;
	TMenuItem *N2;
	TMenuItem *N3;
	TAction *ActionMs3Load;
	TMenuItem *LoadMS3file1;
	TMenuItem *miMs3FileRepair;
	TAction *ActionMs3FileRepair;
	TMenuItem *miLanguage;
	TAction *ActionViewPlotGridX;
	TMenuItem *miPlot;
	TMenuItem *GridX1;
	TAction *ActionViewPlotGridY;
	TMenuItem *GridY1;
	TAction *ActionViewPlotGridLabelsX;
	TAction *ActionViewPlotGridLabelsY;
	TMenuItem *GridlabelsX1;
	TMenuItem *GridlabelsY1;
	TAction *ActionViewPlotCursors;
	TAction *ActionViewPlotGndMarkers;
	TMenuItem *Cursors1;
	TMenuItem *N0Vmarkers1;
	TLabel *lblXscale;
	TLabel *lblYscale;
	TAction *ActionViewPlotTriggerPosMarker;
	TMenuItem *Showtriggerpositionpretriggermarker1;
	TAction *ActionViewPlotTriggerValMarker;
	TMenuItem *riggervaluemarker1;
	TMenuItem *miStatistics;
	TMenuItem *miDevice;
	TMenuItem *miDeviceConnect;
	TMenuItem *miDeviceDisconnect;
	TAction *ActionViewPlotRangeValues;
	TAction *ActionViewPlotCursorValues;
	TMenuItem *Rangevalues1;
	TMenuItem *Cursorcoordinates1;
	TMenuItem *miLoadRecentJson;
	TMenuItem *miLoadRecentMs3;
	TToolBar *ToolBarQuickAccess;
	TToolButton *ToolButton3;
	TAction *ActionQuickPlotImageSave;
	TTimer *tmrAntirepeat;
	void __fastcall pnlCenterResize(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall pnlCenterMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ActionChangeMouseModeCursorExecute(TObject *Sender);
	void __fastcall ActionChangeMouseModeOffsetExecute(TObject *Sender);
	void __fastcall ActionChangeMouseModeZoomExecute(TObject *Sender);
	void __fastcall ActionChangeMouseModeZoomXExecute(TObject *Sender);
	void __fastcall ActionChangeMouseModeZoomYExecute(TObject *Sender);
	void __fastcall ActionZoomClearExecute(TObject *Sender);
	void __fastcall pnlCenterMouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall ActionModeScopeExecute(TObject *Sender);
	void __fastcall ActionModeSpectrumExecute(TObject *Sender);
	void __fastcall miAboutClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall miDeviceSettingsClick(TObject *Sender);
	void __fastcall miViewLogClick(TObject *Sender);
	void __fastcall tmrUpdateConnStateTimer(TObject *Sender);
	void __fastcall PaintBoxCenterPaint(TObject *Sender);
	void __fastcall PaintBoxCenterMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PaintBoxCenterMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall ActionCopyPlotClipbrdExecute(TObject *Sender);
	void __fastcall miMeasurementsClick(TObject *Sender);
	void __fastcall ActionShowCommonSettingsExecute(TObject *Sender);
	void __fastcall ActionModeRecorderExecute(TObject *Sender);
	void __fastcall ActionExportPlotBitmapExecute(TObject *Sender);
	void __fastcall ActionCalibrateSensitivityExecute(TObject *Sender);
	void __fastcall ActionExportDataExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ActionAutoFitYExecute(TObject *Sender);
	void __fastcall ActionAutoFitXExecute(TObject *Sender);
	void __fastcall ActionLoadJsonFileExecute(TObject *Sender);
	void __fastcall ActionShowDataFilterChainExecute(TObject *Sender);
	void __fastcall ActionMs3RecordExecute(TObject *Sender);
	void __fastcall ActionMs3LoadExecute(TObject *Sender);
	void __fastcall ActionMs3FileRepairExecute(TObject *Sender);
	void __fastcall edIntervalExit(TObject *Sender);
	void __fastcall tabOscControlTopChange(TObject *Sender);
	void __fastcall ActionViewPlotExecute(TObject *Sender);
	void __fastcall PaintBoxCenterMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall pnlCenterMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
	void __fastcall miStatisticsClick(TObject *Sender);
	void __fastcall miDeviceConnectClick(TObject *Sender);
	void __fastcall miDeviceDisconnectClick(TObject *Sender);
	void __fastcall File1Click(TObject *Sender);
	void __fastcall ActionQuickPlotImageSaveExecute(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall tmrAntirepeatTimer(TObject *Sender);

private:	// User declarations
	/** \brief Pointer to plot graph object
	*/
	Display *display;
	/** \brief Type of tool to use with plot graph
	*/
	enum E_MOUSE_TOOL { MOUSE_CURSOR = 0,
						MOUSE_OFFSET,
						MOUSE_ZOOM,
						MOUSE_ZOOM_X,
						MOUSE_ZOOM_Y } mouse_tool;
    /** \brief Resources ID's used as cursors for plot graph
    */
	enum E_CURSOR_RES { CURSOR_HAND = 100,
						CURSOR_ZOOM };

	/** \brief Plot zoom multiplier for every click with zoom tool
	*/
	const float fZoomChangeGrid;
	void UpdateCursorReadings(enum E_MODE, int cursorId, float fValX, float fValY);

	/** \brief Create TImage class object that holds current plot image
	*/
	std::auto_ptr<Graphics::TBitmap> CreatePlotBitmap(void);

	void CalculateSpectrum(void);
	void UpdateFftWindow(void);				///< set FFT window type and size (precompute coefficients)

	int iPlotMoveStartX, iPlotMoveStartY;	///< temporary variables used to pass offset vector to display
	bool bMovingPlot;                       ///< are we moving plot right now?
	bool bDataUpdate;						///< flag set when new data is acquired
	bool bPaintUpdate;                      ///< flag set when plot canvas has to be refreshed
	bool bFrameProgressUpdate;				///< flag requesting RX progress bar update
	bool bTriggered;
	bool bDataDivXUpdate;
	bool bJsonViewerMode;					///< set when viewing JSON file content
    float fMs3DivX;
	bool bMs3ViewerMode;					///< set when viewing MS3 file content

	AnsiString asDevDllDir;					///< device dll's directory
	DeviceInterface *device;                ///< pointer to object that interfaces with data acquisision hardware
	Mutex mutexDeviceData;                  ///< mutex that prevents hazards with simultaneous data updating by device and data plotting

	Data current;							///< state at the moment of trigger

	void UpdateConnectionState(void);
	/** \brief Update user interface to reflect settings types and ranges offered by device
        \param capabilities ptr to structure describing device properties
	*/
	void SetCapabilities(const struct S_SCOPE_CAPABILITIES* const capabilities);
	/** \brief Use supplied data structure */
	void UseData(Data &data);

	void LoadDevice(AnsiString dllname);
	void ConnectDevice(void);
	void DisconnectDevice(void);

	/** \brief Display dialog when input device is not selected
		(probably first run of application)
	*/
	void FirstRunDialog(void);

	void SetMode(enum E_MODE);

	void ApplyCalibration(void);

	/** \brief Report program failure with messagebox and log entry.
		Do nothing if status == 0.
	*/
	void ReportError(int status, AnsiString description);
	/** \brief Get current (main) channel */
	int CurrentChannel(void);

	void UpdateDisplayVisibility(void);

	void __fastcall miSelectLangClick(TObject *Sender);

	void UpdateMruMenu(void);
	void AddMruJsonItem(AnsiString filename);
	void __fastcall MruJsonClick(TObject *Sender);
	void AddMruMs3Item(AnsiString filename);
	void __fastcall MruMs3Click(TObject *Sender);

	void LoadJson(AnsiString filename);
	void LoadMs3(AnsiString filename);

	void UpdateZoomCfg(void);

	const HotKeyConf *lastHotkey;	
	void RegisterGlobalHotKeys(void);
	void ExecAction(const struct Action& action);


	void __fastcall WMXButtonDown(TWMMouse& msg);
	void __fastcall WMHotKey(TWMHotKey &Message);


public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
	__fastcall ~TfrmMain();
	/** \brief Called when log window is closed
	*/
	void CallbackCloseLogFn(void);
	/** \brief Called by device when new data is received
	*/
	void CallbackDataFn(void);
	/** \brief Called by device when connection state is changed
        \param state current connection state
        \param szMsgText additional text message related to current connection state
	*/
	void CallbackConnectFn(int state, const char *szMsgText);
	/** \brief Called by device when triggered
	*/
	void CallbackTriggerFn(void);

	/** \brief Request calibration calculation
		\param fVoltageHigh real voltage at left cursor
		\param offset returned 0V offset
		\param gain returned real sensitivity/declared sensitivity coeff
		\return 0 on success
	*/
	int CallbackCalculateCalibrationFn(float fVoltageHigh,	int* const offset, float* const gain);
	/** \brief Store calibration data to device
	*/
	int CallbackStoreCalibrationFn(const int offset, const float gain);

	void CallbackPlotToolbarClickFn(int tag);
	/** \brief Called when closing JSON/MS3 viewer
	*/
	void CallbackFileViewerClose(void);
	/** \brief Called when MS3 viewer updates data
	*/
	void CallbackMs3ViewerDataUpdate(Data &ms3data);

	void CallbackGetCursorCoordinates(enum E_MODE &mode,
		float &X1, float &Y1, float &X2, float &Y2);

	void obsUpdate(Observable* o, Argument * arg);

	BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_XBUTTONDOWN, TWMMouse, WMXButtonDown)
		VCL_MESSAGE_HANDLER(WM_HOTKEY, TWMHotKey, WMHotKey)
	END_MESSAGE_MAP(TForm)

};

//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
