#ifndef _SYSCFG_H_
#define _SYSCFG_H_

#define VER_MAIN 	20
#define VER_SUB 	8

// all code and data are linked to 0x500 to 0x2000,except vectors and startup code.
//#pragma USERCLASS (code = MAIN)		 

// ================= Debug switch =====================
#define ASIC 1

#define FEEDBACK_TEST 0
#define PHASECOMPEN 0
#define CSA_TUNNING 0

#define COAR_USE_GMR 0

#define FINE_USE_SQW  1    //1·½²¨§  0ÕýÏÒ
#define USE_EXT_ADC  1   //1 Íâ²¿ADC
#define FLT_MODE	 0		// 0: allwave  1:LPF
#define LPF_CUTOFF   4		// 0:125Hz(NG)  1:250Hz  2:500Hz  3:1000Hz  4:2000Hz  5:4000Hz  6:8000Hz  7:16000Hz
#define TEMP_COMPEN  0  	// 1: enable temperature compensition
#define FINE_COR_REVERSE 1 	// 0: Normal 1: Reversed
#define HIGHVOL      2  	// 1: Use P2.6  2: Use P0.3

    
//#define USETIMER2
//#define SPIFLASH


#define SYSTEM_CLOCK_REF 12000000
#define SYSTEM_CLOCK_PLL 96000000
  
#define UART1BAUD 115200 //256000
#define UART1HWFIFO_LEN 8

// Using 8051 timer

#define TMCTcalculate12(x) (x*(12000000/1000)/1000)/12-3		
#define TMCTcalculate6(x) (x*(6000000/1000)/1000)/12-3		
#define TMCT_6(x) (TMCTcalculate6(x)>0)? TMCTcalculate6(x):0			// Calculate delay count, us->count
#define TMCT_12(x) (TMCTcalculate12(x)>0)? TMCTcalculate12(x):0		// Calculate delay count, us->count

// Using external timer
#define ETMCTR(x) (x*(SYSTEM_CLOCK_REF/1000000))	// Calculate delay count, us->count	

#define MAXDUMPBUF 3800


//#define BISS_DATA_REGISTER10	(0x10000-10*128)

#define BISS_DATA_REGISTER3	(0x10000-4*128)
#define BISS_DATA_REGISTER2	(0x10000-3*128)
#define BISS_DATA_REGISTER1	(0x10000-2*128)
#define BISS_DATA_REGISTER0	(0x10000-1*128)
#define BISS_DATA_ADDR 		(0x10000-128) //last page of flash	  	//top bank and register bank 0
#define BISS_REGISTER_BANK_LEN  64
#define BISS_MAX_REGISTER_BANK  4 //
#define BISS_DATA_MULTITURN_RES_ADDR  0x56
#define BISS_DATA_SINGLETURN_RES_ADDR  0x57
#endif