#include "fw_hal.h"

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

uint8_t bff[10];
// #include "stdio.h"
void volt_read(void)
{
	uint16_t volt_value;
	static uint32_t timer_wait_2s;
	if ((uint32_t)(timer_tick - timer_wait_2s) > 2000)
	{
		timer_wait_2s = timer_tick;

		ADC_Start();
		NOP();
		NOP();
		while (!ADC_SamplingFinished())			;
		ADC_ClearInterrupt();
		volt_value = ((ADC_RES * 5) / 0xFF) * 1268;
		// sprintf(bff, "%.2f", volt_value);
		// UART1_TxString(bff);
		// UART1_TxString("\r\n");
	}
}