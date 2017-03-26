/**********************************************************************************
 *
 *  Copyright (c) 2009,Mlink-tech Inc
 *
 *  All Rights Reserved
 *
 *
 *  Filename:   SPIflash.c
 *
 *  State:       Draft 
 *  Version:     1.0
 *  Author:      YC
 *  Date:        5/11/2015
 *
 *  
 *  This file is for SPI flash utility
 *
 ***********************************************************************************/

#include "Evatronix\reg515.H"
#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"
//#include "pupcalib.h"
//#include "flash.h"
#include <intrins.h>
#include "uart.h"
#include "spiflash.h"

#ifdef SPIFLASH

sbit BOOTSEL=P2^1;
sbit SPICE=  P2^0;
sbit SPICLK= P2^2;
sbit SPISO=  P2^3;
sbit SPISI=  P0^0;

BYTE bdata spibyte;
sbit spibit7=  spibyte^7;
sbit spibit0=  spibyte^0;



void spiDelay(BYTE d)	 // T=1.578x+3.3 us
{
	BYTE i;
	for (i=0; i<d;i++) ;
}

void spiWriteByte(BYTE a)
{
	BYTE i;

	spibyte=a;

	for (i=0;i<8;i++)
	{
		SPICLK=0;
		SPISO=spibit7;
		spibyte=spibyte<<1;
		SPICLK=1;
	}
}

BYTE spiReadByte(void)
{
	BYTE i;

	for (i=0;i<8;i++)
	{
		SPICLK=0;
		spibyte=spibyte<<1;
		spibit0=SPISI;
		SPICLK=1;
	}

	return(spibyte);
}

BYTE spiRDSR(void)
{
    BYTE d;
	SPICE=0;
	spiWriteByte(05);
	d=spiReadByte();
	SPICE=1;

	return(d);
}

void spiWRSR(BYTE sr)
{
	SPICE=0;
	spiWriteByte(0x50);
	SPICE=1;

	spiDelay(0);
	SPICE=0;
	spiWriteByte(0x01);
	spiWriteByte(sr);
	SPICE=1;
}

BYTE spiCheckBusy(void)	  // Return 1: busy 0: ready
{
	return(spiRDSR() & 0x01);
}


void spiReadData(long add, USHORT len, BYTE xdata* dout)
{
	USHORT ct;

	SPICE=0;
	spiWriteByte(03);
	spiWriteByte(*(((BYTE*)&add)+1));
	spiWriteByte(*(((BYTE*)&add)+2));
	spiWriteByte(*(((BYTE*)&add)+3));

	for (ct=0;ct<len;ct++)
	{
		*(dout+ct)=spiReadByte();
		//spiReadByte();
	}

	SPICE=1;

}

void spiWriteData(long add, USHORT len, BYTE xdata* din)
{
	USHORT ct;

	SPICE=0;
	spiWriteByte(0x06);	// WREN
	SPICE=1;

	spiDelay(0);

	SPICE=0;
	spiWriteByte(0xaf);	// 
	spiWriteByte(*(((BYTE*)&add)+1));
	spiWriteByte(*(((BYTE*)&add)+2));
	spiWriteByte(*(((BYTE*)&add)+3));

	for (ct=0;ct<len;ct++)
	{
		spiWriteByte(*(din+ct));
		SPICE=1;
		spiDelay(12);	// 20us
		SPICE=0;
		if (ct != (len-1))
			spiWriteByte(0xaf);
		else
			spiWriteByte(0x04);	// WRDI

	}
	
	SPICE=1;
	spiDelay(0);
	
	spiRDSR(); 
}

void spiErase(long add, BYTE mode)  //  mode: 0: Sector 1: Block
{
	SPICE=0;
	spiWriteByte(0x06);	// WREN
	SPICE=1;

	spiDelay(0);

	SPICE=0;
	if (mode==0)
	spiWriteByte(0x20);
	else
	spiWriteByte(0x52);

	spiWriteByte(*(((BYTE*)&add)+1));
	spiWriteByte(*(((BYTE*)&add)+2));
	spiWriteByte(*(((BYTE*)&add)+3));

	SPICE=1;

	while (spiCheckBusy()) ;
}

void spiBoot(void)
{
#if 1
	BYTE xdata bcbuff[16];
	BYTE i;
	USHORT xdata fadd,ramadd,len,jmpadd;

	spiReadData(0,16,bcbuff);
	for (i=0;i<16;i++)
	{
		UARTSendString("\r\n");
		UARTSendByte(bcbuff[i]);

	}

	*(((BYTE*)&fadd)+0)=bcbuff[1];
	*(((BYTE*)&fadd)+1)=bcbuff[0];
	*(((BYTE*)&ramadd)+0)=bcbuff[3];
	*(((BYTE*)&ramadd)+1)=bcbuff[2];
	*(((BYTE*)&len)+0)=bcbuff[5];
	*(((BYTE*)&len)+1)=bcbuff[4];
	*(((BYTE*)&jmpadd)+0)=bcbuff[7];
	*(((BYTE*)&jmpadd)+1)=bcbuff[6];
	
	UARTSendString("\r\nFLS:");
	UARTSendInt16(fadd);
	UARTSendString("\r\nRAM:");
	UARTSendInt16(ramadd);
	UARTSendString("\r\nLEN:");
	UARTSendInt16(len);
	UARTSendString("\r\nJMP:");
	UARTSendInt16(jmpadd);

	spiReadData(fadd,len,(BYTE*)ramadd);

	((void(code *)(void)) jmpadd) ();
#endif

}


#endif	//SPIFLASH
