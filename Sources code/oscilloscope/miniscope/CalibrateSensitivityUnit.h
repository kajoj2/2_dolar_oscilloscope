/** \file
*/

#ifndef CalibrateSensitivityUnitH
#define CalibrateSensitivityUnitH

#include "CommonUnit.h"

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TfrmCalibrateSensitivity : public TfrmCommon
{
__published:	// IDE-managed Components
	TLabel *lblTarget;
	TLabel *lblVoltage;
	TEdit *edVoltage;
	TLabel *lblCapture;
	TLabel *lblCursors;
	TImage *imgCursor;
	TLabel *lblCursors2;
	TLabel *lblCalibrate;
	TButton *btnCalibrate;
	TLabel *lblOffset;
	TEdit *edOffset;
	TLabel *lblGain;
	TEdit *edGain;
	TButton *btnStoreCalibration;
	TBevel *Bevel1;
	TLabel *lblNote;
	void __fastcall btnCalibrateClick(TObject *Sender);
	void __fastcall btnStoreCalibrationClick(TObject *Sender);
private:	// User declarations
	/** \brief Request calibration calculation
		\param fVoltageHigh real voltage at left cursor
		\param offset returned 0V offset
		\param gain returned real sensitivity/declared sensitivity coeff
		\return 0 on success
	*/
	typedef int (__closure *CallbackCalculate)(float fVoltageHigh,
		int* const offset, float* const gain);
	/** \brief Store calibration data to device
	*/
	typedef int (__closure *CallbackStore)(const int offset, const float gain);
public:		// User declarations
	__fastcall TfrmCalibrateSensitivity(TComponent* Owner);
	CallbackCalculate callbackCalculate;
	CallbackStore callbackStore;
	void SetCurrentCalibration(const int offset, const float gain);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCalibrateSensitivity *frmCalibrateSensitivity;
//---------------------------------------------------------------------------
#endif
