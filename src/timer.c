/*****************************************************************************************************************
 *
 *  Copyright (c) 2015,MLink-tech Inc.
 *
 *  All Rights Reserved
 *
 *
 *
 *  Filename:timer.c
 *
 *
 *  State:Draft 
 *  Version:1.0
 *  Author:LYC
 *  Date:   04/17/2015
 *
 * 
 *	    
 *
 ******************************************************************************************************************/
#pragma noiv;

#include "Evatronix\reg515.H"

#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"
#include "timer.h"
#include "uart.h"


#ifdef USETIMER2
void init_timer2()	  // Using Timer2 for Tick
{
#define SYSTEM_CLOCK 12000000
#define TICKFREQ 20

	T2CON=0;
	CRCL=(65536-(SYSTEM_CLOCK/12/TICKFREQ)) & 0xff;
	CRCH=(65536-(SYSTEM_CLOCK/12/TICKFREQ))>>8 & 0xff;
	TL2=(65536-(SYSTEM_CLOCK/12/TICKFREQ)) & 0xff;
	TH2=(65536-(SYSTEM_CLOCK/12/TICKFREQ))>>8 & 0xff;
	T2CON=0x11;

	ET2=1;
	
}

void timer2_interrupt()	  // Using Timer2 for Tick
{
	
	TF2=0;
}
#endif

#if 0
void etimer2_init()
{
	rwReg(rw32TIMER2_CTR)=0x08;	// Reset timer counter
	//wReg32var(rw32TIMER1_MATCH0,time);
	//MACRO_ETIMER1M0_INT_CLR;
	rwReg(rw32TIMER2_CTR)=0x1;	// Start
}
#endif


void etimer1_set(ULONG time)
{
	rwReg(rw32TIMER1_CTR)=0x08;	// Reset timer counter
	wReg32var(rw32TIMER1_MATCH0,time);


	MACRO_ETIMER1M0_INT_CLR;
	rwReg(rw32TIMER1_CTR)=0x1;	// Start
}

void etimer1_delay(ULONG time)	// us
{
	etimer1_set(ETMCTR(time));
	while (!MACRO_ETIMER1M0_CHECK) ;
}

#if 0
void delayclk(ULONG syscyc)
{
	
	etimer1_set(syscyc);
	while (!MACRO_ETIMER1M0_CHECK)
	;
	
}
#endif

void delay_soft8(BYTE x)
{
	BYTE i;

	for (i=0;i<x;i++) ;	  
}

//void delay_soft32(ULONG x)
//{
//	ULONG xdata i;
//
//	for (i=0;i<x;i++) ;	  
//}

//void delay_soft_500us(ULONG x)
//{
//	ULONG xdata i;

//	for (i=0;i<x*17;i++) ;
//}