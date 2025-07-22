// #include "fw_hal.h"
// #include "timer_virtural.h"

// // const uint8_t segment_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
// const uint8_t segment_code[36] = {
//     // 0–9
//     0x3F, // 0 → abcdef
//     0x06, // 1 → bc
//     0x5B, // 2 → abdeg
//     0x4F, // 3 → abcdg
//     0x66, // 4 → bcfg
//     0x6D, // 5 → acdfg
//     0x7D, // 6 → acdefg
//     0x07, // 7 → abc
//     0x7F, // 8 → abcdefg
//     0x6F, // 9 → abcdfg

//     // A–Z
//     0x77, // A → abcefg
//     0x7C, // B → cdefg (giống b thường)
//     0x39, // C → adef
//     0x5E, // D → bcdeg
//     0x79, // E → adefg
//     0x71, // F → aefg
//     0x3D, // G → acdef
//     0x76, // H → bcefg
//     0x06, // I → bc
//     0x1E, // J → bcde
//     0x75, // K → aefg (gần giống)
//     0x38, // L → def
//     0x37, // M → abcf (giả lập)
//     0x54, // N → ceg (giả lập)
//     0x3F, // O → abcdef
//     0x73, // P → abefg
//     0x6B, // Q → abcdfg
//     0x50, // R → eg
//     0x6D, // S → acdfg
//     0x78, // T → defg
//     0x3E, // U → bcdef
//     0x1C, // V → cde (giả lập)
//     0x2A, // W → bdeg (giả lập)
//     0x76, // X → bcefg (giống H)
//     0x6E, // Y → bcdfg
//     0x5B  // Z → abdeg
// };

// // // Gán LED dữ liệu hiển thị
// // struct
// // {
// // 	uint8_t enable;
// // 	uint8_t buffer[3];
// // 	timer_virtual_t time;
// // } seg_data[2];
// uint8_t led_buffer[3] = {1, 2, 3};

// // time refresh led
// timer_virtual_t seg_refresh;
// static uint8_t seg_index = 0;

// // Hàm xuất dữ liệu ra các segment (a-h)
// void seg_set_segments(uint8_t value)
// {
// 	// bit 0 → segment a (P3.4)
// 	P34 = value & 0x01;
// 	P10 = (value >> 1) & 0x01;
// 	P12 = (value >> 2) & 0x01;
// 	P13 = (value >> 3) & 0x01;
// 	P14 = (value >> 4) & 0x01;
// 	P15 = (value >> 5) & 0x01;
// 	P16 = (value >> 6) & 0x01;
// 	P17 = (value >> 7) & 0x01;
// }

// // Tắt cả 3 LED
// void seg_off_all()
// {
// 	P31 = 1;
// 	P30 = 1;
// 	P33 = 1;
// }

// // Hiển thị từng LED theo chỉ số
// void seg_display(uint8_t index)
// {
// 	seg_off_all();

// 	seg_set_segments(segment_code[led_buffer[index]]);

// 	switch (index)
// 	{
// 	case 0:
// 		P31 = 0;
// 		break;
// 	case 1:
// 		P30 = 0;
// 		break;
// 	case 2:
// 		P33 = 0;
// 		break;
// 	}
// }

// void seg_request(void)
// {
// 	// set interval refresh segment
// 	if(seg_refresh.start == _timer_off)
// 		timer_set(&seg_refresh, 5);

// 	//làm mới dữ liệu hiển thị ở màng hình
// 	if (timer_expired(&seg_refresh) == 1)
// 	{
// 		timer_restart(&seg_refresh);

// 		seg_display(seg_index);
// 		(seg_index < 2) ? (seg_index += 1) : (seg_index = 0);
// 	}
// }

// // void seg_show(uint8_t id, uint8_t en, uint8_t *buf, uint8_t sec)
// // {
// // 	if (id >= 2)
// // 		return;

// // 	if (en == 0)
// // 		led_buffer[id].enable = 0;

// // 	else
// // 	{
// // 		led_buffer[id].enable = 1;
// // 		led_buffer[id].buffer[0] = buf[0];
// // 		led_buffer[id].buffer[1] = buf[1];
// // 		led_buffer[id].buffer[2] = buf[2];
// // 		timer_set(&led_buffer[id].time, sec * 1000);
// // 	}
// // }