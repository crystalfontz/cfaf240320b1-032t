#include "KD032QVTBN008.h"
void clearScreen(void)
{
	unsigned int i,j;
	displayHome();

	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
//			writeData(BLACK);
			writeData(BLUE);
		}
	}
}
void fillScreen(unsigned int color)
{
// 	unsigned int i,j, data;
// 	displayHome();
// 	data = 0;
// 	for(i=0;i<320;i++)
// 	{
// 		for(j=0;j<240;j++)
// 		{
// 			writeData(data++);
// 		}
// 	}
	long n, i, j;
	i = (color >> 8);
	j = (color & 0xFF);
	displayHome();
	//setXY(0,0,disp_y_size,disp_x_size);
	writeCommand(ST7789V_GRAM);

	for (n = 0; n < 76800; n++) {
		writeData(i);
		writeData(j);
	}
}
// **************************************************
void displayInit(void)
{
	SET_RESET
	delay(1);
	CLR_RESET
	delay(10);
	SET_RESET
	delay(120);
	SET_RD

	writeCommand(0xb2);
	writeData(0x0c);
	writeData(0x0c);
	writeData(0x00);
	writeData(0x33);
	writeData(0x33);

	writeCommand(0xb7);
	writeData(0x35);

	writeCommand(0x3A);
	writeData(0x55);

	writeCommand(0x36);
	writeData(0x00);

	writeCommand(0xbb);
	writeData(0x2A);//VCOM

	writeCommand(0xc3);
	writeData(0x0A);//GVDD

	writeCommand(0xc4);
	writeData(0x20);

	writeCommand(0xc6);
	writeData(0x0f);

	writeCommand(0xd0);
	writeData(0xa4);
	writeData(0xa2);

	writeCommand(0xe0);
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

	writeCommand(0xe1);
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


	writeCommand(0x2A); //Frame rate control
	writeData(0x00);
	writeData(0x00);
	writeData(0x00);
	writeData(0xEF);

	writeCommand(0x2B); //Display function control
	writeData(0x00);
	writeData(0x00);
	writeData(0x01);
	writeData(0x3F);

	writeCommand(0x11); //Exit Sleep
	delay(120);

	writeCommand(0x29); //display on

//	writeCommand(0x2c);

}
// **************************************************
void init2(void)
{
	SET_RESET
	delay(1);
	CLR_RESET
	delay(10);
	SET_RESET
	delay(120);
// 	
// 	SET_RESET();
// 	CLR_RESET();
	writeCommand(ST7789V_RESET);
	_delay_ms(100);
	
	writeCommand(ST7789V_POWERA);
	writeData(0x39);
	writeData(0x2C);
	writeData(0x00);
	writeData(0x34);
	writeData(0x02);
	writeCommand(ST7789V_POWERB);
	writeData(0x00);
	writeData(0xC1);
	writeData(0x30);
	writeCommand(ST7789V_DTCA);
	writeData(0x85);
	writeData(0x00);
	writeData(0x78);
	writeCommand(ST7789V_DTCB);
	writeData(0x00);
	writeData(0x00);
	writeCommand(ST7789V_POWER_SEQ);
	writeData(0x64);
	writeData(0x03);
	writeData(0x12);
	writeData(0x81);
	writeCommand(ST7789V_PRC);
	writeData(0x20);
	writeCommand(ST7789V_POWER1);
	writeData(0x23);
	writeCommand(ST7789V_POWER2);
	writeData(0x10);
	writeCommand(ST7789V_VCOM1);
	writeData(0x3E);
	writeData(0x28);
	writeCommand(ST7789V_VCOM2);
	writeData(0x86);
	writeCommand(ST7789V_MAC);
	writeData(0x40);
	writeCommand(ST7789V_PIXEL_FORMAT);
	writeData(0x55);
	writeCommand(ST7789V_FRC);
	writeData(0x00);
	writeData(0x18);
	writeCommand(ST7789V_DFC);
	writeData(0x08);
	writeData(0x82);
	writeData(0x27);
	writeCommand(ST7789V_3GAMMA_EN);
	writeData(0x00);
	writeCommand(ST7789V_COLUMN_ADDR);
	writeData(0x00);
	writeData(0x00);
	writeData(0x00);
	writeData(0xEF);
	writeCommand(ST7789V_PAGE_ADDR);
	writeData(0x00);
	writeData(0x00);
	writeData(0x01);
	writeData(0x3F);
	writeCommand(ST7789V_GAMMA);
	writeData(0x01);
	writeCommand(ST7789V_PGAMMA);
	writeData(0x0F);
	writeData(0x31);
	writeData(0x2B);
	writeData(0x0C);
	writeData(0x0E);
	writeData(0x08);
	writeData(0x4E);
	writeData(0xF1);
	writeData(0x37);
	writeData(0x07);
	writeData(0x10);
	writeData(0x03);
	writeData(0x0E);
	writeData(0x09);
	writeData(0x00);
	writeCommand(ST7789V_NGAMMA);
	writeData(0x00);
	writeData(0x0E);
	writeData(0x14);
	writeData(0x03);
	writeData(0x11);
	writeData(0x07);
	writeData(0x31);
	writeData(0xC1);
	writeData(0x48);
	writeData(0x08);
	writeData(0x0F);
	writeData(0x0C);
	writeData(0x31);
	writeData(0x36);
	writeData(0x0F);
	writeCommand(ST7789V_SLEEP_OUT);
	_delay_ms(100);
	writeCommand(ST7789V_DISPLAY_ON);
	writeCommand(ST7789V_GRAM);
	_delay_ms(20);	
}
void interfaceSelect(int selectedInterface)
{
	//PORTF = selectedInterface;
	PORTF = selectedInterface;
}
// **************************************************
void LCD_test(void)
{
	unsigned int i,j;
	displayHome();

	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			if(i>279)
				writeData(BLACK);
			else if(i>239)
				writeData(BLUE);
			else if(i>199)
				writeData(GREEN);
			else if(i>159)
				writeData(CYAN);
			else if(i>119)
				writeData(RED);
			else if(i>79)
				writeData(MAGENTA);
			else if(i>39)
				writeData(YELLOW);
			else
				writeData(WHITE);
		}
	}
}

//*********************************************************
void enterSleep (void)
{
    writeCommand(0x28);
    delay(20);
    writeCommand(0x10);
  
  }

//*********************************************************
void exitSleep (void)
{
	writeCommand(0x11);
	delay(120);
	writeCommand(0x29);
}

//*****************´°¿ÚµØÖ·************************************

void LCDWindowSet(int x0, int y0, int x1, int y1)
{ 
  writeCommand(0x2a);
  writeData(x0>>8);
  writeData(x0);
  writeData(x1>>8);
  writeData(x1); 
  
  writeCommand(0x2b);
  writeData(y0>>8);
  writeData(y0);
  writeData(y1>>8);
  writeData(y1); 

//  writeCommand(0x2c);
}