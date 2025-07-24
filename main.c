#include "fw_hal.h"

static uint32_t timer_tick;
static uint32_t volt_value;

typedef enum
{
	mode_nomal,
	mode_power,
	mode_pluse,
} mode_run_t;

typedef enum
{
	mode_show_volt,
	mode_high_volt,
	mode_low_volt,
	mode_show_duty,
} mode_show_t;

struct
{
	uint8_t duty;  // thời gian on 1 xung
	uint8_t power; // năng lượng giá trị chứa 1, 2, 3
	uint8_t pulse; // số xung. giá trị chứa 1, 3, 5
} systerm;
mode_run_t button_mode; // mode đang hoạt động
mode_show_t show_mode;	// mode đang hiển thị (auto nhảy)

#define TM2_FREQUENCY 200							 // Hz
#define TM2_CAL_TIME(x) (x / (1000 / TM2_FREQUENCY)) // ms

#include "msp_eeprom.c"
#include "msp_segment.c"
#include "msp_volt.c"
#include "msp_button.c"

const uint8_t edge[6] = {0, 1, 0, 1, 0, 1};
static uint8_t tm1_counter = 0;
static uint8_t index_duty = 0;
uint16_t time_duty[6];

INTERRUPT(Timer0_Routine, EXTI_VectTimer0)
{
	if (++tm1_counter >= time_duty[index_duty])
	{
		tm1_counter = 0;
		if (++index_duty >= systerm.pulse)
			TIM_Timer0_SetRunState(HAL_State_OFF);
		P11 = edge[index_duty];
	}
}

INTERRUPT(Timer2_Routine, EXTI_VectTimer2) // interrupt 10ms
{
	timer_tick += 1;
}

void enable_trigger(void)
{
	uint8_t i;

	for (i = 0; i < systerm.pulse - 1; i += 2)
	{
		time_duty[i] = 4;
		time_duty[i + 1] = 5;
	}
	time_duty[i] = systerm.duty * systerm.power;
	time_duty[i + 1] = 100;

	index_duty = 0;
	TIM_Timer0_SetRunState(HAL_State_ON);
	P11 = 0;
}

void detect_trigger(void)
{
#define PIN_TRIGGER P54
	static uint32_t time_comfirm;
	static __BIT actived = RESET;

	if (PIN_TRIGGER == SET) // pin detect active
	{
		if (actived == RESET && (uint32_t)(timer_tick - time_comfirm) > TM2_CAL_TIME(1500))
		{
			actived = SET;
			enable_trigger();
		}
	}
	else // pin detect release
	{
		actived = RESET;
		time_comfirm = timer_tick;
	}
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
	P5M0 = 0x10; // 0b00000000; bit4 = 0
}
// __BIT dir = SET;
void main(void)
{
	// Set system clock. Remove this line if system clock is already set by STC-ISP
	SYS_SetClock();

	// read eeprom
	IAP_SetWaitTime();
	eeprom_read();

	IO_Config();
	volt_init();

	// UART1 configuration: baud 115200 with Timer1, 1T mode, no interrupt
	// UART1_Config8bitUart(UART1_BaudSource_Timer1, HAL_State_ON, 115200);

	TIM_Timer0_Config(HAL_State_ON, TIM_TimerMode_16BitAuto, 1000);
	EXTI_Timer0_SetIntState(HAL_State_ON);
	EXTI_Timer0_SetIntPriority(EXTI_IntPriority_High);

	// timer for virtual
	// 1T mode, prescaler:255+1, frequency: 1, interrupt: ON
	TIM_Timer2_Config(HAL_State_ON, 0xFF, TM2_FREQUENCY);
	EXTI_Timer2_SetIntState(HAL_State_ON);

	EXTI_Global_SetIntState(HAL_State_ON);
	// TIM_Timer0_SetRunState(HAL_State_ON);
	TIM_Timer2_SetRunState(HAL_State_ON);

	//
	show_mode = mode_show_volt;
	button_mode = mode_nomal;

	while (1)
	{
		detect_trigger();
		seg_handle();
		seg_request();
		volt_read();
		btn_handle();
	}
}
