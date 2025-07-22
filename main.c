#include "fw_hal.h"
#include "timer_virtural.h"

static uint32_t timer_tick;
#include "segment.c"
#include "volt.c"
#include "button.c"

static uint16_t counter = 0;

INTERRUPT(Timer0_Routine, EXTI_VectTimer0)
{
	counter++;
	if (counter == 1000)
	{
		counter = 0;
		UART1_TxString("hello\r\n");
	}
}

INTERRUPT(Timer2_Routine, EXTI_VectTimer2)
{
	// UART1_TxString("hello\r\n");
	// timer_periodic_poll();
	// button_timme_tick();
	timer_tick += 1;
}

void IO_Config(void)
{
	// P1.0 → P1.7: Output push-pull (M1 = 0, M0 = 1)
	P1M1 = 0x00;
	P1M0 = 0xFF;

	// P3.0, P3.1, P3.3, P3.4, P3.5: Output push-pull
	// P3.2: analog input (float input)
	// P3.6, P3.7: input pull-up

	// Tính toán bit mask:
	// Push-pull (M1=0, M0=1): bit0,1,3,4,5 → M1 = 0, M0 = 1
	// Analog input (M1=0, M0=0): bit2 → M1 = 0, M0 = 0
	// Pull-up input (M1=1, M0=0): bit6,7 → M1 = 1, M0 = 0

	P3M1 = 0xC4; // 0b11000100; bit6,7 = 1 (pull-up input); các bit khác = 0
	P3M0 = 0x3B; // 0b00111011; bit0,1,3,4,5 = 1 (push-pull); bit2 = 0; bit6,7 = 0

	// P3.6, P3.7: input pull-up
	GPIO_SetPullUp(GPIO_Port_3, 0xC0, HAL_State_ON);

	// P5.4: input pull-up
	P5M1 = 0x10; // 0b00010000; bit4 = 1
	P5M0 = 0x00; // 0b00000000; bit4 = 0
}
// __BIT dir = SET;
void main(void)
{
	// Set system clock. Remove this line if system clock is already set by STC-ISP
	SYS_SetClock();

	IO_Config();
	// volt_init();

	// UART1 configuration: baud 115200 with Timer1, 1T mode, no interrupt
	UART1_Config8bitUart(UART1_BaudSource_Timer1, HAL_State_ON, 115200);

	//     // TIM_Timer0_Config(HAL_State_ON, TIM_TimerMode_16BitAuto, 1000);
	//     // EXTI_Timer0_SetIntState(HAL_State_ON);
	//     // EXTI_Timer0_SetIntPriority(EXTI_IntPriority_High);

	// timer for virtual
	// 1T mode, prescaler:255+1, frequency: 1, interrupt: ON
	TIM_Timer2_Config(HAL_State_ON, 0xFF, 100);
	EXTI_Timer2_SetIntState(HAL_State_ON);

	EXTI_Global_SetIntState(HAL_State_ON);
	//    TIM_Timer0_SetRunState(HAL_State_ON);
	TIM_Timer2_SetRunState(HAL_State_ON);
	while (1)
	{
		// seg_request();
		volt_read();
	}
}
