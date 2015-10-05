#ifndef __MMC_SD_h__
#define __MMC_SD_h__

#include "FAT.h"

#define int8	signed char
#define	int16	signed int
#define int32	signed long
#define uint8	unsigned char
#define uint16	unsigned int
#define uint32	unsigned long

#define SD_PORT				PORTB						// use portB to communicate SD
#define	SD_CS_PIN			0							// mega128 PortB.0 ss
#define DDR_INI()			DDRB|=_BV(0)|_BV(1)|_BV(2)	// SS, MOSI, SCK for m128 portB, write
#define SPI_SD_CS_Assert()		SD_PORT &= ~_BV(SD_CS_PIN)	//active low SS=0
#define SPI_SD_CS_Deassert()	SD_PORT |= _BV(SD_CS_PIN)	// SS=1

extern void		MMC_SD_Init(void);
extern uint8	MMC_SD_Reset(void);
extern uint8	MMC_SD_ReadSingleBlock(uint32 sector, uint8 *buffer);
extern uint8	MMC_SD_WriteSingleBlock(uint32 sector, uint8 *buffer);
extern uint32	MMC_SD_ReadCapacity(void);
extern void		SPI_High(void);
extern void		SPI_Low(void);
extern uint8	SPI_WriteByte(uint8 val);
extern void		soft_spi_send_byte(char cmd, char data);

#endif /* __MMC_SD_h__ */