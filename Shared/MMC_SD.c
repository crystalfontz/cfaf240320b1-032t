#include <avr/io.h>
#include "MMC_SD.h"

//spi low speed
void SPI_Low(void)
{
	SPCR =   _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);
	SPSR &= ~_BV(SPI2X);
}

//spi full speed
void SPI_High(void)
{
	SPCR =  _BV(SPE)|_BV(MSTR);
	SPSR |= _BV(SPI2X);
}

//port initialize
void SPI_Init(void)
{
//#ifndef interface3WSPI
	DDR_INI();
//#endif
	SPI_Low();
	SPI_High();
}

//read and write one byte
uint8 SPI_WriteByte(uint8 val)
{
	SPDR = val;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

/*uint8 SPI_ReadByte(void)
{
	SPDR = 0xff;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}*/
//sd卡初始化		//sd card initialize
void MMC_SD_Init(void)
{
	uint8 retry = 0;
	sd_restart: 	//label for "goto"

	OSCCAL = 0x00;    //in order to operate some low speed card the initialization should run at lowest speed

	SPI_Init();
	SPI_SD_CS_Deassert();

	while (MMC_SD_Reset())  // Initialize SD Card
	{
		retry++;
		if (retry > 254)
		{
			while (1)
			{
				goto sd_restart;				// try again
			}
		}
	}

	OSCCAL = 0xff;      //run at high speed for normal operation for SD Card

	delay(0xffff);

	if (FAT_Init())   	//initialize file system - FAT16 and FAT32 are supported
	{
		while (1)
		{
			goto sd_restart;				// try again
		}
	}
}

//sd send command
uint8 MMC_SD_SendCommand(uint8 cmd, uint32 arg)
{
	uint8 r1;
	uint8 retry=0;
	
	SPI_WriteByte(0xff);
	SPI_SD_CS_Assert();
	
	SPI_WriteByte(cmd | 0x40);//分别写入命令	//send command
	SPI_WriteByte(arg>>24);
	SPI_WriteByte(arg>>16);
	SPI_WriteByte(arg>>8);
	SPI_WriteByte(arg);
	SPI_WriteByte(0x95);
	
	while((r1 = SPI_WriteByte(0xff)) == 0xff)//等待响应，	//wait response
		if(retry++ > 254) break;//超时退出					//time out error

	SPI_SD_CS_Deassert();

	return r1;//返回状态值					//return state
}

//reset sd card (software)
uint8 MMC_SD_Reset(void)
{
	uint8 i;
	uint8 retry;
	uint8 r1=0;
	retry = 0;
	uint16 n = 0xffff;
	SPI_Low();
	do
	{
		for(i=0;i<10;i++) SPI_WriteByte(0xff);
		r1 = MMC_SD_SendCommand(0, 0);//发idle命令	//send idle command
		retry++;
		if(retry>254) return 1;//超时退出		//time out
	} while(r1 != 0x01);	

	while(n--);
		
	retry = 0;
	do
	{
		r1 = MMC_SD_SendCommand(1, 0);//发active命令	//send active command
		retry++;
		if(retry>254) return 1;//超时退出		//time out
	} while(r1);
	SPI_High();
	r1 = MMC_SD_SendCommand(59, 0);//关crc		//disable CRC

	r1 = MMC_SD_SendCommand(16, 512);//设扇区大小512	//set sector size to 512
	return 0;//正常返回		//normal return
}

//读一个扇区		//read one sector
uint8 MMC_SD_ReadSingleBlock(uint32 sector, uint8* buffer)
{
	uint8 r1;
	uint16 i;
	uint16 retry=0;

	r1 = MMC_SD_SendCommand(17, sector<<9);//读命令	//read command
	
	if(r1 != 0x00)
		return r1;

	SPI_SD_CS_Assert();
	//等数据的开始	//wait to start recieve data
	while(SPI_WriteByte(0xff) != 0xfe)if(retry++ > 1000){SPI_SD_CS_Deassert();return 1;}

	for(i=0; i<512; i++)//读512个数据	//read 512 bytes
	{
		*buffer++ = SPI_WriteByte(0xff);
	}

	SPI_WriteByte(0xff);//伪crc
	SPI_WriteByte(0xff);
	
	SPI_SD_CS_Deassert();

	return 0;
}


//写一个扇区		//wirite one sector //not used in this application
uint8 MMC_SD_WriteSingleBlock(uint32 sector, uint8* buffer)
{
	uint8 r1;
	uint16 i;
	uint16 retry=0;

	r1 = MMC_SD_SendCommand(24, sector<<9);//写命令	//send command
	if(r1 != 0x00)
		return r1;

	SPI_SD_CS_Assert();
	
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);

	SPI_WriteByte(0xfe);//发开始符			//send start byte
	
	for(i=0; i<512; i++)//送512字节数据		//send 512 bytes data
	{
		SPI_WriteByte(*buffer++);
	}
	
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	
	r1 = SPI_WriteByte(0xff);
	
	if( (r1&0x1f) != 0x05)//等待是否成功	//judge if it successful
	{
		SPI_SD_CS_Deassert();
		return r1;
	}
	//等待操作完		//wait no busy
	while(!SPI_WriteByte(0xff))if(retry++ > 2000){SPI_SD_CS_Deassert();return 1;}

	SPI_SD_CS_Deassert();

	return 0;
}



uint32 MMC_SD_ReadCapacity()
{
	uint8 r1;
	uint16 i;
	uint16 temp;
	uint8 buffer[16];
	uint32 Capacity;
	//uint8 retry=0;

	r1 = MMC_SD_SendCommand(9, 0);//写命令	//send command  //READ CSD
	if(r1 != 0x00)
		return r1;

	SPI_SD_CS_Assert();
	while(SPI_WriteByte(0xff) != 0xfe);
	
	for(i=0;i<16;i++)
	{
		buffer[i]=SPI_WriteByte(0xff);
	}	

	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	
	SPI_WriteByte(0xff);
	
	SPI_SD_CS_Deassert();

/*********************************/
//	C_SIZE
	i = buffer[6]&0x03;
	i<<=8;
	i += buffer[7];
	i<<=2;
	i += ((buffer[8]&0xc0)>>6);

/**********************************/
//  C_SIZE_MULT

	r1 = buffer[9]&0x03;
	r1<<=1;
	r1 += ((buffer[10]&0x80)>>7);


/**********************************/
// BLOCKNR

	r1+=2;

	temp = 1;
	while(r1)
	{
		temp*=2;
		r1--;
	}
	
	Capacity = ((uint32)(i+1))*((uint32)temp);

/////////////////////////
// READ_BL_LEN

	i = buffer[5]&0x0f;

/*************************/
//BLOCK_LEN

	temp = 1;
	while(i)
	{
		temp*=2;
		i--;
	}
/************************/


/************** formula of the capacity ******************/
//
//  memory capacity = BLOCKNR * BLOCK_LEN
//	
//	BLOCKNR = (C_SIZE + 1)* MULT
//
//           C_SIZE_MULT+2
//	MULT = 2
//
//               READ_BL_LEN
//	BLOCK_LEN = 2
/**********************************************/

//The final result
	
	Capacity *= (uint32)temp;	 
	return Capacity;		
}

void soft_spi_send_byte(char cmd, char data)
{
	// enable chip_sel
	asm("sbi %0, 7" : : "I" (_SFR_IO_ADDR(PORTB)));	// set chip select
	asm("cbi %0, 7" : : "I" (_SFR_IO_ADDR(PORTB)));	// clear chip select
	
	// Send the data/command flag bit
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));	//	clear the clock
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));	//	clear the SDIN
	asm("sbrc %0, 0" : : "a" (cmd));				//	check if the data should be set or not
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));	//	set the data
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));	//	set the clock
	
	// Send Bit 7 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 7" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// Send Bit 6 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 6" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// Send bit 5 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 5" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// Send bit 4 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 4" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// Send bit 3 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 3" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// Send bit 2 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 2" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// Send bit 1 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 1" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// Send bit 0 of data
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbrc %0, 0" : : "a" (data));
	asm("sbi %0, 6" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("sbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));
	
	// disable chip_sel
	asm("sbi %0, 7" : : "I" (_SFR_IO_ADDR(PORTB)));
	asm("cbi %0, 5" : : "I" (_SFR_IO_ADDR(PORTB)));	// clear the clock
}