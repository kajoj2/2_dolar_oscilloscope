/** \file Settings.cpp
*/

/* Copyright (C) 2008-2009 Tomasz Ostrowski <tomasz.o.ostrowski at gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.

 * Miniscope is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma hdrstop

#include "Settings.h"
#include "display/Hsv.h"
#include "Base64.h"
#include "KeybKeys.h"
#include <memory>
#include <algorithm>
#include <fstream>
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

const float Settings::fRecorderMinInterval = 0.01f;
const float Settings::fRecorderMaxInterval = 7200.0f;

Settings::Settings(void)
{
	SetDefault();
}

void Settings::SetDefault(void)
{
	frmMain.iPosX = 30;
	frmMain.iPosY = 30;
	frmMain.iHeight = 550;
	frmMain.iWidth = 600;
	frmMain.bAlwaysOnTop = false;
	frmMain.bWindowMaximized = false;
	frmMain.asInputDeviceDllName = "";
	frmMain.asPlotPictureExportDir = "";
	frmMain.asPlotDataExportDir = "";
	frmMain.asLanguage = "en";

	Logging.iMaxUiLogLines = 1000;
	Logging.bLogToFile = true;

	Graph.iDisplayRefreshFreq = 20;
	Graph.bTraceAntialiasing = false;
	Graph.bSmoothTraceMoving = true;
	Graph.ToolboxTransparency = 255;
	Graph.fPlotLineWidth = 1.0f;

	GraphVisibility.bGridX = true;
	GraphVisibility.bGridY = true;
	GraphVisibility.bGridLabelsX = true;
	GraphVisibility.bGridLabelsY = false;
	GraphVisibility.bCursor = true;
	GraphVisibility.bGndMarker = true;
	GraphVisibility.bTriggerPosMarker = true;
	GraphVisibility.bTriggerValMarker = true;
	GraphVisibility.bRangeValues = true;
	GraphVisibility.bCursorValues = true;

	GraphColors.traces.clear();	

	Measurements.bVisible = false;
	Measurements.iPosX = 40;
	Measurements.iPosY = 40;

	Ms3Recording.iSizeLimit = 100*1024;

	FFT.iSamplesLimit = 4096;
	FFT.asWindowName = "Hanning";

	Zoom.fZoomX = 1.0f;
	Zoom.fZoomY.clear();
	Zoom.bKeepRound = false;

	PlotPosition.fOffsetY.clear();

	MRUjson.clear();
	MRUms3.clear();

	hotKeyConf.clear();	
}

int Settings::Read(AnsiString asFileName)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

    SetDefault();

	try
	{
		std::ifstream ifs(asFileName.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strConfig, root );
		if ( !parsingSuccessful )
		{
			return 2;
		}
	}
	catch(...)
	{
		return 1;
	}

	const Json::Value &frmMainJson = root["frmMain"];
	int maxX = GetSystemMetrics(SM_CXSCREEN);
	int maxY = GetSystemMetrics(SM_CYSCREEN);
	frmMain.iPosX = frmMainJson.get("PosX", frmMain.iPosX).asInt();
	frmMain.iPosY = frmMainJson.get("PosY", frmMain.iPosY).asInt();
	frmMain.iWidth = frmMainJson.get("Width", frmMain.iWidth).asInt();
	frmMain.iHeight = frmMainJson.get("Height", frmMain.iHeight).asInt();
	if (frmMain.iWidth < 250 || frmMain.iWidth > maxX + 20)
	{
		frmMain.iWidth = 250;
	}
	if (frmMain.iHeight < 200 || frmMain.iHeight > maxY + 20)
	{
		frmMain.iHeight = 200;
	}
	if (frmMain.iPosX < 0)
		frmMain.iPosX = 0;
	if (frmMain.iPosY < 0)
		frmMain.iPosY = 0;
	if (frmMain.iPosX + frmMain.iWidth > maxX)
		frmMain.iPosX = maxX - frmMain.iWidth;
	if (frmMain.iPosY + frmMain.iHeight > maxY)
		frmMain.iPosY = maxY - frmMain.iHeight;
	frmMain.bAlwaysOnTop = frmMainJson.get("AlwaysOnTop", frmMain.bAlwaysOnTop).asBool();
	frmMain.bWindowMaximized = frmMainJson.get("WindowMaximized", frmMain.bWindowMaximized).asBool();
	frmMain.asInputDeviceDllName = frmMainJson.get("InputDeviceDllName", frmMain.asInputDeviceDllName.c_str()).asString().c_str();
	frmMain.asPlotPictureExportDir = frmMainJson.get("PlotPictureExportDir", frmMain.asPlotPictureExportDir.c_str()).asString().c_str();
	frmMain.asPlotDataExportDir = frmMainJson.get("PlotDataExportDir", frmMain.asPlotDataExportDir.c_str()).asString().c_str();
	frmMain.asLanguage = frmMainJson.get("Language", frmMain.asLanguage.c_str()).asString().c_str();

	const Json::Value &loggingJson = root["Logging"];
	Logging.iMaxUiLogLines = loggingJson.get("MaxUiLogLines", Logging.iMaxUiLogLines).asUInt();
	Logging.bLogToFile = loggingJson.get("LogToFile", Logging.bLogToFile).asBool();

	const Json::Value &graphJson = root["Graph"];
	Graph.iDisplayRefreshFreq = graphJson.get("DisplayRefreshFreq", Graph.iDisplayRefreshFreq).asUInt();
	Graph.bTraceAntialiasing = graphJson.get("TraceAntialiasing", Graph.bTraceAntialiasing).asBool();
	Graph.bSmoothTraceMoving = graphJson.get("SmoothTraceMoving", Graph.bSmoothTraceMoving).asBool();
	Graph.ToolboxTransparency = graphJson.get("ToolboxTransparency", Graph.ToolboxTransparency).asInt();
	if (Graph.ToolboxTransparency < 30 || Graph.ToolboxTransparency > 255)
		Graph.ToolboxTransparency = 255;
	Graph.fPlotLineWidth = graphJson.get("PlotLineWidth", Graph.fPlotLineWidth).asDouble();

	const Json::Value &graphVisibilityJson = root["GraphVisibility"];
	GraphVisibility.bGridX = graphVisibilityJson.get("GridX", GraphVisibility.bGridX).asBool();
	GraphVisibility.bGridY = graphVisibilityJson.get("GridY", GraphVisibility.bGridY).asBool();
	GraphVisibility.bGridLabelsX = graphVisibilityJson.get("GridLabelsX", GraphVisibility.bGridLabelsX).asBool();
	GraphVisibility.bGridLabelsY = graphVisibilityJson.get("GridLabelsY", GraphVisibility.bGridLabelsY).asBool();
	GraphVisibility.bCursor = graphVisibilityJson.get("Cursor", GraphVisibility.bCursor).asBool();
	GraphVisibility.bGndMarker = graphVisibilityJson.get("GndMarker", GraphVisibility.bGndMarker).asBool();
	GraphVisibility.bTriggerPosMarker = graphVisibilityJson.get("TriggerPosMarker", GraphVisibility.bTriggerPosMarker).asBool();
	GraphVisibility.bTriggerValMarker = graphVisibilityJson.get("TriggerValMarker", GraphVisibility.bTriggerValMarker).asBool();
	GraphVisibility.bRangeValues = graphVisibilityJson.get("RangeValues", GraphVisibility.bRangeValues).asBool();
	GraphVisibility.bCursorValues = graphVisibilityJson.get("CursorValues", GraphVisibility.bCursorValues).asBool();

	const Json::Value &graphColorsTracesJson = root["GraphColors"]["Traces"];
	if(graphColorsTracesJson.type() == Json::arrayValue)
	{
		for (unsigned int i=0; i<graphColorsTracesJson.size(); i++)
		{
			unsigned int val = graphColorsTracesJson[i].asUInt();
            GraphColors.traces.push_back(val);
		}
	}

	const Json::Value &measurementsJson = root["Measurements"];
	Measurements.bVisible = measurementsJson.get("Visible", Measurements.bVisible).asBool();
	Measurements.iPosX = measurementsJson.get("PosX", Measurements.iPosX).asInt();
	Measurements.iPosY = measurementsJson.get("PosY", Measurements.iPosY).asInt();
	if (Measurements.iPosX < 0)
		Measurements.iPosX = 0;
	if (Measurements.iPosY < 0)
		Measurements.iPosY = 0;
	if (Measurements.iPosX + 40 > maxX)
		Measurements.iPosX = maxX - 40;
	if (Measurements.iPosY + 40 > maxY)
		Measurements.iPosY = maxY - 40;

	const Json::Value &ms3RecordingJson = root["Ms3Recording"];
	Ms3Recording.iSizeLimit = ms3RecordingJson.get("SizeLimit", Ms3Recording.iSizeLimit).asUInt();
	if (Ms3Recording.iSizeLimit < 5*1024 || Ms3Recording.iSizeLimit > 2000*1024)
	{
		Ms3Recording.iSizeLimit = 100*1024;
	}

	const Json::Value &FFTJson = root["FFT"];
	FFT.iSamplesLimit = FFTJson.get("SamplesLimit", FFT.iSamplesLimit).asInt();
	if (FFT.iSamplesLimit < 512 || FFT.iSamplesLimit > S_FFT::SAMPLES_HARD_LIMIT)
		FFT.iSamplesLimit = 4096;
	FFT.asWindowName = FFTJson.get("WindowName", FFT.asWindowName.c_str()).asString().c_str();

	const Json::Value &zoomJson = root["Zoom"];
	Zoom.fZoomX = zoomJson.get("ZoomX", Zoom.fZoomX).asDouble();
	Zoom.bKeepRound = zoomJson.get("KeepRound", Zoom.bKeepRound).asBool();
	const Json::Value &zoomYJson = zoomJson["ZoomY"];
	if(zoomYJson.type() == Json::arrayValue)
	{
		for (unsigned int i=0; i<zoomYJson.size(); i++)
		{
			float val = zoomYJson[i].asDouble();
			Zoom.fZoomY.push_back(val);
		}
	}

	const Json::Value &plotPositionJson = root["PlotPosition"];
	if (plotPositionJson.type() == Json::arrayValue)
	{
		for (unsigned int i=0; i<plotPositionJson.size(); i++)
		{
			float val = plotPositionJson[i].asDouble();
			PlotPosition.fOffsetY.push_back(val);
		}
	}

	const Json::Value &MRUjsonJson = root["MRUjson"];
	if (MRUjsonJson.type() == Json::arrayValue)
	{
		for (unsigned int i=0; i<MRUjsonJson.size(); i++)
		{
			std::string val = MRUjsonJson[i].asString();
			MRUjson.push_back(val.c_str());
		}
	}

	const Json::Value &MRUms3Json = root["MRUms3"];
	if (MRUms3Json.type() == Json::arrayValue)
	{
		for (unsigned int i=0; i<MRUms3Json.size(); i++)
		{
			std::string val = MRUms3Json[i].asString();
			MRUms3.push_back(val.c_str());
		}
	}

	const Json::Value &hotkeyConfJson = root["hotkeyConf"];
	for (unsigned int i=0; i<hotkeyConfJson.size(); i++)
	{
		const Json::Value &hotkeyJson = hotkeyConfJson[i];
		class HotKeyConf cfg;
		cfg.keyCode = hotkeyJson.get("keyCode", "").asString().c_str();
		int id = vkey_find(cfg.keyCode.c_str());
		if (id >= 0)
		{
        	cfg.vkCode = vkey_list[id].vkey;
		}
		else
		{
        	cfg.vkCode = -1;
		}
		cfg.modifiers = hotkeyJson.get("modifiers", 0).asInt();
		cfg.global = hotkeyJson.get("global", false).asBool();
		const Json::Value &action = hotkeyJson["action"];
		cfg.action.type = static_cast<Action::Type>(action.get("type", 0).asInt());
		cfg.action.id = action.get("id", 0).asInt();

		hotKeyConf.push_back(cfg); 
	}	

	return 0;
}

int Settings::Write(AnsiString asFileName)
{
    Json::Value root;
	Json::StyledWriter writer;

	{
		Json::Value &jv = root["frmMain"];
		jv["PosX"] = frmMain.iPosX;
		jv["PosY"] = frmMain.iPosY;
		jv["Width"] = frmMain.iWidth;
		jv["Height"] = frmMain.iHeight;
		jv["AlwaysOnTop"] = frmMain.bAlwaysOnTop;
		jv["WindowMaximized"] = frmMain.bWindowMaximized;
		jv["InputDeviceDllName"] = frmMain.asInputDeviceDllName.c_str();
		jv["PlotPictureExportDir"] = frmMain.asPlotPictureExportDir.c_str();
		jv["PlotDataExportDir"] = frmMain.asPlotDataExportDir.c_str();
		jv["Language"] = frmMain.asLanguage.c_str();
	}
	{
		Json::Value &jv = root["Logging"];
		jv["MaxUiLogLines"] = Logging.iMaxUiLogLines;
		jv["LogToFile"] = Logging.bLogToFile;
	}
	{
		Json::Value &jv = root["Graph"];
		jv["DisplayRefreshFreq"] = Graph.iDisplayRefreshFreq;
		jv["TraceAntialiasing"] = Graph.bTraceAntialiasing;
		jv["SmoothTraceMoving"] = Graph.bSmoothTraceMoving;
		jv["ToolboxTransparency"] = Graph.ToolboxTransparency;
		jv["PlotLineWidth"] = Graph.fPlotLineWidth;
	}
	{
		Json::Value &jv = root["GraphVisibility"];
		jv["GridX"] = GraphVisibility.bGridX;
		jv["GridY"] = GraphVisibility.bGridY;
		jv["GridLabelsX"] = GraphVisibility.bGridLabelsX;
		jv["GridLabelsY"] = GraphVisibility.bGridLabelsY;
		jv["Cursor"] = GraphVisibility.bCursor;
		jv["GndMarker"] = GraphVisibility.bGndMarker;
		jv["TriggerPosMarker"] = GraphVisibility.bTriggerPosMarker;
		jv["TriggerValMarker"] = GraphVisibility.bTriggerValMarker;
		jv["RangeValues"] = GraphVisibility.bRangeValues;
		jv["CursorValues"] = GraphVisibility.bCursorValues;
	}
	{
		Json::Value &jv = root["GraphColors"]["Traces"];
		jv.resize(0);
		for (unsigned int i=0; i<GraphColors.traces.size(); i++)
		{
			jv[i] = GraphColors.traces[i];
		}
	}
	{
		Json::Value &jv = root["Measurements"];
		jv["Visible"] = Measurements.bVisible;
		jv["PosX"] = Measurements.iPosX;
		jv["PosY"] = Measurements.iPosY;
	}
	{
		Json::Value &jv = root["Ms3Recording"];
		jv["SizeLimit"] = Ms3Recording.iSizeLimit;
	}
	{
		Json::Value &jv = root["FFT"];
		jv["SamplesLimit"] = FFT.iSamplesLimit;
		jv["WindowName"] = FFT.asWindowName.c_str();
	}
	{
		Json::Value &jv = root["Zoom"];
		jv["ZoomX"] = Zoom.fZoomX;
		jv["KeepRound"] = Zoom.bKeepRound;
		jv["ZoomY"].resize(0);
		for (unsigned int i=0; i<Zoom.fZoomY.size(); i++)
		{
			jv["ZoomY"][i] = Zoom.fZoomY[i];
		}
	}
	{
		Json::Value &jv = root["PlotPosition"];
		jv.resize(0);
		for (unsigned int i=0; i<PlotPosition.fOffsetY.size(); i++)
		{
			jv[i] = PlotPosition.fOffsetY[i];
		}
	}
	{
		Json::Value &jv = root["MRUjson"];
		jv.resize(0);
		for (unsigned int i=0; i<MRUjson.size(); i++)
		{
			jv[i] = MRUjson[i].c_str();
		}
	}
	{
		Json::Value &jv = root["MRUms3"];
		jv.resize(0);		
		for (unsigned int i=0; i<MRUms3.size(); i++)
		{
			jv[i] = MRUms3[i].c_str();
		}
	}
	{
		int i = 0;
		std::list<HotKeyConf>::iterator iter;
		Json::Value &jv = root["hotkeyConf"];
		jv.resize(0);		
		for (iter = hotKeyConf.begin(); iter != hotKeyConf.end(); ++iter)
		{
			struct HotKeyConf &cfg = *iter;
			Json::Value &cfgJson = jv[i++];
			cfgJson["keyCode"] = cfg.keyCode.c_str();
			cfgJson["modifiers"] = cfg.modifiers;
			cfgJson["global"] = cfg.global;
			cfgJson["action"]["type"] = cfg.action.type;
			cfgJson["action"]["id"] = cfg.action.id;
		}
	}
	


	std::string outputConfig = writer.write( root );

	try
	{
		std::ofstream ofs(asFileName.c_str());
		ofs << outputConfig;
		ofs.close();
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}

void Settings::AddMruJson(AnsiString item)
{
	// check if item is already in MRU list
	std::deque<AnsiString>::iterator iter;
	for (iter=MRUjson.begin(); iter != MRUjson.end(); )
	{
		if (*iter == item)
		{
			iter = MRUjson.erase(iter);
		}
		else
		{
            ++iter;
        }
	}
	// push item on top
	MRUjson.push_back(item);
	while (MRUjson.size() > MRU_LIMIT)
	{
		MRUjson.pop_front();
    }
}

void Settings::ClearMruJsonItems(void)
{
	MRUjson.clear();
}

void Settings::AddMruMs3(AnsiString item)
{
	// check if item is already in MRU list
	std::deque<AnsiString>::iterator iter;
	for (iter=MRUms3.begin(); iter != MRUms3.end(); )
	{
		if (*iter == item)
		{
			iter = MRUms3.erase(iter);
		}
		else
		{
            ++iter;
        }
	}
	// push item on top
	MRUms3.push_back(item);
	while (MRUms3.size() > MRU_LIMIT)
	{
		MRUms3.pop_front();
    }
}

void Settings::ClearMruMs3Items(void)
{
	MRUms3.clear();
}

float Settings::S_ZOOM::GetY(int channel)
{
	if (channel < (int)fZoomY.size())
	{
		return fZoomY[channel];
	}
	else
	{
    	return 1.0f;
    }
}

void Settings::S_ZOOM::SetY(int channel, float val)
{
	for (int i=fZoomY.size(); i<=channel; i++)
	{
		fZoomY.push_back(1.0f);
	}
	fZoomY[channel] = val;
}

float Settings::S_PLOT_POSITION::GetY(int channel)
{
	if (channel < (int)fOffsetY.size())
	{
		return fOffsetY[channel];
	}
	else
	{
		return 0.0f;
	}
}

void Settings::S_PLOT_POSITION::SetY(int channel, float val)
{
	for (int i=fOffsetY.size(); i<=channel; i++)
	{
		fOffsetY.push_back(0.0f);
	}
	fOffsetY[channel] = val;
}
