//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Ms3FileRepairUnit.h"
#include "Ms3Recorder.h"
#include "BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMs3FileRepair *frmMs3FileRepair;
//---------------------------------------------------------------------------
__fastcall TfrmMs3FileRepair::TfrmMs3FileRepair(TComponent* Owner)
	: TfrmCommon(Owner)
{
	fix_status.progress = 0;
	ProgressBar->Position = fix_status.progress;
}
//---------------------------------------------------------------------------

AnsiString TfrmMs3FileRepair::GetStatusDescription(enum TfrmMs3FileRepair::E_STATUS status)
{
	AnsiString msg;
	switch (status)
	{
	case	E_FIXED:
		msg = _("Fixed file was created");
		break;
	case	E_ERRORFIXING:
		msg = _("Error occured during creting fixed file");
		break;
	case	E_ABORTED:
		msg = _("Fixed file not created - user abort");
		break;
	case	E_NOTHINGTODO:
		msg = _("File is not damaged - nothing to do");
		break;
	case	E_CANTFIX:
		msg = _("Can not fix file");
		break;
	default:
		{
			AnsiString fmt = _("Error %d when trying to fix file. No detailed description.");
			msg.sprintf(fmt.c_str(), status);
		}
		break;
	}
	return msg;
}

__fastcall TfrmMs3FileRepair::ShowModal(void)
{
	return TForm::ShowModal();
}

void TfrmMs3FileRepair::Execute(AnsiString filename, enum TfrmMs3FileRepair::E_STATUS &status)
{
	status = E_ABORTED;
	fix_status.progress = 0;
	enum Ms3Recorder::ERR err = Ms3Recorder::VerifyFile(filename);
	if (err == Ms3Recorder::ERR_NONE)
	{
		status = E_NOTHINGTODO;
		return;
	}
	if (err == Ms3Recorder::ERR_MISSING_FILE_FOOTER)
	{
		lblErrorTypeVal->Caption = _("Missing file index and/or footer");
		fix_status.status = &status;
		fix_status.filename = filename;
		ShowModal();
	}
	else
	{
		LOG(E_LOG_TRACE, "MS3 repair: can't fix file - file status = %d", err);
		status = E_CANTFIX;
	}
}

void __fastcall TfrmMs3FileRepair::btnCloseClick(TObject *Sender)
{
	this->Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMs3FileRepair::btnCreateFixedFileClick(TObject *Sender)
{
	// lock buttons until done
	BtnController btnCtrlClose(btnClose);
	BtnController btnCtrlCreateFixed(btnCreateFixedFile);

	fix_status.filename_output = fix_status.filename;
	fix_status.filename_output.Insert("_fixed", fix_status.filename_output.Length() - ExtractFileExt(fix_status.filename_output).Length() + 1);
	SaveDialog->Title = _("Select name of fixed file to create");
	SaveDialog->DefaultExt = "ms3";
	SaveDialog->Filter = _("MS3 files (*.ms3)|*.ms3");
	SaveDialog->FilterIndex = 1;
	SaveDialog->InitialDir = ExtractFileDir(fix_status.filename_output);
	SaveDialog->FileName = fix_status.filename_output;
	if (SaveDialog->Execute()) {
		if (FileExists(SaveDialog->FileName))
		{
			AnsiString msg = (AnsiString)_("File already exists, file will be overwritten.") +
					"\r\n" + _("Are you sure you want to continue?");
			if (MessageBox(this->Handle, msg.c_str(),
				this->Caption.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
			{
				return;
			}
		}
		DWORD dwtid;
		*fix_status.status = E_IN_PROGRESS;
		HANDLE Thread = CreateThread(NULL, 0, ThreadRepairProc, &this->fix_status, 0, &dwtid);
		if (!Thread)
		{
			*fix_status.status = E_ERRORFIXING;
			LOG(E_LOG_TRACE, "Ms3 file repair: failed to create thread");
			return;
		}

		while (*fix_status.status == E_IN_PROGRESS)
		{
			ProgressBar->Position = fix_status.progress;
			ProgressBar->Update();
			Application->ProcessMessages();
			Sleep(25);
		}
		Close();
	}
}
//---------------------------------------------------------------------------

DWORD WINAPI ThreadRepairProc(LPVOID data)
{
	TfrmMs3FileRepair::S_FIX_STATUS *status = (TfrmMs3FileRepair::S_FIX_STATUS*)data;
	Ms3Recorder::ERR err =
		Ms3Recorder::FixMissingFileFooter(status->filename,
			status->filename_output, frmMs3FileRepair->fix_status.progress);
	if (err)
		*status->status = TfrmMs3FileRepair::E_ERRORFIXING;
	else
		*status->status = TfrmMs3FileRepair::E_FIXED;
	return 0;
}


void __fastcall TfrmMs3FileRepair::FormShow(TObject *Sender)
{
	fix_status.progress = 0;
	ProgressBar->Position = fix_status.progress;	
}
//---------------------------------------------------------------------------

