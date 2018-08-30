#include "stm32f10x.h"
#include "buzzer.h"
#include "hw_config.h"
#include "delay.h"

void beep(void)	//buzzer
{
	  Buzzer_On();
	  /* delay */
	  for(volatile int i=0;i<0x50000;i++);
	  Buzzer_Off();
	  /* delay */
	  for(volatile int i=0;i<0x50000;i++);
}

void lbeep(void)
{
	  Buzzer_On();
	  /* delay */
	  for(volatile int i=0;i<0x100000;i++);
	  Buzzer_Off();
	  /* delay */
	  for(volatile int i=0;i<0x100000;i++);
}
