/** \file Settings.h
*/

#ifndef SettingsH
#define SettingsH
//---------------------------------------------------------------------------
#include "HotKeyConf.h"
#include <System.hpp>
#include <vector>
#include <deque>
#include <list>

/** \brief Common (not related to any dll) program settings with Read/Write functions
*/
class Settings
{
public:
	int Read(AnsiString asFileName);  ///< write settings to ini file
	int Write(AnsiString asFileName); ///< read settings from ini file
	void SetDefault(void);
    Settings(void);

	struct S_FRM_MAIN
	{
		int iPosX, iPosY;                  ///< main window coordinates
		int iHeight, iWidth;			   ///< main window size
	//	bool bAutoScroll;				    ///< auto scroll log
		bool bAlwaysOnTop;                 ///< window always on top setting
		bool bWindowMaximized;             ///< was window maximized when app was closing?
		AnsiString asInputDeviceDllName;   ///< name of previously selected device interface dll
		AnsiString asPlotPictureExportDir;	///< default directory to save exported plot picture bitmap
		AnsiString asPlotDataExportDir;		///< default directory to save exported plot as JSON file
		AnsiString asLanguage;
	} frmMain;

	struct S_LOGGING
	{
		unsigned int iMaxUiLogLines;	   ///< maximum no. of lines cached with GUI
		bool bLogToFile;					///< write log to file
	} Logging;

	struct S_GRAPH
	{
		unsigned int iDisplayRefreshFreq;	///< display refresh frequency (if applicable)
		bool bTraceAntialiasing;
		bool bSmoothTraceMoving;
		int ToolboxTransparency;
		float fPlotLineWidth;		
	} Graph;

	struct S_GRAPH_VISIBILITY
	{
		bool bGridX;
		bool bGridY;
		bool bGridLabelsX;
		bool bGridLabelsY;
		bool bCursor;
		bool bGndMarker;
		bool bTriggerPosMarker;
		bool bTriggerValMarker;
		bool bRangeValues;
		bool bCursorValues;
	} GraphVisibility;
	
	struct S_GRAPH_COLORS
	{
		std::vector<unsigned int> traces;
    } GraphColors;

	// "Measurements" module settings
	struct S_MEASUREMENTS
	{
		bool bVisible;
		int iPosX, iPosY;
	} Measurements;

	struct S_MS3RECORDING
	{
		unsigned int iSizeLimit;		///< recording file size limit [kB]
	} Ms3Recording;

	struct S_FFT
	{
        enum { SAMPLES_HARD_LIMIT = 131072 };
		unsigned int iSamplesLimit;	///< limit of samples used to calculate FFT (will be rounded up to next power of 2)
		AnsiString asWindowName;
	} FFT;

	struct S_ZOOM
	{
		float fZoomX;				///< note: only for oscilloscope mode
		std::vector<float> fZoomY;	///< note: only for oscilloscope mode
		float GetY(int channel);
		void SetY(int channel, float val);

		bool bKeepRound;
	} Zoom;

	struct S_PLOT_POSITION
	{
		std::vector<float> fOffsetY;
		float GetY(int channel);
		void SetY(int channel, float val);
	} PlotPosition;

	static const float fRecorderMinInterval;
	static const float fRecorderMaxInterval;
	struct S_RECORDER
	{
		float fInterval;
	} Recorder;

	/** Most Recently Used */
	enum { MRU_LIMIT = 20 };

	std::deque<AnsiString> MRUjson;
	void AddMruJson(AnsiString item);
	void ClearMruJsonItems(void);

	std::deque<AnsiString> MRUms3;
	void AddMruMs3(AnsiString item);
	void ClearMruMs3Items(void);

	std::list<HotKeyConf> hotKeyConf;
};

extern Settings appSettings;


#endif
