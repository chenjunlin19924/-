/*****************************************************************************************************************
 *
 *  Copyright (c) 2015,MLink-tech Inc.
 *
 *  All Rights Reserved
 *
 *
 *
 *  Filename:isr.c
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
#pragma noiv

#include "Evatronix\reg515.H"
#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"

#include "timer.h"
#include "hostif.h"


void EXTINT0_FLASH_C(void) interrupt 0 //using 1
{

}
void TIMER0_FLASH_C(void) interrupt 1 //using 1
{

}
void EXTINT1_FLASH_C(void) interrupt 2 //using 1
{
	isr_hostif();
}
void TIMER1_FLASH_C(void) interrupt 3 //using 1
{

}
void UART0_FLASH_C(void) interrupt 4 //using 1
{

}
void TIMER2_FLASH_C(void) interrupt 5// using 1
{

}
void UART1_FLASH_C(void) interrupt 6 //using 1
{

}

void EXTINT2_FLASH_C(void) interrupt 9 //using 1
{

}
   
void EXTINT3_FLASH_C(void) interrupt 10 //using 1
{

}
void EXTINT4_FLASH_C(void) interrupt 11 //using 1
{
	isr_hostif_pos();
}
void EXTINT5_FLASH_C(void) interrupt 12 //using 1
{

}
void EXTINT6_FLASH_C(void) interrupt 13 //using 1
{

}

