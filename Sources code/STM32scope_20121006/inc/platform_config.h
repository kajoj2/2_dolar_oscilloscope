#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

#include "stm32f10x.h"

#define BUZZER_PORT                     GPIOB
#define BUZZER_PIN                  	GPIO_Pin_0
#define RCC_APB2Periph_GPIO_BUZZER      RCC_APB2Periph_GPIOB

#endif /* __PLATFORM_CONFIG_H */

