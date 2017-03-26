/*****************************************************************************************************************
 *
 *  Copyright (c) 2015,MLink-tech Inc.
 *
 *  All Rights Reserved
 *
 *
 *
 *  Filename:main.c
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
//#pragma iv(0x1000);
//#pragma noiv;
#define _CMAIN_
#include "Evatronix\reg515.H"
#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"
#include "cfg_hw.h"
#include "cfg_prod.h"

#include "uart.h"
#include "timer.h"
#include "spiflash.h"
#include "dcode.h"
#include <math.h>
#include "hostif.h"

#define _UARTSendString(x)

extern void uart_ui_loop(void); 
extern ULONG xdata databuf[];

#if HIGHVOL==1
sbit P15V=P2^6;
#endif
#if HIGHVOL==2
sbit P15V=P0^3;
#endif

extern char bcd_to_hex(char bcd);
void multi_turn_test(void);
char start_test=0;
//USHORT angle_resolution;

void main (void)
{

  	//ABS_Process_Rotary();
#ifdef SPIFLASH
	bitSET32(rwIOMUX_CTRL,24);
	bitSET32(rwIOMUX_CTRL,26);
	bitSET32(rwIOMUX_CTRL,27);
	bitCLR32(rwIOMUX_CTRL,16);
	bitCLR32(rwIOMUX_CTRL,25);
#endif
//apamp 96m/960=100k    0x3c0=960 //ffff DAC数字输出和power down
	wReg32(REG_APAMP_CFG1,0x7c0ffff);  // power down 15vamp
	P15V=0;
	
	#if HIGHVOL==1
	bitSET32(rwIOMUX_CTRL,30);			// p2.6 15v control
	#endif
	#if HIGHVOL==2
	bitSET32(rwIOMUX_CTRL,19);			// p0.3 15v control
	#endif



	//bitSET32(rwIOMUX_CTRL,30);			// p2.6 15v control
	//bitSET32(rwIOMUX_CTRL,19);			// p0.3 15v control

	
	rwReg(rw32CLK_CTRL+1) = 0xff;
	rwReg(rw32CLK_CTRL+2) |= 0x1f;

	uart_init();

	EAL=1;
	EX1=1;	  //host_if_mixed_interrupt
	EX0=1;	  //peripheral

	UARTSendString("MLINK\r\n");
	UARTSendString("Z");
	prod_curr=prod_default;//是否把程序烧死

	if((rwReg(BISS_DATA_ADDR +0x42) ==0x61)&&(rwReg(BISS_DATA_ADDR +0x43) ==0x28))
	{
		prod_curr.prod_angle_resolution = bcd_to_hex(rwReg(BISS_DATA_ADDR +0x57)) -5;
//		UARTSendString("prod_curr.prod_angle_resolution");
	}


	databufpt=(ULONG xdata*)((((USHORT)databuf)&0xfffC)+4);
	disp_ang_flag=0;
	dumpdata_uart=0;
	protocol=0;

	ui_state=ST_IDLE;

	#if ASIC==0
	rwReg(REG_CLK_SETUP)=0x78;//
	bitSET32(REG_CLK_SETUP,7);//使能AD采样时钟
	#else

	wReg32(REG_CLK_SETUP,0xb8c0a8);		 //96M/3=32M 32M/40=800K  800K/40=20KHz 
//	wReg32(REG_CLK_SETUP,0xb8c0a0);       //25k
//  wReg32(REG_CLK_SETUP,0xb8c099);      //32 k  
//  wReg32(REG_CLK_SETUP,0xb8c094);		 // 40KHz
 //   wReg32(REG_CLK_SETUP,0xa8c098);    //50Khz
 //   wReg32(REG_CLK_SETUP,0xa8c094);		 // 60KHz   96/2=48M  48M/20=2.4M

	#if USE_EXT_ADC==1
	bitSET32(REG_CLK_SETUP,24);	   	// External ADC使能外部ADC
	#endif

	bitSET32  (REG_BGBUF_CFG,8);
	#if USE_EXT_ADC==1
	fldWRITE32(REG_BGBUF_CFG,3,1,3);	// trim curve
	fldWRITE32(REG_BGBUF_CFG,6,4,3);	// trim bg
	#else
	fldWRITE32(REG_BGBUF_CFG,3,1,7);	// trim curve
	fldWRITE32(REG_BGBUF_CFG,6,4,10);	// trim bg
	#endif


	//fldWRITE32(REG_ADC_CFG1,31,29,6);	// trim res

	bitSET32  (REG_ADC_CFG1,25); 			// sh soft en
	bitCLR32  (REG_ADC_CFG2,11); 			// op En

	fldWRITE32(REG_ADC_CFG2,23,16,0x80);	//dac0

	bitSET32(rwIOMUX_CTRL,31); 				// p2.7

	multi_Init();// set spi1


	#endif

	Init_Dcode();


	#ifdef USETIMER2
	init_timer2();  //中断TF2=0;
	#endif

	UARTSendString("\r\n\r\n");
		//hostif_init_SSI();
		//hostif_init_BISS();
		hostif_init_RS485(0);
 wReg32(rwANGLE_RES,0x0000000b);//包含12位角度信息总共11+5=16位
 
 
// angle_resolution=prod_curr.prod_angle_resolution;
// UARTSendString(angle_resolution);
 
// 	  fldWRITE32(REG_ADC_CFG2,23,16,0x60);
//	  fldWRITE32(REG_BGBUF_CFG,6,0,0x1e);
// 	  bitSET32(REG_ADC_CFG2,0); 	// IA1 high gain
// 		bitSET32(REG_ADC_CFG2,2); 	// IA2 high gain
//		bitSET32(REG_ADC_CFG2,4); 	// IA3 high gain
//		bitSET32(REG_ADC_CFG1,27);//PGA powerdown
//		bitCLR32(REG_ADC_CFG2,11);//op powerdown

		
	while(1)
	{

		if (MACRO_ETIMER1M0_CHECK && disp_ang_flag)
		{
			etimer1_set(ETMCTR(100000));
			Disp_angle();
		}

		// UART Command Processing 
		uart_ui_loop();

		#if 1
		if(start_test)
		{
			start_test = 0;
			multi_turn_test();
		}
		
		//etimer1_delay(100000);
		#endif
	}	

}



unsigned short  test_addr=0;
unsigned short  test_data=0xff00;
unsigned short  last_data;
char last=0;

unsigned short test_cnt=0;
#define TEST_TIMES  1
void multi_turn_test(void)
{
	unsigned short ret;

	if( test_cnt%(TEST_TIMES*2*3) <TEST_TIMES*2)
	 	 test_addr = 0;
	else if( test_cnt%(TEST_TIMES*2*3) <TEST_TIMES*2*2)
		test_addr = 4;
	else
		test_addr = 5;

//	test_data = (test_cnt%TEST_TIMES)*0x1;
	//if(test_cnt%3==0)
	test_data =test_cnt/6;

		

	if(	 test_cnt%2==0)	  //write
	{
		//test_data =~test_data;
		UARTSendString("\r\nwrite, addr:  ");
		UARTSendByte(test_addr);
		last_data =   test_data;
		UARTSendString("data:    ");
		UARTSendInt16(test_data);
		multi_Write(test_addr,test_data);
	}
	else
	{		  
	   	ret=multi_Read(test_addr);
		UARTSendString("            read :  ");
		UARTSendInt16(ret);

		if((ret != 	last_data) ||last)
		//if(ret != 	last_data)
		{
			last ++;
			UARTSendString("!!!!!");
		}
	}

	test_cnt++;
	//test_cnt%= TEST_TIMES*2*3;

	
}




