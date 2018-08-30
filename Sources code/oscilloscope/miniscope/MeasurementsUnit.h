//---------------------------------------------------------------------------

#ifndef MeasurementsUnitH
#define MeasurementsUnitH
//---------------------------------------------------------------------------
#include "CommonUnit.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "Measurements.h"
#include <vector>

class Data;

//---------------------------------------------------------------------------
class TfrmMeasurements : public TfrmCommon
{
__published:	// IDE-managed Components
	TMemo *memo;
private:	// User declarations
	Measurements measurements;
public:		// User declarations
	__fastcall TfrmMeasurements(TComponent* Owner);
	void Process(const Data &data, int channel);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMeasurements *frmMeasurements;
//---------------------------------------------------------------------------
#endif
