//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Settings.h"
//---------------------------------------------------------------------------
USEFORM("Main.cpp", frmMain);
USEFORM("AboutUnit.cpp", AboutBox);
USEFORM("SettingsUnit.cpp", frmSettings);
USEFORM("LogUnit.cpp", frmLog);
USEFORM("MeasurementsUnit.cpp", frmMeasurements);
USEFORM("CalibrateSensitivityUnit.cpp", frmCalibrateSensitivity);
USEFORM("PlotToolbarUnit.cpp", frmPlotToolbar);
USEFORM("WaitUnit.cpp", frmWait);
USEFORM("FilterSelector.cpp", frmFilterSelector);
USEFORM("JsonViewerUnit.cpp", frmJsonViewer);
USEFORM("Ms3ViewerUnit.cpp", frmMs3Viewer);
USEFORM("Ms3FrameListUnit.cpp", frmMs3FrameList);
USEFORM("NotificationUnit.cpp", frmNotification);
USEFORM("Ms3FileRepairUnit.cpp", frmMs3FileRepair);
USEFORM("ControlUnit.cpp", frmControl);
USEFORM("StatisticsUnit.cpp", frmStatistics);
USEFORM("FormHotkeys.cpp", frmHotkeys);
//---------------------------------------------------------------------------
#pragma link "jsoncpp.lib"

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();

		AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".json" );
		appSettings.Read(asConfigFile);

		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TfrmSettings), &frmSettings);
		Application->CreateForm(__classid(TfrmLog), &frmLog);
		Application->CreateForm(__classid(TfrmMeasurements), &frmMeasurements);
		Application->CreateForm(__classid(TfrmCalibrateSensitivity), &frmCalibrateSensitivity);
		Application->CreateForm(__classid(TfrmPlotToolbar), &frmPlotToolbar);
		Application->CreateForm(__classid(TfrmWait), &frmWait);
		Application->CreateForm(__classid(TfrmFilterSelector), &frmFilterSelector);
		Application->CreateForm(__classid(TfrmJsonViewer), &frmJsonViewer);
		Application->CreateForm(__classid(TfrmMs3Viewer), &frmMs3Viewer);
		Application->CreateForm(__classid(TfrmMs3FrameList), &frmMs3FrameList);
		Application->CreateForm(__classid(TfrmNotification), &frmNotification);
		Application->CreateForm(__classid(TfrmMs3FileRepair), &frmMs3FileRepair);
		Application->CreateForm(__classid(TfrmControl), &frmControl);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
