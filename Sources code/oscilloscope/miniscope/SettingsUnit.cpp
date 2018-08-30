/** \file SettingsUnit.cpp
	\brief Common (not related to plugins) settings window.
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

#include <vcl.h>
#pragma hdrstop

#include "SettingsUnit.h"
#include "FormHotkeys.h"
#include "MiniscopeDeviceInterface.h"
#include "FFTWindowFactory.h"
#include "Utilities.h"
#include "display/Hsv.h"
#include <SysUtils.hpp>
#include <assert.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;

namespace
{
	std::vector<const FFTWindowDescription*> fft_descriptions;
}

//---------------------------------------------------------------------------
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TfrmCommon(Owner)
{
	this->appSettings = NULL;
	PageControl->ActivePage = tsScopePlugins;

	frmHotkeys = new TfrmHotkeys(tsHotkeys);
	frmHotkeys->Parent = tsHotkeys;
	frmHotkeys->Visible = true;

	FFTWindowFactory::Instance().GetRegisteredList(fft_descriptions);
	assert(fft_descriptions.size() > 0);
	for (unsigned int i=0; i<fft_descriptions.size(); i++)
	{
		AnsiString name = fft_descriptions[i]->name.c_str();
		cbFftWindowType->Items->Add(name);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
	lvScopePlugins->Items->Count = DeviceInterface::dlls.size();

	assert(appSettings);
	tmpSettings = *appSettings;
	chbAlwaysOnTop->Checked = tmpSettings.frmMain.bAlwaysOnTop;
	edMs3RecorderFileSizeLimit->Text = tmpSettings.Ms3Recording.iSizeLimit;
	chbLogToFile->Checked = tmpSettings.Logging.bLogToFile;
	cmbMaxUiLogLines->ItemIndex = -1;
	for (int i=0; i<cmbMaxUiLogLines->Items->Count; i++)
	{
		if ((unsigned int)atoi(cmbMaxUiLogLines->Items->Strings[i].c_str()) >= tmpSettings.Logging.iMaxUiLogLines)
		{
			cmbMaxUiLogLines->ItemIndex = i;
			break;
		}
	}
	if (cmbMaxUiLogLines->ItemIndex == -1)
	{
		cmbMaxUiLogLines->ItemHeight = cmbMaxUiLogLines->Items->Count - 1;
	}
	for (int i=0; i<cbGraphRefreshingFreq->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cbGraphRefreshingFreq->Items->Strings[i]) >= tmpSettings.Graph.iDisplayRefreshFreq)
		{
			cbGraphRefreshingFreq->ItemIndex = i;
			break;
		}
	}

	for (int i=0; i<cbFftSamplesLimit->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cbFftSamplesLimit->Items->Strings[i]) >= tmpSettings.FFT.iSamplesLimit)
		{
			cbFftSamplesLimit->ItemIndex = i;
			break;
		}
	}
    cbFftWindowType->ItemIndex = 0;
	for (int i=0; i<cbFftWindowType->Items->Count; i++)
	{
		if (cbFftWindowType->Items->Strings[i] == tmpSettings.FFT.asWindowName)
		{
			cbFftWindowType->ItemIndex = i;
			break;
        }
    }
	SetFftWindowDescription();

	tbarPlotToolbarTransparency->Position = 255 - tmpSettings.Graph.ToolboxTransparency;
	chbSmoothTraceMoving->Checked = tmpSettings.Graph.bSmoothTraceMoving;
	chbZoomKeepRound->Checked = tmpSettings.Zoom.bKeepRound;
	edGraphPlotLineWidth->Text = tmpSettings.Graph.fPlotLineWidth;

	lvGraphTraceColors->Items->Count = tmpSettings.GraphColors.traces.size();

	frmHotkeys->SetCfg(&tmpSettings.hotKeyConf);	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
    tmpSettings.Logging.bLogToFile = chbLogToFile->Checked;
	tmpSettings.Ms3Recording.iSizeLimit = this->edMs3RecorderFileSizeLimit->Text.ToIntDef(0);
	tmpSettings.Graph.bSmoothTraceMoving = chbSmoothTraceMoving->Checked;
	tmpSettings.Zoom.bKeepRound = chbZoomKeepRound->Checked;
	tmpSettings.Graph.fPlotLineWidth = StrToFloatDef(edGraphPlotLineWidth->Text, 1.0f);

	if (tmpSettings.Ms3Recording.iSizeLimit < 5120 || tmpSettings.Ms3Recording.iSizeLimit > 2048000)
	{
		AnsiString msg = _("MS3 recorder file size outside range (5120...2048000kB).");
		MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	if (lvScopePlugins->Selected)
	{
		tmpSettings.frmMain.asInputDeviceDllName = lvScopePlugins->Selected->Caption;
	}
	*appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAlwaysOnTopClick(TObject *Sender)
{
	tmpSettings.frmMain.bAlwaysOnTop = chbAlwaysOnTop->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cmbMaxUiLogLinesChange(TObject *Sender)
{
	tmpSettings.Logging.iMaxUiLogLines = StrToInt(cmbMaxUiLogLines->Text);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::lvScopePluginsData(TObject *Sender,
      TListItem *Item)
{
	int id = Item->Index;
	Item->Caption = ExtractFileName(DeviceInterface::dlls[id].name);
	if (Item->Caption == tmpSettings.frmMain.asInputDeviceDllName)
		Item->ImageIndex = 0;
	else
		Item->ImageIndex = -1;
	Item->SubItems->Add(DeviceInterface::dlls[id].file_version);
}
//---------------------------------------------------------------------------


void __fastcall TfrmSettings::cbGraphRefreshingFreqChange(TObject *Sender)
{
	tmpSettings.Graph.iDisplayRefreshFreq = StrToInt(cbGraphRefreshingFreq->Text);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::tbarPlotToolbarTransparencyChange(TObject *Sender)
{
	tmpSettings.Graph.ToolboxTransparency = 255 - tbarPlotToolbarTransparency->Position;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbFftSamplesLimitChange(TObject *Sender)
{
	tmpSettings.FFT.iSamplesLimit = StrToInt(cbFftSamplesLimit->Text);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbFftWindowTypeChange(TObject *Sender)
{
	tmpSettings.FFT.asWindowName = cbFftWindowType->Items->Strings[cbFftWindowType->ItemIndex];
	SetFftWindowDescription();
}
//---------------------------------------------------------------------------

void TfrmSettings::SetFftWindowDescription(void)
{
	AnsiString description = fft_descriptions[cbFftWindowType->ItemIndex]->desc.c_str();
	memoFftWindowDescription->Text = description;
}

void __fastcall TfrmSettings::btnAddGraphTraceColorClick(TObject *Sender)
{
	tmpSettings.GraphColors.traces.push_back(clWhite);
	lvGraphTraceColors->Items->Count = tmpSettings.GraphColors.traces.size();
	lvGraphTraceColors->Invalidate();
	lvGraphTraceColors->Selected = lvGraphTraceColors->Items->Item[lvGraphTraceColors->Items->Count - 1];
	EditGraphTraceColor();
}
//---------------------------------------------------------------------------

void TfrmSettings::EditGraphTraceColor(void)
{
	TListItem *item = lvGraphTraceColors->Selected;
	if (!item)
		return;
	ColorDialog->Color = static_cast<TColor>(tmpSettings.GraphColors.traces[item->Index]);
	if (ColorDialog->Execute())
	{
		tmpSettings.GraphColors.traces[item->Index] = ColorDialog->Color;
	}
	lvGraphTraceColors->Invalidate();
}

void __fastcall TfrmSettings::lvGraphTraceColorsDrawItem(
      TCustomListView *Sender, TListItem *Item, TRect &Rect,
      TOwnerDrawState State)
{
	TCanvas *pCanvas = Sender->Canvas;
	TColor prevColor = pCanvas->Brush->Color;

	pCanvas->Brush->Color = static_cast<TColor>(tmpSettings.GraphColors.traces[Item->Index]);
	TRect col1rect = Rect;
	col1rect.left += lvGraphTraceColors->Columns->Items[0]->Width;
	if (Item->Selected)
	{
		pCanvas->Rectangle(col1rect);
	}
	else
	{
		pCanvas->FillRect(col1rect);
	}

	pCanvas->Brush->Color = prevColor;
	RECT R = static_cast<RECT>(Rect);
	R.right -= lvGraphTraceColors->Columns->Items[1]->Width;
	AnsiString text(GetChannelName(Item->Index));
	DrawText(pCanvas->Handle, text.c_str(), text.Length(), &R,
			 DT_CENTER | DT_SINGLELINE);
	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnChangeGraphTraceColorClick(TObject *Sender)
{
	EditGraphTraceColor();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnRemoveGraphTraceColorClick(TObject *Sender)
{
	TListItem *item = lvGraphTraceColors->Selected;
	if (!item)
		return;
	tmpSettings.GraphColors.traces.erase(tmpSettings.GraphColors.traces.begin() + item->Index);
	lvGraphTraceColors->Items->Count = tmpSettings.GraphColors.traces.size();
	lvGraphTraceColors->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnDefaultSetGraphTraceColorClick(TObject *Sender)
{
	Hsv hsv;
	int cnt = tmpSettings.GraphColors.traces.size();
	for (int i=0; i<cnt; i++)
	{
		tmpSettings.GraphColors.traces[i] = hsv.GetUniformColor(cnt, 0, i); 
	}
	lvGraphTraceColors->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnRefreshDllListClick(TObject *Sender)
{
	DeviceInterface::ReEnumerateDlls();
	lvScopePlugins->Items->Count = DeviceInterface::dlls.size();
	lvScopePlugins->Invalidate();
}
//---------------------------------------------------------------------------

