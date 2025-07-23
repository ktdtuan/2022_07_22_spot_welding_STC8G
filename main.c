#include "fw_hal.h"

static uint32_t timer_tick;
static uint32_t volt_value;

typedef enum
{
	bt_mode_nomal,
	bt_mode_power,
	bt_mode_pluse,
} bt_mode_run_t;
struct
{
	bt_mode_run_t mode; // mode đang hoạt động
	uint8_t duty;		// thời gian on 1 xung
	uint8_t power;		// năng lượng giá trị chứa 1, 2, 3
	uint8_t pulse;		// số xung. giá trị chứa 1, 3, 5
} systerm;

void uart_tx_dec(uint32_t dec);

void enable_trigger(void);
#include "msp_segment.c"
#include "msp_volt.c"
#include "msp_button.c"

static uint8_t counter = 0;
static uint8_t location = 0;

const uint8_t edge[6] = {0, 1, 0, 1, 0, 1};
uint16_t frequency[6] = {4, 5, 4, 5, 1, 100};
INTERRUPT(Timer0_Routine, EXTI_VectTimer0)
{
	if (++counter >= frequency[location])
	{
		counter = 0;
		if (++location >= systerm.pulse)
			TIM_Timer0_SetRunState(HAL_State_OFF);
		P11 = edge[location];
	}
}

INTERRUPT(Timer2_Routine, EXTI_VectTimer2) // interrupt 10ms
{
	timer_tick += 1;
}

void uart_tx_dec(uint32_t dec)
{
	char buf[10];
	int i = 0;
	if (dec == 0)
		buf[i++] = (dec % 10) + '0';
	while (dec > 0)
	{
		buf[i++] = (dec % 10) + '0';
		dec /= 10;
	}
	while (i--)
		UART1_TxChar(buf[i]);

	UART1_TxString("\r\n");
}

void enable_trigger(void)
{
	uint8_t i;

	for (i = 0; i < systerm.pulse - 1; i += 2)
	{
		frequency[i] = 4;
		frequency[i + 1] = 5;
	}
	frequency[i] = systerm.duty * systerm.power;
	frequency[i + 1] = 100;

	location = 0;
	TIM_Timer0_SetRunState(HAL_State_ON);
	P11 = 0;
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
	volt_init();

	// UART1 configuration: baud 115200 with Timer1, 1T mode, no interrupt
	UART1_Config8bitUart(UART1_BaudSource_Timer1, HAL_State_ON, 115200);

	TIM_Timer0_Config(HAL_State_ON, TIM_TimerMode_16BitAuto, 1000);
	EXTI_Timer0_SetIntState(HAL_State_ON);
	EXTI_Timer0_SetIntPriority(EXTI_IntPriority_High);

	// timer for virtual
	// 1T mode, prescaler:255+1, frequency: 1, interrupt: ON
	TIM_Timer2_Config(HAL_State_ON, 0xFF, 200);
	EXTI_Timer2_SetIntState(HAL_State_ON);

	EXTI_Global_SetIntState(HAL_State_ON);
	// TIM_Timer0_SetRunState(HAL_State_ON);
	TIM_Timer2_SetRunState(HAL_State_ON);

	//
	systerm.mode = bt_mode_nomal;
	systerm.duty = 5;
	systerm.power = 1;
	systerm.pulse = 1;
	while (1)
	{
		seg_handle();
		seg_request();
		volt_read();
		btn_handle();
	}
}
