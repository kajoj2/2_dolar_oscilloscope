/** \file
*/

#ifndef COMMAND_H
#define COMMAND_H

/** \brief Passed back type of action to execute
*/
enum E_ACTION_REQUEST
{
	REQUEST_NONE = 0,
	REQUEST_IDENTIFY
};

struct S_ARMSCOPE_SETTINGS;

/** \brief Execute single command
	\return Action type to perform by caller
*/
enum E_ACTION_REQUEST ExecuteCmd(unsigned char *dataIn, int length, struct S_ARMSCOPE_SETTINGS * const settings);

#endif // COMMAND_H
