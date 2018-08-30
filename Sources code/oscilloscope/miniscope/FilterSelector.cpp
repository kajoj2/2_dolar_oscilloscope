//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FilterSelector.h"
#include "filter/DataFilterFactory.h"
#include "filter/DataFilterChain.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmFilterSelector *frmFilterSelector;
//---------------------------------------------------------------------------
__fastcall TfrmFilterSelector::TfrmFilterSelector(TComponent* Owner)
	: TfrmCommon(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmFilterSelector::FormShow(TObject *Sender)
{
	static bool once = false;
	if (!once)
	{
		once = true;
		DataFilterFactory::Instance().GetRegisteredList(filterNames);
		lvAvailableFilters->Items->Count = filterNames.size();
		lvAvailableFilters->Width--; lvAvailableFilters->Width++;
		lvAvailableFilters->Invalidate();
	}
}

void TfrmFilterSelector::UpdateFilterNamesView(void)
{
	lvFilterChain->Items->Count = filterSelNames.size();
	lvFilterChain->Width--; lvFilterChain->Width++;
	lvFilterChain->Invalidate();
}

void TfrmFilterSelector::AddFilter(void)
{
	if (!lvAvailableFilters->Selected)
		return;
	int id = lvAvailableFilters->Selected->Index;
	filterSelNames.push_back(filterNames[id]);
	UpdateFilterNamesView();
}

//---------------------------------------------------------------------------
void __fastcall TfrmFilterSelector::lvAvailableFiltersData(TObject *Sender,
      TListItem *Item)
{
	int id = Item->Index;
	Item->Caption = filterNames[id].c_str();
}
//---------------------------------------------------------------------------
void __fastcall TfrmFilterSelector::btnAddClick(TObject *Sender)
{
	AddFilter();
}
//---------------------------------------------------------------------------

void __fastcall TfrmFilterSelector::lvFilterChainData(TObject *Sender,
      TListItem *Item)
{
	int id = Item->Index;
	Item->Caption = filterSelNames[id].c_str();
}
//---------------------------------------------------------------------------

void __fastcall TfrmFilterSelector::btnRemoveClick(TObject *Sender)
{
	if (!lvFilterChain->Selected)
		return;
	int id = lvFilterChain->Selected->Index;
	filterSelNames.erase(filterSelNames.begin() + id);
	lvFilterChain->ClearSelection();
	UpdateFilterNamesView();
}
//---------------------------------------------------------------------------

void __fastcall TfrmFilterSelector::btnUpClick(TObject *Sender)
{
	if (!lvFilterChain->Selected)
		return;
	int i = lvFilterChain->Selected->Index;
	if (i == 0)
		return;
	std::string tmp = filterSelNames[i-1];
	filterSelNames[i-1] = filterSelNames[i];
	filterSelNames[i] = tmp;
	lvFilterChain->Items->Item[i-1]->Selected = true;
	UpdateFilterNamesView();
}
//---------------------------------------------------------------------------

void __fastcall TfrmFilterSelector::btnDownClick(TObject *Sender)
{
	if (!lvFilterChain->Selected)
		return;
	unsigned int i = lvFilterChain->Selected->Index;
	if (i == filterSelNames.size() - 1)
		return;
	std::string tmp = filterSelNames[i+1];
	filterSelNames[i+1] = filterSelNames[i];
	filterSelNames[i] = tmp;
	lvFilterChain->Items->Item[i+1]->Selected = true;
	UpdateFilterNamesView();
}
//---------------------------------------------------------------------------

void __fastcall TfrmFilterSelector::lvAvailableFiltersDblClick(TObject *Sender)
{
	AddFilter();	
}
//---------------------------------------------------------------------------

const std::vector<std::string>& TfrmFilterSelector::Get(void)
{
	return filterSelNames;
}

