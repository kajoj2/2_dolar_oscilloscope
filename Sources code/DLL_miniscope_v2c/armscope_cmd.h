/** \file
	\brief Set of command values shared between armscope firmware and dll.
	
	Each command consists of E_COMMAND_TYPE byte, numerical value or E_SUBCOMMAND_FUNCTION_TYPE byte
	and 0x00 byte (end of cmd).
*/

#ifndef ARMSCOPE_CMD_H
#define ARMSCOPE_CMD_H

/** \brief First byte of command */
enum E_COMMAND_TYPE
{
	/** \brief Set analog front-end sensitivity.
	
	Accepted values 0x01..0x06,
	lower value means higher sensitivity. Values are:
	3.3V/div, 1.1V/div, 0.37V/div, 0.12V/div, 0.04V/div and 0.013V/div.
	Range is -5 div to +5 div.
	*/
	CMD_SET_SENSITIVITY = 0x04,

	/** \brief Send special command (E_SUBCOMMAND_FUNCTION_TYPE)
	*/
	CMD_FUNCTION = 0x05,
};

/** \brief Second byte of command if first byte is CMD_FUNCTION */
enum E_SUBCOMMAND_FUNCTION_TYPE
{
	/** \brief Request to send identify string (basically "VDSO" + compilation date) */
	CMD_FUNCTION_IDENTIFY = 0x08,

	/** \brief Set analog front-end coupling to direct current path */
	CMD_FUNCTION_COUPLING_DC = 0x01,

	/** \brief Set analog front-end coupling to AC */
	CMD_FUNCTION_COUPLING_AC = 0x02,
};

#endif
