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
	writeCommand(0x2c); // Memory write
	// When this command is accepted, the column register and the page
	// register are reset to the start column/start page positions.
}
// **************************************************
void pictureSlideShow()
{
	uint8 i;              //loop variable
	uint16 j;             //loop variable
	uint32 p;             //cluster
	uint16 *buffer;       //buffer
	uint16 pics = 1;
	uint16 totalFiles = 0;
	uint16 slideshowFlag = 1;
	uint8 sector;
	uint32 pixels;
	unsigned char * PATH = "\\batch";

	Search(PATH, &PictureInfo, &totalFiles);

	if (totalFiles == 0)
	{
		return;
	}

	if(!(buffer = malloc(512)))
	{
		return;
	}
	
	clearScreen(); // BLACK
	do{
		//find the file
		Search(PATH, &PictureInfo, &pics);                                         
		
		//the first cluster of the file
		p = PictureInfo.deStartCluster + (((uint32)PictureInfo.deHighClust) << 16);
		
		sector = 0;

		//read a sector
		FAT_LoadPartCluster(p, sector, buffer);

		// total # of pixels to fill
		pixels = (uint32) 320 * 240;

		// byte count
		j = 0; 
	
		while(pixels > 0)
		{
			writeColor(buffer[j]);  // write 16 bits
			pixels--;               // which is one pixel
			
			j++;                    // increment word count
			if (j == 256)           // time for a new sector
			{
				sector++;
				if (sector == SectorsPerClust)
				{
					p = FAT_NextCluster(p);	// read next cluster
					sector = 0;
				}

				FAT_LoadPartCluster(p, sector, buffer);	// read a sector
				j = 0;
			}
		}

		if(slideshowFlag)
		{
			for (i = 0; i < 3; i++)	// delay for a while
			{
				delay(0xFFFF);
			}

			pics++;					// increment picture number
			if (pics > totalFiles)	// if last
			{
				pics = 1;			// wrap around
			}

		}
	} while(slideshowFlag);
	
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
	CLR_CD
#ifdef MCU16BIT
	PORTE=command>>8;
	PORTA=command;
#endif
#ifdef MCU8BIT
	PORTE=command;
#endif
	CLR_WR
	SET_WR
	LED1_OFF
}
// **************************************************
void writeData(unsigned long data)
{
	LED2_ON
	SET_CD
#ifdef MCU16BIT
	PORTE=data>>8;
	PORTA=data;
#endif
#ifdef MCU8BIT
	PORTE=data;
#endif
	CLR_WR
	SET_WR
	LED2_OFF
}
// **************************************************
void writeColor(unsigned long color)
{
	LED2_ON
	SET_CD
#ifdef MCU16BIT	
	PORTE=color>>8;
	PORTA=color;
#endif
#ifdef MCU8BIT
 	CLR_WR
 	PORTE=color>>8;
 	SET_WR
	PORTE=color;
#endif
	CLR_WR
	SET_WR
	LED2_OFF
}
