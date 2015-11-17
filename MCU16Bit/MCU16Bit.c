/*
 * MCU16Bit.c
 *
 * Created: 2015-09-23 10:11:37 AM
 *  Author: max
 */

//For the ATMEGA2561
#define F_CPU 8000000

//This sets up the code to generate the right command and data formats
#define MCU16BIT

#include "CFAF240320B1-032T.h"

int main(void)
{
	DDRA = 0xFF;	// low data port, all outputs DB[01-08]
	DDRB = 0x00;	// UNUSED
	DDRC = 0xFF;	// Command lines
	DDRD = 0x0F;	// VPP and led pins output
	DDRE = 0xFF;	// high data port, all outputs DB[10-17]
	DDRF = 0xFF;	// Interface select - all outputs
	
	PORTA = 0x00;	// initial values
	PORTB = 0x20;	// UNUSED
	PORTC = 0x00;	// Pin 0 BL pin/ off by default
	PORTD = 0xF0;	// pull-up on switches, all others off
	PORTE = 0x00;	// initial values

	//Does the display work?
	BL_ON
	setInterface();
	displayInit();
	writeColorBars();
	delay(100);
	
	// Initialize the uSD card of the CFA demo board
	MMC_SD_Init();    //SPI initialize

	// Put the pictures from the uSD card onto the display
	pictureSlideShow();
	
	while(1);
}
