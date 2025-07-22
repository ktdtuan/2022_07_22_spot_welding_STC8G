#include "fw_hal.h"


#define BTN_SW1 P36 // nút tăng - SW1
#define BTN_SW2 P37 // nút giảm - SW2
#define active RESET
#define de_active SET

uint8_t btn_sw1_check(void)
{
	static uint32_t time_sw1;
	static __BIT stt_sw1 = de_active;
	if (BTN_SW1 == de_active && stt_sw1 == active)
	{
		uint32_t interval = (uint32_t)(timer_tick - time_sw1);

		stt_sw1 = de_active;
		if (interval > 200)
			return 2; // hold
		else if (interval > 20 && interval < 60)
			return 1; // click
	}

	else if (BTN_SW1 == active && stt_sw1 == de_active)
	{
		stt_sw1 = active;
		time_sw1 = timer_tick;
	}
	return 0; // nomal
}

uint8_t btn_sw2_check(void)
{
	static uint32_t time_sw2;
	static __BIT stt_sw2 = de_active;
	if (BTN_SW2 == de_active && stt_sw2 == active)
	{
		uint32_t interval = (uint32_t)(timer_tick - time_sw2);

		stt_sw2 = de_active;
		if (interval > 200)
			return 2; // hold
		else if (interval > 20 && interval < 60)
			return 1; // click
	}

	else if (BTN_SW2 == active && stt_sw2 == de_active)
	{
		stt_sw2 = active;
		time_sw2 = timer_tick;
	}
	return 0; // nomal
}

void btn_handle(void)
{
	uint8_t sw_stt1, sw_stt2;

	sw_stt1 = btn_sw1_check();
	sw_stt2 = btn_sw2_check();

	switch (systerm.mode)
	{
	case bt_mode_nomal:
		// click
		if (sw_stt1 == 1 && systerm.interval < 99)
			systerm.interval += 1;
		if (sw_stt2 == 1 && systerm.interval > 1)
			systerm.interval -= 1;

		// hold
		if (sw_stt1 == 2)
			systerm.mode = bt_mode_power;
		if (sw_stt2 == 2)
			systerm.mode = bt_mode_pluse;
		break;

	case bt_mode_power:
		// click
		if (sw_stt1 == 1 && systerm.power < 3)
			systerm.power += 1;
		if (sw_stt2 == 1 && systerm.power > 1)
			systerm.power -= 1;

		// hold
		if (sw_stt1 == 2)
			systerm.mode = bt_mode_nomal;
		break;

	case bt_mode_pluse:
		// click
		if (sw_stt1 == 1 && systerm.pulse < 3)
			systerm.pulse += 1;
		if (sw_stt2 == 1 && systerm.pulse > 1)
			systerm.pulse -= 1;

		// hold
		if (sw_stt2 == 2)
			systerm.mode = bt_mode_nomal;
		break;
	}
}
