
#include "fw_hal.h"

#define TEST_ADDR 0x0400
#define TEST_SIZE sizeof(systerm)

void eeprom_read(void)
{
    uint8_t pos = 0;
    uint8_t *p_data = (uint8_t *)&systerm;
    // UART1_TxString("IAP_CmdRead\r\n");
    IAP_SetEnabled(HAL_State_ON);
    for (pos = 0; pos < TEST_SIZE; pos++)
    {
        IAP_CmdRead(TEST_ADDR + pos);
        if (IAP_IsCmdFailed())
            IAP_ClearCmdFailFlag();
        else
            p_data[pos] = IAP_ReadData();
    }
    IAP_SetEnabled(HAL_State_OFF);
}

void eeprom_write(void)
{
    uint8_t pos = 0;
    uint8_t *p_data = (uint8_t *)&systerm;
    // UART1_TxString("IAP_CmdWrite\r\n");
    IAP_SetEnabled(HAL_State_ON);
    for (pos = 0; pos < TEST_SIZE; pos++)
    {
        IAP_WriteData(p_data[pos]);
        IAP_CmdWrite(TEST_ADDR + pos);
        if (IAP_IsCmdFailed())
            IAP_ClearCmdFailFlag();
    }
    IAP_SetEnabled(HAL_State_OFF);
}
