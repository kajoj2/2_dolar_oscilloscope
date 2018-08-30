//---------------------------------------------------------------------------

#pragma hdrstop

#include "Decimator.h"
#include "Mutex.h"
#include "ScopedLock.h"
#include <string.h>
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace
{
	static Mutex mutex;
};

Decimator::Decimator():
	factor(1),
	buffer_filled(0)
{
}

void Decimator::Reset(void)
{
	buffer_filled = 0;
}

int Decimator::SetFactor(unsigned int val)
{
	ScopedLock<Mutex> lock(mutex);
	if (factor < 1 || factor > E_MAX_FACTOR)
		return -1;
	factor = val;
	Reset();
	return 0;
}

/** \note Plain simple moving average used as decimation filter.
*/
int Decimator::Process(const short* input, unsigned int input_len, short* const output)
{
	ScopedLock<Mutex> lock(mutex);

	int output_len = 0;
	assert(input_len > 0);
	assert(buffer_filled < factor);
		
	if (factor == 1)
	{
		memcpy(output, input, input_len * sizeof(*input));
		return input_len;
	}
	else if (factor == 2)
	{
		if (buffer_filled == 1)
		{
			output[output_len++] = (buffer[0] + input[0])/2;
			input_len--;
			input++;
		}
		
		while (input_len >= factor)
		{
			output[output_len++] = ((*input) + (*(input+1)))/2;
			input += 2;
			input_len -= 2;
		}

		if (input_len)
		{
			buffer[0] = *input;
			buffer_filled = 1;
		}
	}
	else
	{
		if (buffer_filled + input_len >= factor)
		{
			int sum = 0;
			unsigned int i;
			for (i=0; i<buffer_filled; i++)
			{
				sum += buffer[i];
			}
			for (i; i<factor; i++)
			{
				sum += *input;
				input_len--;
				input++;
			}
			output[output_len++] = sum/factor;
		}
		while (input_len >= factor)
		{
			int sum = 0;	// integer - do not allow overflow!
			for (unsigned int i=0; i<factor; i++)
			{
				sum += *input;
				input++;
			}
			output[output_len++] = sum/factor;
			input_len -= factor;
		}
		int remained = 0;
		buffer_filled = 0;
		while (input_len > 0)
		{
			buffer[remained++] = *input;
			input++;
			input_len--;
			buffer_filled++;
		}
	}

	return output_len;
}

