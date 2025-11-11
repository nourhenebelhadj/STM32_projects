#ifndef BOOT_COUNTER_H
#define BOOT_COUNTER_H

#include "stm32h7xx_hal.h"

uint32_t BootCounter_Read(void);
HAL_StatusTypeDef BootCounter_Write(uint32_t count);

#endif
