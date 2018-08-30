#include "command.h"
#include "armscope_cmd.h"
#include "armscope_settings.h"
#include "buzzer.h"
#include "hw_config.h"
#include "delay.h"


enum E_ACTION_REQUEST ExecuteCmd(unsigned char *dataIn, int length,
	struct S_ARMSCOPE_SETTINGS * const settings)
{
	if (dataIn[0] == CMD_FUNCTION)	//special functions
	{
		switch (dataIn[1])
		{
		case CMD_FUNCTION_IDENTIFY:		//identify request
			//beep();
			return REQUEST_IDENTIFY;
			break;
#if 0
		case CMD_FUNCTION_COUPLING_DC:		//DC
			beep();
			settings->coupling_type = ARMSCOPE_COUPLING_DC;
			setACDC();
			break;
		case CMD_FUNCTION_COUPLING_AC:		//AC
			beep();
			settings->coupling_type = ARMSCOPE_COUPLING_AC;
			clearACDC();
			break;
#endif
		}
	}
#if 0
	if (dataIn[0] == CMD_SET_SENSITIVITY)	//zmiana czulosci
	{
		beep();
		switch (dataIn[1])
		{
		case 0x01:		// highest sensitivity
			clearX10();	// no 1:10 relay
			clearG1();
			clearG2();
			setG3();
			break;
		case 0x02:
			clearX10();
			clearG1();
			setG2();
			clearG3();
			break;
		case 0x03:
			clearX10();
			setG1();
			clearG2();
			clearG3();
			break;
		case 0x04:
			clearX10();
			clearG1();
			clearG2();
			clearG3();
			break;
		case 0x05:
			setX10();
			setG1();
			clearG2();
			clearG3();
			break;
		case 0x06:
			setX10();
			clearG1();
			clearG2();
			clearG3();
			break;
		}
		if (dataIn[1] >= 0x01 && dataIn[1] <= 0x06)
			settings->sensitivity = dataIn[1];
		// give some time for ADC front-end
		Delay1k(500);
	}
#endif
	return REQUEST_NONE;
}
