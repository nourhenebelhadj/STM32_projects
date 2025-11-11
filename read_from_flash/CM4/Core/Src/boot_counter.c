#include "boot_counter.h"
#include "stm32h7xx_hal.h"

#define FLASH_USER_START_ADDR   0x080E0000UL  // choose safe flash page

uint32_t *flash_addr = (uint32_t *)FLASH_USER_START_ADDR;

uint32_t BootCounter_Read(void)
{


	// Check if memory is initialized (first boot)


	return *flash_addr;  // the stored counter
}

HAL_StatusTypeDef BootCounter_Write(uint32_t count)
{
	HAL_StatusTypeDef status;

	// Unlock Flash
	HAL_FLASH_Unlock();

	// Erase sector before writing (for STM32H7)
	FLASH_EraseInitTypeDef eraseInit;
	uint32_t sectorError;

	eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseInit.Banks = FLASH_BANK_1;
	eraseInit.Sector = FLASH_SECTOR_7;   // Adjust based on address
	eraseInit.NbSectors = 1;
	eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_4;

	status = HAL_FLASHEx_Erase(&eraseInit, &sectorError);
	if (status != HAL_OK)
	{
		HAL_FLASH_Lock();
		return status;
	}

	// Write two 32-bit words: [key, counter]
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, FLASH_USER_START_ADDR, (uint32_t)&count);

	HAL_FLASH_Lock();
	return status;
}
