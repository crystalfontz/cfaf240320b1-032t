/*
 * cfaf240320a032t.h
 *
 * Created: 2015-08-18 9:16:09 AM
 *  Author: max
 */ 

#ifndef __KD032QVTBN008_h__
#define __KD032QVTBN008_h__

#include "atmega2561.h"
#define ST7789V_COLOR_WHITE			0xFFFF
#define ST7789V_COLOR_BLACK			0x0000
#define ST7789V_COLOR_RED			0xF800
#define ST7789V_COLOR_GREEN			0x07E0
#define ST7789V_COLOR_GREEN2		0xB723
#define ST7789V_COLOR_BLUE			0x001F
#define ST7789V_COLOR_BLUE2			0x051D
#define ST7789V_COLOR_YELLOW		0xFFE0
#define ST7789V_COLOR_ORANGE		0xFBE4
#define ST7789V_COLOR_CYAN			0x07FF
#define ST7789V_COLOR_MAGENTA		0xA254
#define ST7789V_COLOR_GRAY			0x7BEF //1111 0111 1101 1110
#define ST7789V_COLOR_BROWN			0xBBCA

#define ST7789V_RESET				0x01
#define ST7789V_SLEEP_OUT			0x11
#define ST7789V_GAMMA				0x26
#define ST7789V_DISPLAY_OFF			0x28
#define ST7789V_DISPLAY_ON			0x29
#define ST7789V_COLUMN_ADDR			0x2A
#define ST7789V_PAGE_ADDR			0x2B
#define ST7789V_GRAM				0x2C
#define ST7789V_MAC					0x36
#define ST7789V_PIXEL_FORMAT		0x3A
#define ST7789V_WDB					0x51
#define ST7789V_WCD					0x53
#define ST7789V_RGB_INTERFACE		0xB0
#define ST7789V_FRC					0xB1
#define ST7789V_BPC					0xB5
#define ST7789V_DFC					0xB6
#define ST7789V_POWER1				0xC0
#define ST7789V_POWER2				0xC1
#define ST7789V_VCOM1				0xC5
#define ST7789V_VCOM2				0xC7
#define ST7789V_POWERA				0xCB
#define ST7789V_POWERB				0xCF
#define ST7789V_PGAMMA				0xE0
#define ST7789V_NGAMMA				0xE1
#define ST7789V_DTCA				0xE8
#define ST7789V_DTCB				0xEA
#define ST7789V_POWER_SEQ			0xED
#define ST7789V_3GAMMA_EN			0xF2
#define ST7789V_INTERFACE			0xF6
#define ST7789V_PRC					0xF7

// color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void displayInit(void);
//void initOLED(void);
void interfaceSelect(int selectedInterface);
void LCD_test(void);
//void resetDisplay(void);
//void showBitmap(unsigned char bitmap);
//void clearScreen(void);
void enterSleep (void);
void exitSleep (void);
void LCDWindowSet(int x0,int y0,int x1,int y1);
void fillScreen(unsigned int color);
//
// Customer Code starts here
//

#endif /* __KD032QVTBN008_h__ */
