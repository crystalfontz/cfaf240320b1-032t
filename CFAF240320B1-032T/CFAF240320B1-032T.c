/*
 * CFAF240320B1-032T.c
 *
 * Created: 2015-09-23 10:11:37 AM
 *  Author: max
 */

//For the ATMEGA2561
#define F_CPU 8000000

#include "ST7789V.h"

int main(void)
{
	DDRA = 0xFF;	// low data port, all outputs DB[01-08]
	DDRB = 0x00;	// UNUSED
	DDRC = 0xFF;	// Command lines
	DDRD = 0x0F;	// VPP and led pins output
	DDRE = 0xFF;	// high data port, all outputs DB[10-17]
	//DDRF = 0xFF;	// Interface select - all outputs
	DDRF = 0x02;	// Interface select - only Pin 1 is an output
	
	PORTA = 0x00;	// initial values
	PORTB = 0x20;	// UNUSED
	PORTC = 0x00;	// Pin 0 BL pin/ off by default
	PORTD = 0xF0;	// pull-up on switches, all others off
	PORTE = 0x00;	// initial values
	PORTF = 0x00;	// initial values

	//Does the display work?
	BL_ON
	setInterface();
	displayInit();
	writeColorBars();
	delay(100);
	
	while(1)
	{
		// Initialize the uSD card of the CFA demo board
		MMC_SD_Init();    //SPI initialize

		// Put the pictures from the uSD card onto the display
		pictureSlideShow();
	}
}
