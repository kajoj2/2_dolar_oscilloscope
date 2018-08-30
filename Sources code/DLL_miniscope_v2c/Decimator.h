/** \file
*/

#ifndef DecimatorH
#define DecimatorH
//---------------------------------------------------------------------------

class Decimator
{
public:
	Decimator();
	/** \brief Clear all remaining data buffered by filter
	*/
	void Reset(void);
	/** \brief Set decimation factor
		\return 0 on success
	*/
	int SetFactor(unsigned int val);
	/** \param input
		\param input_len number of input samples, must be > 0
		\param output ptr to output buffer, must handle all input samples
		\return Number of samples written to output, < 0 on error
	*/
	int Process(const short* input, unsigned int input_len, short* const output);
private:
	enum { E_MAX_FACTOR = 1024 };
	unsigned int factor;
	/** \brief Buffer used to collect samples before decimation.
	*/
	short buffer[E_MAX_FACTOR];
	unsigned int buffer_filled;
};

#endif
