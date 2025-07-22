#include "fw_hal.h"

// const uint8_t segment_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
const uint8_t segment_code[39] = {
	// 0–9
	0x3F, // 0 → abcdef
	0x06, // 1 → bc
	0x5B, // 2 → abdeg
	0x4F, // 3 → abcdg
	0x66, // 4 → bcfg
	0x6D, // 5 → acdfg
	0x7D, // 6 → acdefg
	0x07, // 7 → abc
	0x7F, // 8 → abcdefg
	0x6F, // 9 → abcdfg

	// -, .
	0x40, // 10- -
	0x80, // 11- .

	// A–Z
	0x77, // 11- A → abcefg
	0x7C, // 12- B → cdefg (giống b thường)
	0x39, // 13- C → adef
	0x5E, // 14- D → bcdeg
	0x79, // 15- E → adefg
	0x71, // 16- F → aefg
	0x3D, // 17- G → acdef
	0x76, // 18- H → bcefg
	0x06, // 19- I → bc
	0x1E, // 20- J → bcde
	0x75, // 21- K → aefg (gần giống)
	0x38, // 22- L → def
	0x37, // 23- M → abcf (giả lập)
	0x54, // 24- N → ceg (giả lập)
	0x5C, // 25- O → bcdeg
	0x73, // 26- P → abefg
	0x6B, // 27- Q → abcdfg
	0x50, // 28- R → eg
	0x6D, // 29- S → acdfg
	0x78, // 30- T → defg
	0x1C, // 31- U → cde
	0x1C, // 32- V → cde (giả lập)
	0x2A, // 33- W → bdeg (giả lập)
	0x76, // 34- X → bcefg (giống H)
	0x6E, // 35- Y → bcdfg
	0x5B, // 36- Z → abdeg

	// off
	0x00 // 38
};

uint8_t seg_buffer[3] = {1, 2, 3};

// Hàm xuất dữ liệu ra các segment (a-h)
void seg_set_segments(uint8_t value)
{
	// bit 0 → segment a (P3.4)
	P34 = value & 0x01;
	P10 = (value >> 1) & 0x01;
	P12 = (value >> 2) & 0x01;
	P13 = (value >> 3) & 0x01;
	P14 = (value >> 4) & 0x01;
	P15 = (value >> 5) & 0x01;
	P16 = (value >> 6) & 0x01;
	P17 = (value >> 7) & 0x01;
}

// Tắt cả 3 LED
void seg_off_all()
{
	P31 = 1;
	P30 = 1;
	P33 = 1;
}

// Hiển thị từng LED theo chỉ số
void seg_display(uint8_t index)
{
	seg_off_all();

	// seg_set_segments(segment_code[seg_buffer[index]]);
	seg_set_segments(seg_buffer[index]);

	switch (index)
	{
	case 0:
		P31 = 0;
		break;
	case 1:
		P30 = 0;
		break;
	case 2:
		P33 = 0;
		break;
	}
}

void seg_request(void)
{
	static uint32_t timer_wait_2s;
	static uint8_t seg_index = 0;

	// set interval refresh segment
	if ((uint32_t)(timer_tick - timer_wait_2s) > 2)
		return;
	timer_wait_2s = timer_tick;

	// làm mới dữ liệu hiển thị ở màng hình
	seg_display(seg_index);
	(seg_index < 2) ? (seg_index += 1) : (seg_index = 0);
}

void seg_handle(void)
{
	static bt_mode_run_t mode;
	static uint16_t data_cmp;
	int8_t index_led = 2;

	// refresh data compare
	if (mode != systerm.mode)
	{
		mode = systerm.mode;
		data_cmp = 0;
	}

	switch (mode)
	{
	case bt_mode_nomal:
		if (data_cmp == volt_value)
			break;
		data_cmp = volt_value;

		while (index_led >= 0)
		{
			if (data_cmp > 0)
			{
				seg_buffer[index_led--] = segment_code[data_cmp % 10];
				data_cmp /= 10;
			}
			else
				seg_buffer[index_led--] = segment_code[38]; // off
		}
		seg_buffer[1] |= segment_code[11];
		break;
	case bt_mode_power:
		if (data_cmp == systerm.power)
			break;
		seg_buffer[0] = segment_code[27]; // P
		seg_buffer[1] = segment_code[26]; // o
		seg_buffer[2] = segment_code[systerm.power];
		data_cmp = systerm.power;
		break;
	case bt_mode_pluse:
		if (data_cmp == systerm.pulse)
			break;
		seg_buffer[0] = segment_code[27]; // P
		seg_buffer[1] = segment_code[32]; // o
		seg_buffer[2] = segment_code[systerm.pulse];
		data_cmp = systerm.pulse;
		break;
	}
}
