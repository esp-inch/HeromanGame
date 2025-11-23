
#include <stdint.h>
#include <pgmspace.h>
#pragma once


#define SPRITE_FRAME_WIDTH  46
#define SPRITE_FRAME_HEIGHT 50

#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
// ESP8266 and ESP32 support PROGMEM for flash storage
#include <pgmspace.h>
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
// PROGMEM is defefind for T4 to place data in specific memory section
#undef PROGMEM
#define PROGMEM
#else
#define PROGMEM
#endif


// Left-facing sprites removed - using runtime horizontal flip instead

#include "images/sprite_right_red_00.h"
#include "images/sprite_right_red_01.h"
#include "images/sprite_right_red_02.h"
#include "images/sprite_right_red_03.h"
#include "images/sprite_right_red_04.h"
#include "images/sprite_right_red_05.h"
#include "images/sprite_right_red_06.h"
#include "images/sprite_right_red_07.h"
#include "images/sprite_right_red_08.h"
#include "images/sprite_right_red_09.h"
#include "images/sprite_right_red_10.h"
#include "images/sprite_right_red_11.h"
#include "images/sprite_right_red_12.h"
#include "images/sprite_right_red_13.h"
#include "images/sprite_right_red_14.h"
#include "images/sprite_right_red_15.h"
#include "images/sprite_right_red_16.h"
#include "images/sprite_right_red_17.h"
#include "images/sprite_right_red_18.h"
#include "images/sprite_right_red_19.h"
#include "images/sprite_right_red_20.h"
#include "images/sprite_right_red_21.h"
#include "images/sprite_right_red_22.h"
#include "images/sprite_right_red_23.h"
#include "images/sprite_right_red_24.h"
#include "images/sprite_right_red_25.h"
#include "images/sprite_right_red_26.h"
#include "images/sprite_right_red_27.h"

#include "images/sprite_right_blue_00.h"
#include "images/sprite_right_blue_01.h"
#include "images/sprite_right_blue_02.h"
#include "images/sprite_right_blue_03.h"
#include "images/sprite_right_blue_04.h"
#include "images/sprite_right_blue_05.h"
#include "images/sprite_right_blue_06.h"
#include "images/sprite_right_blue_07.h"
#include "images/sprite_right_blue_08.h"
#include "images/sprite_right_blue_09.h"
#include "images/sprite_right_blue_10.h"
#include "images/sprite_right_blue_11.h"
#include "images/sprite_right_blue_12.h"
#include "images/sprite_right_blue_13.h"
#include "images/sprite_right_blue_14.h"
#include "images/sprite_right_blue_15.h"
#include "images/sprite_right_blue_16.h"
#include "images/sprite_right_blue_17.h"
#include "images/sprite_right_blue_18.h"
#include "images/sprite_right_blue_19.h"
#include "images/sprite_right_blue_20.h"
#include "images/sprite_right_blue_21.h"
#include "images/sprite_right_blue_22.h"
#include "images/sprite_right_blue_23.h"
#include "images/sprite_right_blue_24.h"
#include "images/sprite_right_blue_25.h"
#include "images/sprite_right_blue_26.h"
#include "images/sprite_right_blue_27.h"


// player_left_frames array removed - sprites are flipped at runtime instead

const unsigned short *player_right_frames[] = {
                                          sprite_right_red_00
                                        , sprite_right_red_01
                                        , sprite_right_red_02
                                        , sprite_right_red_03
                                        , sprite_right_red_04
                                        , sprite_right_red_05
                                        , sprite_right_red_06
                                        , sprite_right_red_07
                                        , sprite_right_red_08
                                        , sprite_right_red_09
                                        , sprite_right_red_10
                                        , sprite_right_red_11
                                        , sprite_right_red_12
                                        , sprite_right_red_13
                                        , sprite_right_red_14
                                        , sprite_right_red_15
                                        , sprite_right_red_16
                                        , sprite_right_red_17
                                        , sprite_right_red_18
                                        , sprite_right_red_19
                                        , sprite_right_red_20
                                        , sprite_right_red_21
                                        , sprite_right_red_22
                                        , sprite_right_red_23
                                        , sprite_right_red_24
                                        , sprite_right_red_25
                                        , sprite_right_red_26
                                        , sprite_right_red_27
                                        };

const unsigned short *player_right_blue_frames[] = {
                                          sprite_right_blue_00
                                        , sprite_right_blue_01
                                        , sprite_right_blue_02
                                        , sprite_right_blue_03
                                        , sprite_right_blue_04
                                        , sprite_right_blue_05
                                        , sprite_right_blue_06
                                        , sprite_right_blue_07
                                        , sprite_right_blue_08
                                        , sprite_right_blue_09
                                        , sprite_right_blue_10
                                        , sprite_right_blue_11
                                        , sprite_right_blue_12
                                        , sprite_right_blue_13
                                        , sprite_right_blue_14
                                        , sprite_right_blue_15
                                        , sprite_right_blue_16
                                        , sprite_right_blue_17
                                        , sprite_right_blue_18
                                        , sprite_right_blue_19
                                        , sprite_right_blue_20
                                        , sprite_right_blue_21
                                        , sprite_right_blue_22
                                        , sprite_right_blue_23
                                        , sprite_right_blue_24
                                        , sprite_right_blue_25
                                        , sprite_right_blue_26
                                        , sprite_right_blue_27
                                        };








// Old arrays removed - left sprites deleted, using runtime flipping instead


// #define PM_width 24
// #define PM_height 24
// 'PM0', 24x24px
// const unsigned char player_bmp_closed [] PROGMEM = {
// 	0x00, 0x7f, 0x00, 0xc0, 0xff, 0x01, 0xf0, 0xff, 0x07, 0xf8, 0xff, 0x0f, 0xfc, 0xff, 0x1f, 0xfc, 
// 	0xff, 0x1f, 0xfe, 0xff, 0x3f, 0xfe, 0xff, 0x3f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 
// 	0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0x3f, 
// 	0xfe, 0xff, 0x3f, 0xfc, 0xff, 0x1f, 0xfc, 0xff, 0x1f, 0xf8, 0xff, 0x0f, 0xf0, 0xff, 0x07, 0xc0, 
// 	0xff, 0x01, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00
// };
// // 'PM1', 24x24px
// const unsigned char player_bmp_right_1 [] PROGMEM = {
// 	0x00, 0x7f, 0x00, 0xc0, 0xff, 0x01, 0xf0, 0xff, 0x07, 0xf8, 0xff, 0x0f, 0xfc, 0xff, 0x1f, 0xfc, 
// 	0xff, 0x1f, 0xfe, 0xff, 0x3f, 0xfe, 0xff, 0x3f, 0xff, 0xff, 0x0f, 0xff, 0xff, 0x01, 0xff, 0x1f, 
// 	0x00, 0xff, 0x07, 0x00, 0xff, 0x1f, 0x00, 0xff, 0xff, 0x01, 0xff, 0xff, 0x0f, 0xfe, 0xff, 0x3f, 
// 	0xfe, 0xff, 0x3f, 0xfc, 0xff, 0x1f, 0xfc, 0xff, 0x1f, 0xf8, 0xff, 0x0f, 0xf0, 0xff, 0x07, 0xc0, 
// 	0xff, 0x01, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00
// };
// // 'PM2', 24x24px
// const unsigned char player_bmp_right_2 [] PROGMEM = {
// 	0x00, 0x7f, 0x00, 0xc0, 0xff, 0x01, 0xf0, 0xff, 0x07, 0xf8, 0xff, 0x07, 0xfc, 0xff, 0x03, 0xfc, 
// 	0xff, 0x01, 0xfe, 0xff, 0x00, 0xfe, 0x7f, 0x00, 0xff, 0x3f, 0x00, 0xff, 0x1f, 0x00, 0xff, 0x0f, 
// 	0x00, 0xff, 0x07, 0x00, 0xff, 0x0f, 0x00, 0xff, 0x1f, 0x00, 0xff, 0x3f, 0x00, 0xfe, 0x7f, 0x00, 
// 	0xfe, 0xff, 0x00, 0xfc, 0xff, 0x01, 0xfc, 0xff, 0x03, 0xf8, 0xff, 0x07, 0xf0, 0xff, 0x07, 0xc0, 
// 	0xff, 0x01, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00
// };
// // 'PM1', 24x24px
// const unsigned char player_bmp_left_1 [] PROGMEM = {
// 	0x00, 0xfe, 0x00, 0x80, 0xff, 0x03, 0xe0, 0xff, 0x0f, 0xf0, 0xff, 0x1f, 0xf8, 0xff, 0x3f, 0xf8, 
// 	0xff, 0x3f, 0xfc, 0xff, 0x7f, 0xfc, 0xff, 0x7f, 0xf0, 0xff, 0xff, 0x80, 0xff, 0xff, 0x00, 0xf8, 
// 	0xff, 0x00, 0xe0, 0xff, 0x00, 0xf8, 0xff, 0x80, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xfc, 0xff, 0x7f, 
// 	0xfc, 0xff, 0x7f, 0xf8, 0xff, 0x3f, 0xf8, 0xff, 0x3f, 0xf0, 0xff, 0x1f, 0xe0, 0xff, 0x0f, 0x80, 
// 	0xff, 0x03, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00
// };
// // 'PM2', 24x24px
// const unsigned char player_bmp_left_2 [] PROGMEM = {
// 	0x00, 0xfe, 0x00, 0x80, 0xff, 0x03, 0xe0, 0xff, 0x0f, 0xe0, 0xff, 0x1f, 0xc0, 0xff, 0x3f, 0x80, 
// 	0xff, 0x3f, 0x00, 0xff, 0x7f, 0x00, 0xfe, 0x7f, 0x00, 0xfc, 0xff, 0x00, 0xf8, 0xff, 0x00, 0xf0, 
// 	0xff, 0x00, 0xe0, 0xff, 0x00, 0xf0, 0xff, 0x00, 0xf8, 0xff, 0x00, 0xfc, 0xff, 0x00, 0xfe, 0x7f, 
// 	0x00, 0xff, 0x7f, 0x80, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xe0, 0xff, 0x1f, 0xe0, 0xff, 0x0f, 0x80, 
// 	0xff, 0x03, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00
// };

// // 'PMBlank', 24x24px
// const unsigned char player_bmp_blank [] PROGMEM = {
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };
