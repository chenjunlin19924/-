/*****************************************************************************************************************
 *
 *  Copyright (c) 2015,MLink-tech Inc.
 *
 *  All Rights Reserved
 *
 *
 *
 *  Filename:uart.c
 *
 *
 *  State:Draft 
 *  Version:1.0
 *  Author:LW
 *  Date:   02/04/2015
 *
 * 
 *	    
 *
 ******************************************************************************************************************/

#include "Evatronix\reg515.H"

#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"
#include "uart.h"

#define SUPPORT_DEC_PRINT 0

//#define MACRO_UART1_INT_DIS (rwReg(rwPERI_INT_ENABLE0) &= ~2)
//#define MACRO_UART1_INT_EN  (rwReg(rwPERI_INT_ENABLE0) |= 2)
//#define MACRO_UART1_INT_CLR  (rwReg(wPERI_INT_CLR0)=2)


// ======================== UART physical Layer routines =====================
extern void delay_soft8(BYTE x);
void uart_init()
{
	bitCLR32(rw32UART1_CONTR,12);
	delay_soft8(100);

	rwReg(rw32UART1_MATCH+0)=(SYSTEM_CLOCK_REF*16/UART1BAUD-256) & 0xff;	
	rwReg(rw32UART1_MATCH+1)=(SYSTEM_CLOCK_REF*16/UART1BAUD-256)>>8  & 0xff;	
	rwReg(rw32UART1_MATCH+2)=(SYSTEM_CLOCK_REF*16/UART1BAUD-256)>>16 & 0xff;	
	rwReg(rw32UART1_MATCH+3)=(SYSTEM_CLOCK_REF*16/UART1BAUD-256)>>24 & 0xff;

	delay_soft8(100);

//	bitSET32(rw32UART1_CONTR,12);

//	bitSET32(rw32UART1_CONTR,13);
//	bitSET32(rw32UART1_CONTR,6);
//	bitCLR32(rw32UART1_CONTR,7);
//	bitSET32(rw32UART1_CONTR,18);
//	bitSET32(rw32UART1_CONTR,19);


	rwReg(rw32UART1_CONTR+0)=0x00;	
	rwReg(rw32UART1_CONTR+1)=0x10;	
	rwReg(rw32UART1_CONTR+2)=0x0c;	
	rwReg(rw32UART1_CONTR+3)=0x00;

}


	
void uart_put_char(BYTE c)
{
	while (1)
	{
		if (fldREAD32(r32UART1_STATUS,5,3)<(UART1HWFIFO_LEN-1))
		{
			rwReg(rwUART1_WDATA)=c;
			break;
		}
	}
}



BYTE uart_check(void)
{
	return rwReg(r32UART1_STATUS)&0x7;
}


BYTE uart_read(void)
{
 	return rwReg(rUART1_RDATA);
}



// ======================== UART Application Layer routines =====================

//***************************************
//Send a String to Serial Port
void UARTSendString(char *ptr)
{
	for (; *ptr != 0 ; ptr++)
	{
		 uart_put_char(*ptr);
		 //if (*ptr == kLF)
		 //		KernelUartPutChar(kCR);
	 }
}

//*************************************
//Send a Byte
code BYTE hex_[]={"0123456789ABCDEF"}; 
void UARTSendByte(BYTE c)
{
	BYTE ch;
	ch = (c>>4) & 0x0f;
	uart_put_char(hex_[ch]);
	ch = c & 0x0f;
	uart_put_char(hex_[ch]);
}
//*************************************
//Send a 16 Bits Int
void UARTSendInt16(USHORT c)
{
	UARTSendByte((BYTE) ((c >>8) & 0xff));
	UARTSendByte((BYTE) (c & 0xff));
}

//*************************************
//Send a 32 bits Long Int

void UARTSendInt32(ULONG c)
{
	BYTE *pt;

	pt =(BYTE*) &c;
	UARTSendByte(*(pt+0));
	UARTSendByte(*(pt+1));
	UARTSendByte(*(pt+2));
	UARTSendByte(*(pt+3));
}

// ============================== HEX to DEC ==================================
#if SUPPORT_DEC_PRINT==1
void HEX2DEC(int iHEX, char *ptDEC)
{
	char i;
	int x, x1;
	
	x = iHEX;
	
	if (x < 0)
	{
		x = -1*x;
		*ptDEC = '-';
	}
	else 
		*ptDEC = '+';
	
	for (i = 0; i < 5; i++)
	{
		x1 = x / 10;
		*(ptDEC + 5 - i) = x - x1*10 + '0';
		x = x1;
	}

}

// ============================== Send a 16 bits Int as DEC =====================

void UARTSendInt16DEC(int c)
{
	char decBF[8];
	HEX2DEC(c, &decBF[1]);
	decBF[0] = 'd';
	decBF[7] = 0;
	UARTSendString(&decBF[0]);
	
}

//*************************************
//Send a Byte in Decimal

void UARTSendByteDec(BYTE c)
{
	BYTE value,ch;
	value = c;

	ch = (value/100);
	value = value - 100*ch;
	if(ch) uart_put_char(0x30+ch);

	ch = (value/10);
	value = value - 10*ch;
	uart_put_char(0x30+ch);

	uart_put_char(0x30+value);
}
#endif

#if 0
void KernelSendByte_String(char *ptr,uchar c)
{
	int ch;

	KernelSendString(ptr);
	
	ch = (c>>4) & 0x0f;
	KernelUartPutChar(hex_[ch]);
	ch = c & 0x0f;
	KernelUartPutChar(hex_[ch]);
}

void KernelSendInt16_String(char *ptr,unsigned c)
{
	KernelSendString(ptr);
	KernelSendByte((uchar) ((c >>8) & 0xff));
	KernelSendByte((uchar) (c & 0xff));
}
#if SUPPORT_DEC_PRINT==1
void KernelSendInt16DEC_String(char *ptr,int c)
{
	char decBF[8];
	
	KernelSendString(ptr);
	KernelHEX2DEC(c, &decBF[1]);
	decBF[0] = 'd';
	decBF[7] = 0;
	KernelSendString(&decBF[0]);
	
}
#endif
void KernelSendInt32_String(char *ptr,unsigned long c)
{
	UCHAR *pt;
	
	KernelSendString(ptr);
	pt =(UCHAR*) &c;
	KernelSendByte(*(pt+3));
	KernelSendByte(*(pt+2));
	KernelSendByte(*(pt+1));
	KernelSendByte(*(pt+0));
}
#endif
