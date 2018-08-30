//---------------------------------------------------------------------------

#ifndef RoundingH
#define RoundingH
//---------------------------------------------------------------------------

/** \brief Get next "round" (1:2:5 scheme) value higher than argument
*/
float GetNextRoundHigherValue(float val);

/** \brief Get next "round" (1:2:5 scheme) value lower than argument
*/
float GetNextRoundLowerValue(float val);

bool IsRoundingNeeded(float val);

#endif
