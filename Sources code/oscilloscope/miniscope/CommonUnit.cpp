//---------------------------------------------------------------------------


#pragma hdrstop

#include "CommonUnit.h"
#include "Settings.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace {
	bool once = false;
}

__fastcall TfrmCommon::TfrmCommon(TComponent* AOwner)
	: TForm(AOwner)
{
#if 0
	if (once == false)
	{
		once = true;
		TP_GlobalIgnoreClassProperty(__classid(TAction), "Category");
		TP_GlobalIgnoreClassProperty(__classid(TControl), "ImeName");
		TP_GlobalIgnoreClassProperty(__classid(TControl), "HelpKeyword");
		//TP_GlobalIgnoreClass(__classid(TMonthCalendar));
		TP_GlobalIgnoreClass(__classid(TFont));
		//TP_GlobalIgnoreClass(__classid(TStatusBar));
		// TP_GlobalIgnoreClass(TWebBrowser);
		// TP_GlobalIgnoreClass(TNoteBook);
		// TP_GlobalHandleClass(TCustomTreeView,HandleTreeView);
		// TP_GlobalHandleClass(TKWizardCustomPage,HandleWizardPage);
		UseLanguage(appSettings.asLanguage);
	}

	TranslateComponent(this);
#endif
}

