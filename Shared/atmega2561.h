#ifndef __ATMEGA2561_H__
#define __ATMEGA2561_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "MMC_SD.h"

extern uint16 SectorsPerClust;
extern uint16 FirstDataSector;
extern uint8 FAT32_Enable;

struct FileInfoStruct FileInfo;
struct direntry PictureInfo;

// all on PORTC
#define LCD_BL  PC0	//Backlight pin for funsies, I mean testing

#define	LCD_RES	PC2
#define	LCD_CS	PC4
//#define	LCD_E	PC5	// E in 6800 mode
#define	LCD_RD  PC5	// RD in 8080 mode
//#define	LCD_RW  PC6	// RW in 6800 mode
#define	LCD_WR  PC6	// WR in 8080 mode
#define	LCD_CD	PC7 // CD / RS

#define BL_OFF PORTC &= ~(1<<LCD_BL);
#define BL_ON  PORTC |=  (1<<LCD_BL);

#define CLR_RESET PORTC &= ~(1<<LCD_RES);
#define SET_RESET PORTC |=  (1<<LCD_RES);

#define CLR_CS PORTC &= ~(1<<LCD_CS);
#define SET_CS PORTC |=  (1<<LCD_CS);

#define CLR_CD PORTC &= ~(1<<LCD_CD);
#define SET_CD PORTC |=  (1<<LCD_CD);

// 6800 mode pin functions
//#define CLR_RW PORTC &= ~(1 << LCD_RW);   // 6800 mode
//#define SET_RW PORTC |=  (1 << LCD_RW);   // 6800 mode
//#define CLR_E  PORTC &= ~(1 << LCD_E);    // 6800 mode
//#define SET_E  PORTC |=  (1 << LCD_E);    // 6800 mode

// 8080 mode pin functions
#define CLR_RD PORTC &= ~(1 << LCD_RD);   // 8080 mode
#define SET_RD PORTC |=  (1 << LCD_RD);   // 8080 mode
#define CLR_WR PORTC &= ~(1 << LCD_WR);   // 8080 mode
#define SET_WR PORTC |=  (1 << LCD_WR);   // 8080 mode

#define LED_1     PD1
#define LED_2     PD2

#define LED1_OFF PORTD &= ~(1 << LED_1);
#define LED1_ON  PORTD |=  (1 << LED_1);
#define LED2_OFF PORTD &= ~(1 << LED_2);
#define LED2_ON  PORTD |=  (1 << LED_2);

extern void clearScreen(void);

void delay(unsigned int t);
void displayHome(void);
void pictureSlideShow();
void writeColor(unsigned long color);
void writeCommand(unsigned long command);
void writeData(unsigned long data);

#endif /* __ATMEGA2561_H__ */
