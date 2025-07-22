// #include "fw_hal.h"

// enum
// {
// 	bt_mode_nomal,
// 	bt_mode_power,
// 	bt_mode_pluse,
// } bt_mode_run;

// uint16_t btn_time_tick;
// uint16_t btn_anti_vibration; // thời gian xác định nút đã nhấn
// uint8_t power, pluse;

// #define BTN_DOWN P36 // nút giảm
// #define BTN_UP P37	 // nút tăng
// #define active RESET
// #define de_active SET

// void button_timme_tick(void)
// {
// 	btn_time_tick += 1;
// }

// __BIT button_check_setup(void)
// {
// 	if (!BTN_DOWN && !BTN_UP)
// 	{
// 		if (btn_time_tick > 2000)
// 			if (bt_mode_run != bt_mode_nomal)
// 				bt_mode_run = bt_mode_power;
// 			else
// 				bt_mode_run = bt_mode_nomal;

// 		return SET;
// 	}
// 	else
// 	{
// 		btn_time_tick = 0;
// 		return RESET;
// 	}
// }

// typedef struct
// {
// 	uint8_t btn_stt;
// 	uint8_t checked;
// 	uint16_t time;
// } btn_stt_t;
// __BIT button_check(btn_stt_t sw1, btn_stt_t sw2)
// {

// }

// void button_scan(void)
// {
// 	switch (bt_mode_run)
// 	{
// 	case bt_mode_nomal:
// 		button_check_setup();
// 		break;

// 	case bt_mode_power:
// 		if (button_check_setup() == SET)
// 			break;
// 		else if (!BTN_DOWN)
// 			bt_mode_run = bt_mode_pluse;
// 		else if (!BTN_UP)
// 			(power < 3) ? (power++) : (power = 1);
// 		break;

// 	case bt_mode_pluse:
// 		if (button_check_setup() == SET)
// 			break;
// 		else if (!BTN_DOWN)
// 			bt_mode_run = bt_mode_power;
// 		else if (!BTN_UP)
// 			(pluse < 3) ? (pluse++) : (pluse = 1);
// 		break;
// 	}
// }