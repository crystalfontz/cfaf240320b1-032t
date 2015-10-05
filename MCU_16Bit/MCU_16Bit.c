/*
 * MCU_16Bit.c
 *
 * Created: 2015-09-23 10:11:37 AM
 *  Author: max
 */ 

#include "KD032QVTBN008.h"

int main(void)
{
	DDRA = 0xFF;	// low data port, all outputs
	DDRB = 0xFF;	// UNUSED
	DDRC = 0xFF;	// Command lines
	DDRD = 0x0F;	// VPP and led pins output
	DDRE = 0xFF;	// high data port, all outputs
	DDRF = 0xFF;	// Interface select - all outputs
	
	PORTA = 0x00;	// initial values
	PORTB = 0x20;	// UNUSED
	PORTC = 0x00;	// Pin 0 BL pin/ off by default
	PORTD = 0xF8;	// pull-up on switches, all others off
	PORTE = 0x00;	// initial values

	//Does the display work?
	BL_ON
	interfaceSelect(INTERFACE);
	displayInit();
	LCD_test();
	delay(100);
	
	// Bring up the uSD card
	MMC_SD_Init();    //SPI initialize

	// Show the pictures from the uSD card on the display
	pictureSlideShow();
	
	while(1);
}
