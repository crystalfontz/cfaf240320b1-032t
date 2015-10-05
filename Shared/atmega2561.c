#include "atmega2561.h"

// **************************************************
void delay(unsigned int t)
{
	unsigned char t1;
	while(t--)
	{
		for(t1=11;t1>0;t1--)
		{
			asm("nop");
		}
	}
}
// **************************************************
void displayHome(void)
{
	writeCommand(0x2A); //Set Column Address
	writeData(0x00);
	writeData(0x00);	//0
	writeData(0x00);
	writeData(0xEF);	//240

	writeCommand(0x2B); //Set Row Address
	writeData(0x00);
	writeData(0x00);	//0
	writeData(0x01);	
	writeData(0x3F);	//320
}
// **************************************************
void pictureSlideShow()
{
	uint8 i;              //loop variable
	uint16 j;             //loop variable
	uint32 p;             //cluster
	uint16 *buffer;       //buffer
	uint16 pics = 1;
	uint16 slide_show_flag = 1;
	uint8 sector;
	uint32 pixels;
	unsigned char * PATH = "\\batch";

	Search(PATH, &PictureInfo, &totalFiles);

	if (totalFiles == 0)
	return;

	buffer = malloc(512);
	if (buffer == 0)
	return;

	next: 	//label for "goto"
	clearScreen(); // BLACK

	Search(PATH, &PictureInfo, &pics);                                         //find the file
	p = PictureInfo.deStartCluster + (((uint32)PictureInfo.deHighClust) << 16);//the first cluster of the file

	sector = 0;

	FAT_LoadPartCluster(p, sector, buffer);	//read a sector

	pixels = (uint32) 320 * 240; // total # of pixels to write

	//displayHome();
	
	j = 0; // byte count
	
	while (1)
	{
		writeData(buffer[j]);  // write 16 bits
		pixels--;               // which is one pixel
		j++;                    // word count

		if (pixels == 0)
		{
			break;
		}

		if (j == 256)           // time for a new sector
		{
			sector++;
			if (sector == SectorsPerClust)
			{
				p = FAT_NextCluster(p);				// read next cluster
				sector = 0;
			}

			FAT_LoadPartCluster(p, sector, buffer);	// read a sector
			j = 0;
		}
	}

	if (slide_show_flag)
	{
		for (i=0;i<30;i++)	// delay for a while
		{
			delay(0x100);
		}

		pics++;					// increment picture number
		if (pics > totalFiles)	// if last
		{
			pics = 1;			// wrap around
		}

		goto next;				// go show next pic
	}
	free(buffer);
}
// **************************************************
void reset_display(void)
{
	CLR_RESET
	delay(10);
	SET_RESET
	delay(10);
	SET_RD
}
// **************************************************
void writeCommand(unsigned long command)
{
	LED1_ON
	switch(INTERFACE)
	{
		case MCU16BIT:
			CLR_CS
			CLR_CD
			PORTE=command>>8;
			PORTA=command;
			CLR_WR
			SET_WR
			SET_CS
			break;
// 		case MCU18BIT:
// 			CLR_CS
// 			CLR_CD
// 			PORTA=command;
// 			CLR_WR
// 			SET_WR
//			SET_CS
			break;
// 		case SPI3W:
// 			soft_spi_send_byte(0, command);
// 			break;
// 		case SPI4W:
// 			CLR_CS   // chip selected
// 			SPI_WriteByte(0x70);       // start byte for command
// 			SPI_WriteByte(command>>8); // set up data on bus
// 			SPI_WriteByte(command);    // set up data on bus
// 			SET_CS   // deselect chip
// 			break;
// 		case MCU18BIT:
// 			break;
	}
	LED1_OFF
}
// **************************************************
void writeData(unsigned long data)
{
	LED2_ON
	switch(INTERFACE)
	{
		case MCU16BIT:
			CLR_CS
			SET_CD
			PORTE=data>>8;
			PORTA=data;
			CLR_WR
			SET_WR
			SET_CS
			break;
// 		case MCU18BIT:
// 			CLR_CS
// 			SET_CD
// 			PORTA=data>>8;
// 			CLR_WR
// 			SET_WR
// 			PORTA=data;
// 			CLR_WR
// 			SET_WR
//			SET_CS
			break;
// 		case SPI4W:
// 			CLR_CS // chip selected
// 			SPI_WriteByte(0x72);	// start byte for data
// 			SPI_WriteByte(data>>8);	// set up data on bus
// 			SPI_WriteByte(data);	// set up data on bus
// 			SET_CS // deselect chip
// 			break;
// 		case SPI3W:
// 			soft_spi_send_byte(1, data);
// 			break;
// 		case MCU18BIT:
// 			break;
	}
	LED2_OFF
}
