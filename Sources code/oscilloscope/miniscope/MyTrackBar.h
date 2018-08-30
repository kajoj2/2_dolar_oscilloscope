/** \file
    \brief Extended trackbar type with OnMouseUp event
*/

#ifndef MYTRACKBAR_H_INCLUDED
#define MYTRACKBAR_H_INCLUDED

/** \brief Extended trackbar with OnMouseUp event
	\note Usage: Put "normal" TTrackbar on form,
        ((TMyTrackBar*)TrackBar1)->OnMouseUp = TrackBarMouseUp;
		void __fastcall TForm1::TrackBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
*/
class TMyTrackBar : public TTrackBar
{
__published:
    /** \brief Additional published event function
    */
    __property OnMouseUp;
};

#endif // MYTRACKBAR_H_INCLUDED
