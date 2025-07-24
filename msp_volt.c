#include "fw_hal.h"
#include "fw_adc.h"

void volt_init(void)
{
	// // Set ADC1(GPIO P1.1) HIP
	// GPIO_P3_SetMode(GPIO_Pin_2, GPIO_Mode_Input_HIP);
	// Channel: ADC1
	ADC_SetChannel(0x0A);
	// ADC Clock = SYSCLK / 2 / (1+1) = SYSCLK / 4
	ADC_SetClockPrescaler(0x01);
	// Left alignment, high 8-bit in ADC_RES
	ADC_SetResultAlignmentLeft();
	// Turn on ADC power
	ADC_SetPowerState(HAL_State_ON);
}

// void uart_tx_dec(uint32_t dec)
// {
// 	char buf[10];
// 	int i = 0;
// 	if (dec == 0)
// 		buf[i++] = (dec % 10) + '0';
// 	while (dec > 0)
// 	{
// 		buf[i++] = (dec % 10) + '0';
// 		dec /= 10;
// 	}
// 	while (i--)
// 		UART1_TxChar(buf[i]);

// 	UART1_TxString("\r\n");
// }


void volt_read(void)
{
	static uint32_t timer_wait;
	if ((uint32_t)(timer_tick - timer_wait) > TM2_CAL_TIME(1000))
	{
		timer_wait = timer_tick;

		// R1 = 200kOhm, R2 = 16.8kOhm
		// range = 0 - 255;
		volt_value = ((uint32_t)(ADC_Convert() * 5) * 127 / 0xFF); // x10 volt.

		// uart_tx_dec(volt_value);
	}
}