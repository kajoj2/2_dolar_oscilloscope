//---------------------------------------------------------------------------

#ifndef FilterSelectorH
#define FilterSelectorH

#include "CommonUnit.h"

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include <vector>
#include <string>

class TfrmFilterSelector : public TfrmCommon
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnApply;
	TButton *btnCancel;
	TGroupBox *grBoxAvailableFilters;
	TGroupBox *grboxUsedFilters;
	TButton *btnAdd;
	TPanel *pnlUsedFiltersBottom;
	TButton *btnRemove;
	TButton *btnUp;
	TButton *btnDown;
	TListView *lvFilterChain;
	TListView *lvAvailableFilters;
	TLabel *lblNote;
	TLabel *lblNote2;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall lvAvailableFiltersData(TObject *Sender, TListItem *Item);
	void __fastcall btnAddClick(TObject *Sender);
	void __fastcall lvFilterChainData(TObject *Sender, TListItem *Item);
	void __fastcall btnRemoveClick(TObject *Sender);
	void __fastcall btnUpClick(TObject *Sender);
	void __fastcall btnDownClick(TObject *Sender);
	void __fastcall lvAvailableFiltersDblClick(TObject *Sender);
private:	// User declarations
	std::vector<std::string> filterNames;
	std::vector<std::string> filterSelNames;
	void UpdateFilterNamesView(void);
	void AddFilter(void);
public:		// User declarations
	__fastcall TfrmFilterSelector(TComponent* Owner);
	const std::vector<std::string>& Get(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmFilterSelector *frmFilterSelector;
//---------------------------------------------------------------------------
#endif
