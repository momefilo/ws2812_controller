#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "include/display.h"
#include "ws2812.pio.h"

//#define NUM_LEDS 16
#define WS2812_PIN 27

#define BUTTON_R 6
#define BUTTON_L 7
#define BUTTON_U 9
#define BUTTON_D 8


uint8_t PATTERNS[3][8][8]={
{	// streifen
	{54,55,56,57,58,59,60,61},
	{62,63,0,1,2,3,4,5},
	{6,7,8,9,10,11,12,13},
	{14,15,16,17,18,19,20,21},
	{22,23,24,25,26,27,28,29},
	{30,31,32,33,34,35,36,37},
	{38,39,40,41,42,43,44,45},
	{46,47,48,49,50,51,52,53}
},{	//schnecke
	{55,56,57,58,59,60,61,62},
	{6,14,22,30,38,46,54,53},
	{52,51,50,49,48,47,39,31},
	{23,15,7,63,0,1,2,3},
	{4,5,13,21,29,37,45,44},
	{43,42,41,40,32,24,16,8},
	{9,10,11,12,20,28,36,35},
	{34,33,25,17,18,19,27,26}
},{	//meander
	{55,56,57,58,59,60,61,62},
	{6,5,4,3,2,1,0,63},
	{7,8,9,10,11,12,13,14},
	{22,21,20,19,18,17,16,15},
	{23,24,25,26,27,28,29,30},
	{38,37,36,35,34,33,32,31},
	{39,40,41,42,43,44,45,46},
	{54,53,52,51,50,49,48,47}
}};