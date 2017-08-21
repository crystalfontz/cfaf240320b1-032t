#include "ST7789V.h"
// ********************************************************
void clearScreen(void)
{
	unsigned int i,j;

	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			writeColor(BLACK);
		}
	}
}
// ********************************************************
void displayInit(void)
{
	SET_RESET
	delay(1);
	CLR_RESET
	delay(10);
	SET_RESET
	delay(120);
	SET_RD

	writeCommand(0xb2); // Porch Control
	writeData(0x0c);	// BPA[0:6] - Back porch setting in normal mode. The minimum setting is 0x01 
	writeData(0x0c);	// FPA[0:6] - Front porch setting in normal mode. The minimum setting is 0x01
	writeData(0x00);	// PSEN - Enable separate porch control
	writeData(0x33);	// BPB[0:3] - Back porch setting in idle mode. The minimum setting is 0x01
						// FPB[0:3] - Front porch setting in idle mode. The minimum setting is 0x01
	writeData(0x33);	// BPC[0:3] - Back porch setting in partial mode. The minimum setting is 0x01
						// FPC[0:3] - Front porch setting in partial mode. The minimum setting is 0x01

	writeCommand(0xb7); // Gate Control
	writeData(0x35);	// VGHS[2:0] & VGLS[2:0]
	//VGHS[2:0] VGH (V)
	//00h       12.2
	//01h       12.54
	//02h       12.89
	//03h       13.26
	//04h       13.65
	//05h       14.06
	//06h       14.5
	//07h       14.97
	
	//VGLS[2:0]  VGL (V)
	//00h        -7.16
	//01h        -7.67
	//02h        -8.23
	//03h        -8.87
	//04h        -9.6
	//05h        -10.43
	//06h        -11.38
	//07h        -12.5

	writeCommand(0x3A); // Interface Pixel Format
	writeData(0x55);	// 65K of RGB interface - 16bit/pixel
	// 	Bit	| Description                    | Comments
	//  -------------------------------------------------------------------
	// 	D7  | -                              | Set to ‘0’
	//  -------------------------------------------------------------------
	// 	D6  |                                | ‘101’ = 65K of RGB interface
	// 	D5  | RGB interface color format     | ‘110’ = 262K of RGB
	// 	D4  |                                | interface
	//  -------------------------------------------------------------------
	// 	D3  | -                              | Set to ‘0’
	//  -------------------------------------------------------------------
	// 	D2  |                                | ‘011’ = 12bit/pixel
	// 	D1  | Control interface color format | ‘101’ = 16bit/pixel
	// 	D0  |                                | ‘110’ = 18bit/pixel
	// 	    |                                | ‘111’ = 16M truncated
	//  -------------------------------------------------------------------

	writeCommand(0x36);	// Memory Data Access Control
	writeData(0x00);
	// 	Bit | NAME | DESCRIPTION
	//  -------------------------------------
	// 	D7  | MY   | Page Address Order
	// 	D6  | MX   | Column Address Order
	// 	D5  | MV   | Page/Column Order
	// 	D4  | ML   | Line Address Order
	// 	D3  | RGB  | RGB/BGR Order
	// 	D2  | MH   | Display Data Latch Order
	// 	D1  | -    | Don't Care
	// 	D0  | -    | Don't Care

	writeCommand(0xbb);	// VCOM Setting
	writeData(0x2A);
	// 	VCOMS[5:0] | VCOM (V) | VCOMS[5:0] | VCOM (V)
	//  ---------------------------------------------
	//  00h        | 0.1      | 20h        | 0.9
	//  01h        | 0.125    | 21h        | 0.925
	// 	02h        | 0.15     | 22h        | 0.95
	// 	03h        | 0.175    | 23h        | 0.975
	// 	04h        | 0.2      | 24h        | 1.0
	// 	05h        | 0.225    | 25h        | 1.025
	// 	06h        | 0.25     | 26h        | 1.05
	// 	07h        | 0.275    | 27h        | 1.075
	// 	08h        | 0.3      | 28h        | 1.1
	// 	09h        | 0.325    | 29h        | 1.125
	// 	0Ah        | 0.35     | 2Ah        | 1.15
	// 	0Bh        | 0.375    | 2Bh        | 1.175
	// 	0Ch        | 0.4      | 2Ch        | 1.2
	// 	0Dh        | 0.425    | 2Dh        | 1.225
	// 	0Eh        | 0.45     | 2Eh        | 1.25
	// 	0Fh        | 0.475    | 2Fh        | 1.275
	// 	10h        | 0.5      | 30h        | 1.3
	// 	11h        | 0.525    | 31h        | 1.325
	// 	12h        | 0.55     | 32h        | 1.35
	// 	13h        | 0.575    | 33h        | 1.375
	// 	14h        | 0.6      | 34h        | 1.4
	// 	15h        | 0.625    | 35h        | 1.425
	// 	16h        | 0.65     | 36h        | 1.45
	// 	17h        | 0.675    | 37h        | 1.475
	// 	18h        | 0.7      | 38h        | 1.5
	// 	19h        | 0.725    | 39h        | 1.525
	// 	1Ah        | 0.75     | 3Ah        | 1.55
	// 	1Bh        | 0.775    | 3Bh        | 1.575
	// 	1Ch        | 0.8      | 3Ch        | 1.6
	// 	1Dh        | 0.825    | 3Dh        | 1.625
	// 	1Eh        | 0.85     | 3Eh        | 1.65
	// 	1Fh        | 0.875    | 3Fh        | 1.675

	writeCommand(0xc3);	// VRH Set
	writeData(0x0A);
	// VRHS[5:0] | VAP(GVDD) (V)                   | VRHS[5:0] | VAP(GVDD) (V)
	// -----------------------------------------------------------------------------------------
	// 00h       | 3.55+(vcom+vcom offset+0.5vdv)  | 15h       | 4.6+( vcom+vcom offset+0.5vdv)
	// 01h       | 3.6+( vcom+vcom offset+0.5vdv)  | 16h       | 4.65+( vcom+vcom offset+0.5vdv)
	// 02h       | 3.65+( vcom+vcom offset+0.5vdv) | 17h       | 4.7+( vcom+vcom offset+0.5vdv)
	// 03h       | 3.7+( vcom+vcom offset+0.5vdv)  | 18h       | 4.75+( vcom+vcom offset+0.5vdv)
	// 04h       | 3.75+( vcom+vcom offset+0.5vdv) | 19h       | 4.8+( vcom+vcom offset+0.5vdv)
	// 05h       | 3.8+( vcom+vcom offset+0.5vdv)  | 1Ah       | 4.85+( vcom+vcom offset+0.5vdv)
	// 06h       | 3.85+( vcom+vcom offset+0.5vdv) | 1Bh       | 4.9+( vcom+vcom offset+0.5vdv)
	// 07h       | 3.9+( vcom+vcom offset+0.5vdv)  | 1Ch       | 4.95+( vcom+vcom offset+0.5vdv)
	// 08h       | 3.95+( vcom+vcom offset+0.5vdv) | 1Dh       | 5+( vcom+vcom offset+0.5vdv)
	// 09h       | 4+( vcom+vcom offset+0.5vdv)    | 1Eh       | 5.05+( vcom+vcom offset+0.5vdv)
	// 0Ah       | 4.05+( vcom+vcom offset+0.5vdv) | 1Fh       | 5.1+( vcom+vcom offset+0.5vdv)
	// 0Bh       | 4.1+( vcom+vcom offset+0.5vdv)  | 20h       | 5.15+( vcom+vcom offset+0.5vdv)
	// 0Ch       | 4.15+( vcom+vcom offset+0.5vdv) | 21h       | 5.2+( vcom+vcom offset+0.5vdv)
	// 0Dh       | 4.2+( vcom+vcom offset+0.5vdv)  | 22h       | 5.25+( vcom+vcom offset+0.5vdv)
	// 0Eh       | 4.25+( vcom+vcom offset+0.5vdv) | 23h       | 5.3+( vcom+vcom offset+0.5vdv)
	// 0Fh       | 4.3+( vcom+vcom offset+0.5vdv)  | 24h       | 5.35+( vcom+vcom offset+0.5vdv)
	// 10h       | 4.35+( vcom+vcom offset+0.5vdv) | 25h       | 5.4+( vcom+vcom offset+0.5vdv)
	// 11h       | 4.4+( vcom+vcom offset+0.5vdv)  | 26h       | 5.45+( vcom+vcom offset+0.5vdv)
	// 12h       | 4.45+( vcom+vcom offset+0.5vdv) | 27h       | 5.5+( vcom+vcom offset+0.5vdv)
	// 13h       | 4.5+( vcom+vcom offset+0.5vdv)  | 28h~3Fh   | Reserved
	// 14h       | 4.55+( vcom+vcom offset+0.5vdv) | --        | --

	// VRHS[5:0] | VAN(GVCL) (V)                    | VRHS[5:0] | VAN(GVCL) (V)
	// -------------------------------------------------------------------------------------------
	// 00h       | -3.55+(vcom+vcom offset-0.5vdv)  | 15h       | -4.6+( vcom+vcom offset-0.5vdv)
	// 01h       | -3.6+( vcom+vcom offset-0.5vdv)  | 16h       | -4.65+( vcom+vcom offset-0.5vdv)
	// 02h       | -3.65+( vcom+vcom offset-0.5vdv) | 17h       | -4.7+( vcom+vcom offset-0.5vdv)
	// 03h       | -3.7+( vcom+vcom offset-0.5vdv)  | 18h       | -4.75+( vcom+vcom offset-0.5vdv)
	// 04h       | -3.75+( vcom+vcom offset-0.5vdv) | 19h       | -4.8+( vcom+vcom offset-0.5vdv)
	// 05h       | -3.8+( vcom+vcom offset-0.5vdv)  | 1Ah       | -4.85+( vcom+vcom offset-0.5vdv)
	// 06h       | -3.85+( vcom+vcom offset-0.5vdv) | 1Bh       | -4.9+( vcom+vcom offset-0.5vdv)
	// 07h       | -3.9+( vcom+vcom offset-0.5vdv)  | 1Ch       | -4.95+( vcom+vcom offset-0.5vdv)
	// 08h       | -3.95+( vcom+vcom offset-0.5vdv) | 1Dh       | -5+( vcom+vcom offset-0.5vdv)
	// 09h       | -4+( vcom+vcom offset-0.5vdv)    | 1Eh       | -5.05+( vcom+vcom offset-0.5vdv)
	// 0Ah       | -4.05+( vcom+vcom offset-0.5vdv) | 1Fh       | -5.1+( vcom+vcom offset-0.5vdv)
	// 0Bh       | -4.1+( vcom+vcom offset-0.5vdv)  | 20h       | -5.15+( vcom+vcom offset-0.5vdv)
	// 0Ch       | -4.15+( vcom+vcom offset-0.5vdv) | 21h       | -5.2+( vcom+vcom offset-0.5vdv)
	// 0Dh       | -4.2+( vcom+vcom offset-0.5vdv)  | 22h       | -5.25+( vcom+vcom offset-0.5vdv)
	// 0Eh       | -4.25+( vcom+vcom offset-0.5vdv) | 23h       | -5.3+( vcom+vcom offset-0.5vdv)
	// 0Fh       | -4.3+( vcom+vcom offset-0.5vdv)  | 24h       | -5.35+( vcom+vcom offset-0.5vdv)
	// 10h       | -4.35+( vcom+vcom offset-0.5vdv) | 25h       | -5.4+( vcom+vcom offset-0.5vdv)
	// 11h       | -4.4+( vcom+vcom offset-0.5vdv)  | 26h       | -5.45+( vcom+vcom offset-0.5vdv)
	// 12h       | -4.45+( vcom+vcom offset-0.5vdv) | 27h       | -5.5+( vcom+vcom offset-0.5vdv)
	// 13h       | -4.5+( vcom+vcom offset-0.5vdv)  | 28h~3Fh   | Reserved
	// 14h       | -4.55+( vcom+vcom offset-0.5vdv) | --        | --

	writeCommand(0xc4);	// VDV Setting
	writeData(0x20);
	// VDVS[5:0] | VDV (V) | VDVS[5:0] | VDV (V)
	//------------------------------------------
	// 00h       | -0.8    | 20h       | 0
	// 01h       | -0.775  | 21h       | 0.025
	// 02h       | -0.75   | 22h       | 0.05
	// 03h       | -0.725  | 23h       | 0.075
	// 04h       | -0.7    | 24h       | 0.1
	// 05h       | -0.675  | 25h       | 0.125
	// 06h       | -0.65   | 26h       | 0.15
	// 07h       | -0.625  | 27h       | 0.175
	// 08h       | -0.6    | 28h       | 0.2
	// 09h       | -0.575  | 29h       | 0.225
	// 0Ah       | -0.55   | 2Ah       | 0.25
	// 0Bh       | -0.525  | 2Bh       | 0.275
	// 0Ch       | -0.5    | 2Ch       | 0.3
	// 0Dh       | -0.475  | 2Dh       | 0.325
	// 0Eh       | -0.45   | 2Eh       | 0.35
	// 0Fh       | -0.425  | 2Fh       | 0.375
	// 10h       | -0.4    | 30h       | 0.4
	// 11h       | -0.375  | 31h       | 0.425
	// 12h       | -0.35   | 32h       | 0.45
	// 13h       | -0.325  | 33h       | 0.475
	// 14h       | -0.3    | 34h       | 0.5
	// 15h       | -0.275  | 35h       | 0.525
	// 16h       | -0.25   | 36h       | 0.55
	// 17h       | -0.225  | 37h       | 0.575
	// 18h       | -0.2    | 38h       | 0.6
	// 19h       | -0.175  | 39h       | 0.625
	// 1Ah       | -0.15   | 3Ah       | 0.65
	// 1Bh       | -0.125  | 3Bh       | 0.675
	// 1Ch       | -0.1    | 3Ch       | 0.7
	// 1Dh       | -0.075  | 3Dh       | 0.725
	// 1Eh       | -0.05   | 3Eh       | 0.75
	// 1Fh       | -0.025  | 3Fh       | 0.775

	writeCommand(0xc6);	// Frame Rate Control in Normal Mode
	writeData(0x0f);
	// NLA[2 :0] : Inversion selection in normal mode.
	// 0x00 : dot inversion.
	// 0x07: column inversion.
	
	// RTNA[4:0]:
	// RTNA[4:0] | FR in normal mode (Hz) | RTNA[4:0] | FR in normal mode (Hz)
	//------------------------------------------------------------------------
	// 00h       | 119                    | 10h       | 58
	// 01h       | 111                    | 11h       | 57
	// 02h       | 105                    | 12h       | 55
	// 03h       | 99                     | 13h       | 53
	// 04h       | 94                     | 14h       | 52
	// 05h       | 90                     | 15h       | 50
	// 06h       | 86                     | 16h       | 49
	// 07h       | 82                     | 17h       | 48
	// 08h       | 78                     | 18h       | 46
	// 09h       | 75                     | 19h       | 45
	// 0Ah       | 72                     | 1Ah       | 44
	// 0Bh       | 69                     | 1Bh       | 43
	// 0Ch       | 67                     | 1Ch       | 42
	// 0Dh       | 64                     | 1Dh       | 41
	// 0Eh       | 62                     | 1Eh       | 40
	// 0Fh       | 60                     | 1Fh       | 39
	
	writeCommand(0xd0);	// Power Control 1
	writeData(0xa4);	// 0xA4 Required
	writeData(0xa2);	// AVDD[1:0], AVCL[1:0], VDS[1:0]
	// AVDD[1:0] | AVDD (V)
	// 00h       | 6.4
	// 01h       | 6.6
	// 02h       | 6.8
	// 03h       | Reserved

	// AVCL[1:0] | AVCL (V)
	// 00h       | -4.4
	// 01h       | -4.6
	// 02h       | -4.8
	// 03h       | -5.0

	// VDS[1:0]  | VDDS (V)
	// 00h       | 2.19
	// 01h       | 2.3
	// 02h       | 2.4
	// 03h       | 2.51

	writeCommand(0xe0);	// Positive Voltage Gamma Control
	//See datasheet for more information
	writeData(0xd0);
	writeData(0x00);
	writeData(0x02);
	writeData(0x07);
	writeData(0x0a);
	writeData(0x28);
	writeData(0x32);
	writeData(0x44);
	writeData(0x42);
	writeData(0x06);
	writeData(0x0e);
	writeData(0x12);
	writeData(0x14);
	writeData(0x17);

	writeCommand(0xe1);	// Negative Voltage Gamma Control
	//See datasheet for more information
	writeData(0xd0);
	writeData(0x00);
	writeData(0x02);
	writeData(0x07);
	writeData(0x0a);
	writeData(0x28);
	writeData(0x31);
	writeData(0x54);
	writeData(0x47);
	writeData(0x0e);
	writeData(0x1c);
	writeData(0x17);
	writeData(0x1b);
	writeData(0x1e);

	//Set the display window to the full size of the display
	setDisplayWindow( 0x0000, 0x0000, 0x00EF, 0x013F);
	
	exitSleep();
}
//*********************************************************
void enterSleep (void)
{
	writeCommand(0x28);	// Display Off
	delay(20);
	writeCommand(0x10);	// Sleep In (Low power mode)
}
//*********************************************************
void exitSleep (void)
{
	writeCommand(0x11); // Exit Sleep Mode
	delay(120);
	writeCommand(0x29); // Display on

	writeCommand(0x2c); // Memory write
	// When this command is accepted, the column register and the page
	// register are reset to the start column/start page positions.
}
// ********************************************************
void fillScreen(unsigned long color)
{
	unsigned int i,j;

	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			writeColor(color);
		}
	}
}
//*********************************************************
void setDisplayWindow(int x0, int y0, int x1, int y1)
{
	writeCommand(0x2a);	// Column Address Set
	writeData(x0>>8);	// X address start:
	writeData(x0);		// 0 <= XS <= X
	writeData(x1>>8);	// X address end:
	writeData(x1);		// S <= XE <= X

	writeCommand(0x2b);	// Row Address Set
	writeData(y0>>8);	// Y address start:
	writeData(y0);		// 0 <= YS <= Y
	writeData(y1>>8);	// Y address start:
	writeData(y1);		// S <= YE <= Y
}
// ********************************************************
void setInterface(void)
{
	//Interface select
	uint8_t IM[4];

#ifdef MCU8BIT
	PORTA = 0x00;	//Pulled low if not in use
	IM[0] = 0;
	IM[1] = 1;
	IM[2] = 0;
	IM[3] = 0;
#endif
#ifdef MCU16BIT
	IM[0] = 0;
	IM[1] = 0;
	IM[2] = 0;
	IM[3] = 0;
#endif

	// PORTF is directly mapped to IM[0:3]
	PORTF = (IM[3]<<3 | IM[2]<<2 | IM[1]<<1 | IM[0]<<0);
}
// ********************************************************
void writeColorBars(void)
{
	unsigned int i,j;

	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			if(i>279)
			{
				writeColor(BLACK);
			}
			else if(i>239)
			{
				writeColor(BLUE);
			}
			else if(i>199)
			{
				writeColor(GREEN);
			}
			else if(i>159)
			{
				writeColor(CYAN);
			}
			else if(i>119)
			{
				writeColor(RED);
			}
			else if(i>79)
			{
				writeColor(MAGENTA);
			}
			else if(i>39)
			{
				writeColor(YELLOW);
			}
			else
			{
				writeColor(WHITE);
			}
		}
	}
}
