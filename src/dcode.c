/*****************************************************************************************************************
 *
 *  Copyright (c) 2015,MLink-tech Inc.
 *
 *  All Rights Reserved
 *
 *
 *
 *  Filename: dcode.c
 *
 *
 *  State:Draft 
 *  Version:1.0
 *  Author:LYC
 *  Date:   05/25/2015
 *
 * 
 *	 
 *
 ******************************************************************************************************************/	 
//#define _CMAIN_
#include "Evatronix\reg515.H"
#include <intrins.h>
#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"
#include "cfg_hw.h"
#include "cfg_prod.h"

#include "uart.h"
#include "timer.h"
//#include "spiflash.h"
#include "dcode.h"
#include "dcode_tab_master.h"
#include "dcode_tab_sincos.h"
#include <math.h>

#if HIGHVOL==1
sbit P15V=P2^6;
#endif
#if HIGHVOL==2
sbit P15V=P0^3;
#endif

sbit SPISE=P2^7;
#define AFE_FINE_gain  10  /////细圈增益
#define AFE_COARSE_gain  2   /////粗圈增益
#define BOOL 56


code ULONG LPFData[8][9]={
/*   125Hz */0x7ff7a8ff,0xc00852f6,0x00000815,0x7fe940a0,0xc016bb56,0x00000814,0x7fe0f09f,0xc01f0b57,0x00000814,
/*   250Hz */0x7fef4b00,0xc010a4d7,0x00002053,0x7fd28142,0xc02d6e98,0x0000204c,0x7fc1e83b,0xc03e07a1,0x00002048,
/*   500Hz */0x7fde7a0d,0xc0214556,0x0000813b,0x7fa5028f,0xc05abcf1,0x00008101,0x7f83ec5f,0xc07bd331,0x000080e0,
/*  1000Hz */0x7fbc848a,0xc0427943,0x00020466,0x7f4a0579,0xc0b4f93c,0x00020297,0x7f0847fe,0xc0f6b73c,0x0002018e,
/*  2000Hz */0x7f774cbc,0xc084ac93,0x00080d62,0x7e940dc3,0xc167f2ba,0x0007ff07,0x7e1249e5,0xc1e9bab1,0x0007f6d5,
/*  4000Hz */0x7ee7b72b,0xc1083ef0,0x002013c9,0x7d2831a3,0xc2c7fd08,0x001fa2aa,0x7c2b630d,0xc3c4eb91,0x001f62c4,
/*  8000Hz */0x7db45d4b,0xc20c0269,0x007f4097,0x7a510d81,0xc5710932,0x007bd299,0x78714585,0xc751c406,0x0079ece9,
/* 16000Hz */0x7b002e5f,0xc4058d3d,0x01f488c7,0x74a7089e,0xca6b9da7,0x01dab377,0x7146edbb,0xcdd296b7,0x01ccf71c
};


// ========================== Debug Dump Data ====================
//ULONG xdata dumpbuf[2000];
//打印数据

void Dump_Data(USHORT len, BYTE sel, ULONG xdata* pt)
{

	USHORT i;
	ULONG a32;

	//UARTSendString("\r\nDumpData");
	//UARTSendInt16(len);

	//rwReg(REG_CLK_SETUP)=0x78;

	#if ASIC==0
	bitSET32(REG_CLK_SETUP,7);
	#endif

	MACRO_set_DMA_ADD((USHORT)pt-0x4000);

	switch(sel)
	{
		case 00:
			rwReg(rwMODEM_DMA_SEL)=sel;
			i=len-1;
			MACRO_set_DMA_LEN(i);
			bitCLR32(rw32ADC_DMA_CTR,pDMAWID);
		break;

		case 01:
		case 02:
		case 03:
		case 04:
			rwReg(rwMODEM_DMA_SEL)=sel;
			i=(len-1)*4;
			MACRO_set_DMA_LEN(i);
	   		bitSET32(rw32ADC_DMA_CTR,pDMAWID);
		break;

		default:
			return;
		break;
	}
	
		
	Check_DMA();   // Rom code

	//bitSET32(rw32ADC_DMA_CTR,pDMAEN);
	//while (bitTST32(rw32ADC_DMA_CTR,pDMAEN)) ;
	
    if (dumpdata_uart !=0)
	{
		for (i=0;i<len;i++)
		{
			UARTSendString("\r\n");
			if (sel==0)
			{
				 UARTSendByte(*(((BYTE*)pt)+i));
			}
			else
			{
				*(((BYTE*)(&a32))+3) = *(((BYTE*)(pt+i))+0);
				*(((BYTE*)(&a32))+2) = *(((BYTE*)(pt+i))+1);
				*(((BYTE*)(&a32))+1) = *(((BYTE*)(pt+i))+2);
				*(((BYTE*)(&a32))+0) = *(((BYTE*)(pt+i))+3);
				UARTSendInt32(a32);
			}
		}
	}

}

//打印角度值
void Disp_angle(void)
{
	ULONG d,p;

#if TEMP_COMPEN ==1    //测温度
	ULONG tcmp;
	int t;

	t=getTemp();
	t=t-0x3c0;
	tcmp=(long)t*29;
	rwReg(rw32ENCODER_ANGLE_B+2)= *(((BYTE*)&tcmp)+1)&0x0f;
	rwReg(rw32ENCODER_ANGLE_B+1)= *(((BYTE*)&tcmp)+2);
	rwReg(rw32ENCODER_ANGLE_B+0)= *(((BYTE*)&tcmp)+3);
#endif
	
	rReg32(rw32CORDIC_ANG_OUT,d);
	rReg32(rw32POSITION_DATA,p);

	UARTSendString("\r\nCORDIC:");
	UARTSendInt32(d);

	UARTSendString("  POSI:");
	UARTSendInt32(p);

//	t=getTemp();
//	UARTSendString("  T:");
//	UARTSendInt16(t);
///////////////////////
//测量多圈值
	UARTSendString("  M:");
	UARTSendInt16(multi_Read(0));
	UARTSendString(" P:");
	UARTSendInt16(multi_Read(1));
	


}




// ================================= Setup WaveGen =========================
void Init_WaveGen_MasterTable(BYTE type)  // type: 0: sin  1: squarewave  2: squarewave	double f
{
#define SINOFF 0x600
	BYTE i;

	UARTSendString("\r\nInit_WaveGen_MasterTable...");
	UARTSendByte(type);

	if (type==0)
	{
		for (i=0;i<80;i++)
		{
			wReg16(rwSINWAVE_MASTER_ST+i*2,dcode_tab_master[i]+SINOFF);
		}
	}

	if (type==1)
	{
		for (i=0;i<40;i++)
		{
			wReg16(rwSINWAVE_MASTER_ST+i*2,0);
		}
		for (i=40;i<80;i++)
		{
			wReg16(rwSINWAVE_MASTER_ST+i*2,0x8000);
		}
	}

	if (type==2)
	{
		for (i=0;i<20;i++)
		{
			wReg16(rwSINWAVE_MASTER_ST+i*2,0);
			wReg16(rwSINWAVE_MASTER_ST+(i+40)*2,0);
		}
		for (i=20;i<40;i++)
		{
			wReg16(rwSINWAVE_MASTER_ST+i*2,0x8000);
			wReg16(rwSINWAVE_MASTER_ST+(i+40)*2,0x8000);
		}
	}

}

void Init_WaveGen_SINCOSTable(void)
{
	BYTE i;

	UARTSendString("\r\nInit_WaveGen_SINCOSTable...");

	for (i=0;i<80;i++)
	{
		wReg16(rwSINCOS_ST+i*2,dcode_tab_sincos[i]);
	}

}
/////////////////////////////////////////
/////////////////////////////////////////
//细圈和粗圈默认细圈，现在为粗圈模式
/////可关闭载波和设置粗圈或者是细圈的载波
/////////////////////

void set_WaveGen_Rotary(BYTE mode)  // 0: Fine 1: Corse
{

	if (mode==0)
	{
		#if FINE_USE_SQW==0        //正弦
		rwReg(rw32DAC_ENABLE)=0x3c;		// DACEN    0f000   //可以抑制载波0x00//正常0x3c
		rwReg(rw32DAC_ENABLE+2)=0x00;	// DAC MODE 0f02
		rwReg(REG_APAMP_CFG1)=0xc3;		// amppd	2404
		rwReg(REG_APAMP_CFG1+1)=0xc3;	// analog/dig	2405
		#else                       //方波
		rwReg(rw32DAC_ENABLE)=0x3c;		// DACEN    0f00    //可以抑制载波0x00//正常细圈0x3c     //c3粗圈
		rwReg(rw32DAC_ENABLE+2)=0xff;	// DAC MODE 0f02      //正常ff        //抑制细圈0x83   ///抑制粗圈0x3c
		rwReg(REG_APAMP_CFG1)=0xff;		// amppd	2404        //正常ff
		rwReg(REG_APAMP_CFG1+1)=0x3c;	// analog/dig	2405   　//0xc3粗圈 //真正控制关闭载波可以抑制全部载波0x00，//也可以抑制部分载波//细圈0x3c
		#endif
		fldWRITE32(REG_ADC_CFG2,23,16,0x70);
	  fldWRITE32(REG_BGBUF_CFG,6,0,0xdc);
		///////    rwReg(REG_APAMP_CFG1+1)    0xc3粗圈 0x02  4  270°///0x01   3  180°///0x40   1  0°    //// 0x80  2  90°
		///////rwReg(REG_APAMP_CFG1+1)       0x3c细圈 0x08  4   270°///0x04   3  180°///0x10   1   0°  //// 0x20  2   90°
	}
	else
	{
		rwReg(rw32DAC_ENABLE)=0xff;		// DACEN    0f00
		rwReg(rw32DAC_ENABLE+2)=0xc3;	// DAC MODE 0f02
		rwReg(REG_APAMP_CFG1)=0xff;		// amppd	2404
		rwReg(REG_APAMP_CFG1+1)=0xc3;	// analog/dig	2405
				fldWRITE32(REG_ADC_CFG2,23,16,0x65);
	  fldWRITE32(REG_BGBUF_CFG,6,0,0xdc);
	}
	// 内部ADC vref

	
	

	P15V=1;
}


void set_WaveGen_Linear(BYTE mode)  // 0: Fine 1-8: Corse
{
	if (mode==0)
	{
	 	rwReg(REG_APAMP_CFG1)=0xff;		// amppd	2404
		rwReg(REG_APAMP_CFG1+1)=0xff;	// analog/dig	2405
		rwReg(rw32DAC_ENABLE)=0x00;		// DACEN    0f00
		rwReg(rw32DAC_ENABLE+2)=0xff;	// DAC MODE 0f02
	}
	else
	{
		rwReg(REG_APAMP_CFG1)=0xff;		// amppd	2404
		rwReg(REG_APAMP_CFG1+1)=0xff;	// analog/dig	2405
   		rwReg(rw32DAC_ENABLE)=0x00;		// DACEN    0f00

		switch (mode)
		{
			case 1:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x04;	// DAC MODE 0f02
			break;
			case 2:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x08;	// DAC MODE 0f02
			break;
			case 3:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x10;	// DAC MODE 0f02
			break;
			case 4:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x20;	// DAC MODE 0f02
			break;
			case 5:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x01;	// DAC MODE 0f02
			break;
			case 6:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x02;	// DAC MODE 0f02
			break;
			case 7:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x40;	// DAC MODE 0f02
			break;
			case 8:	   // Corse
				rwReg(rw32DAC_ENABLE+2)=0x80;	// DAC MODE 0f02
			break;
		}
	}
}



#if FINE_USE_SQW==0 //正弦
////////////////载波+-sin,+-cos
void Init_WaveGen(BYTE mode)		// 0: Normal(90deg phase shift)  1: for Linear ABS det (same phase)
{
	UARTSendString("\r\nInit_WaveGen...");
	Init_WaveGen_SINCOSTable();


	rwReg(rw16DAC_DELAY_DAC0)=0;
	rwReg(rw16DAC_DELAY_DAC1)=0;
	rwReg(rw16DAC_DELAY_DAC2)=0;
	rwReg(rw16DAC_DELAY_DAC3)=0;
	rwReg(rw16DAC_DELAY_DAC4)=0;
	rwReg(rw16DAC_DELAY_DAC5)=0;
	rwReg(rw16DAC_DELAY_DAC6)=0;
	rwReg(rw16DAC_DELAY_DAC7)=0;

	if (mode==0)
	{
		
//细圈
		rwReg(rw16DAC_DELAY_DAC2+1)=20;
		rwReg(rw16DAC_DELAY_DAC3+1)=0;
		rwReg(rw16DAC_DELAY_DAC4+1)=60;
		rwReg(rw16DAC_DELAY_DAC5+1)=40;
//粗圈	
		rwReg(rw16DAC_DELAY_DAC0+1)=60;
		rwReg(rw16DAC_DELAY_DAC1+1)=40;
		rwReg(rw16DAC_DELAY_DAC6+1)=20;
		rwReg(rw16DAC_DELAY_DAC7+1)=0;

		#if PHASECOMPEN==1
		rwReg(rw16DAC_DELAY_DAC0+0)=10;
		rwReg(rw16DAC_DELAY_DAC1+0)=15;
		rwReg(rw16DAC_DELAY_DAC2+0)=12;
		rwReg(rw16DAC_DELAY_DAC3+0)=10;
		#endif
	}
	else
	{
		rwReg(rw16DAC_DELAY_DAC0+1)=0;
		rwReg(rw16DAC_DELAY_DAC1+1)=0;
		rwReg(rw16DAC_DELAY_DAC2+1)=0;
		rwReg(rw16DAC_DELAY_DAC3+1)=0;
		rwReg(rw16DAC_DELAY_DAC4+1)=0;
		rwReg(rw16DAC_DELAY_DAC5+1)=0;
		rwReg(rw16DAC_DELAY_DAC6+1)=0;
		rwReg(rw16DAC_DELAY_DAC7+1)=0;
	}


	rwReg(rw32DAC_ENABLE+2)=0x00;

	Init_WaveGen_MasterTable(0);
	rwReg(rwDAC2_FACTOR)=0xf0;
	delay_soft8(5);
	rwReg(rwDAC3_FACTOR)=0xf0;
	delay_soft8(5);
	rwReg(rwDAC4_FACTOR)=0xf0;
	delay_soft8(5);
	rwReg(rwDAC5_FACTOR)=0xf0;
	delay_soft8(5);

	Init_WaveGen_MasterTable(1);

	rwReg(rwDAC0_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC1_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC6_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC7_FACTOR)=0xff;

}
#else	//FINE_USE_SQW==1    //方波  
////////////////载波+-sin,+-cos
void Init_WaveGen(BYTE mode)		// 0: Normal(90deg phase shift)  1: for Linear ABS det (same phase)
									// 2: For Linear Fine
{
	UARTSendString("\r\nInit_WaveGen...");
	Init_WaveGen_SINCOSTable();


	rwReg(rw16DAC_DELAY_DAC0)=0;
	rwReg(rw16DAC_DELAY_DAC1)=0;
	rwReg(rw16DAC_DELAY_DAC2)=0;
	rwReg(rw16DAC_DELAY_DAC3)=0;
	rwReg(rw16DAC_DELAY_DAC4)=0;
	rwReg(rw16DAC_DELAY_DAC5)=0;
	rwReg(rw16DAC_DELAY_DAC6)=0;
	rwReg(rw16DAC_DELAY_DAC7)=0;

	if (mode==0)
	{
		
//细圈
		rwReg(rw16DAC_DELAY_DAC2+1)=20;
		rwReg(rw16DAC_DELAY_DAC3+1)=0;
		rwReg(rw16DAC_DELAY_DAC4+1)=60;
		rwReg(rw16DAC_DELAY_DAC5+1)=40;
//粗圈	
		rwReg(rw16DAC_DELAY_DAC0+1)=0;      ///3
		rwReg(rw16DAC_DELAY_DAC1+1)=20;    ///4
		rwReg(rw16DAC_DELAY_DAC6+1)=40;    ///1
		rwReg(rw16DAC_DELAY_DAC7+1)=60;    ///2
//		
//		rwReg(rw16DAC_DELAY_DAC0+1)=60;
//		rwReg(rw16DAC_DELAY_DAC1+1)=40;
//		rwReg(rw16DAC_DELAY_DAC6+1)=40;
//		rwReg(rw16DAC_DELAY_DAC7+1)=0;
		
		
////粗圈
//		rwReg(rw16DAC_DELAY_DAC0+1)=0;
//		rwReg(rw16DAC_DELAY_DAC1+1)=0;
//		rwReg(rw16DAC_DELAY_DAC6+1)=0;
//		rwReg(rw16DAC_DELAY_DAC7+1)=0;
		
		
		#if PHASECOMPEN==1
		rwReg(rw16DAC_DELAY_DAC0+0)=10;
		rwReg(rw16DAC_DELAY_DAC1+0)=15;
		rwReg(rw16DAC_DELAY_DAC2+0)=12;
		rwReg(rw16DAC_DELAY_DAC3+0)=10;
		#endif
	}

	if (mode==1)
	{
		rwReg(rw16DAC_DELAY_DAC0+1)=0;
		rwReg(rw16DAC_DELAY_DAC1+1)=0;
		rwReg(rw16DAC_DELAY_DAC2+1)=0;
		rwReg(rw16DAC_DELAY_DAC3+1)=0;
		rwReg(rw16DAC_DELAY_DAC4+1)=0;
		rwReg(rw16DAC_DELAY_DAC5+1)=0;
		rwReg(rw16DAC_DELAY_DAC6+1)=0;
		rwReg(rw16DAC_DELAY_DAC7+1)=0;
	}

	if (mode==2)
	{
		rwReg(rw16DAC_DELAY_DAC2+1)=60;
		rwReg(rw16DAC_DELAY_DAC3+1)=40;
		rwReg(rw16DAC_DELAY_DAC4+1)=20;
		rwReg(rw16DAC_DELAY_DAC5+1)=0;
		rwReg(rw16DAC_DELAY_DAC0+1)=60;
		rwReg(rw16DAC_DELAY_DAC1+1)=40;
		rwReg(rw16DAC_DELAY_DAC6+1)=20;
		rwReg(rw16DAC_DELAY_DAC7+1)=0;

		#if PHASECOMPEN==1
		rwReg(rw16DAC_DELAY_DAC0+0)=10;
		rwReg(rw16DAC_DELAY_DAC1+0)=15;
		rwReg(rw16DAC_DELAY_DAC2+0)=12;
		rwReg(rw16DAC_DELAY_DAC3+0)=10;
		#endif
	}


	rwReg(rw32DAC_ENABLE+2)=0x00;

	if (mode==1)
		Init_WaveGen_MasterTable(2);
	else
		Init_WaveGen_MasterTable(1);


	rwReg(rwDAC2_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC3_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC4_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC5_FACTOR)=0xff;
	delay_soft8(5);

	rwReg(rwDAC0_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC1_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC6_FACTOR)=0xff;
	delay_soft8(5);
	rwReg(rwDAC7_FACTOR)=0xff;

}


#endif


// ============================ Setup Modem ======================================
void set_LPF_coef(BYTE freq)
{

	wReg32var(rw32LPF_PARA_1A,LPFData[freq][0]);
	wReg32var(rw32LPF_PARA_1B,LPFData[freq][1]);
	wReg32var(rw32LPF_PARA_1C,LPFData[freq][2]);
	wReg32var(rw32LPF_PARA_2A,LPFData[freq][3]);
	wReg32var(rw32LPF_PARA_2B,LPFData[freq][4]);
	wReg32var(rw32LPF_PARA_2C,LPFData[freq][5]);
	wReg32var(rw32LPF_PARA_3A,LPFData[freq][6]);
	wReg32var(rw32LPF_PARA_3B,LPFData[freq][7]);
	wReg32var(rw32LPF_PARA_3C,LPFData[freq][8]);

}

///低通和全波
void set_Modem_Filter(BYTE FilterMode)		//FilterMode 1: LPF 0: All Wave
{
	UARTSendString("\r\nset_Modem_Filter...");

	if (FilterMode==0)
	{
		UARTSendString("AWV");

		rwReg(rwMODEM_CTRL_AW_LP)=0; //关闭光栅
	}
	else
	{
		UARTSendString("LPF");

		set_LPF_coef(LPF_CUTOFF);

		rwReg(rwMODEM_CTRL_AW_LP)=0;
		rwReg(rwMODEM_CTRL_AW_LP)=1;//使能光栅
		set_LPF_coef(LPF_CUTOFF);
	
	} 
}

// =========================== ABS position/Coarse Angle Calculate =========================
//Fine_Index= (C_EA*N + Coarse_Index*360*N + CSA/C_EC*M*N - F_EA*M)/ (360*M)


//计算绝对值公式
BYTE ABS_Calculate_Rotary(USHORT C_EA16,USHORT F_EA16 )
{
	ULONG para_CSA16=0x00000000;//0xeda4;//826;
	BYTE c_idx16,f_idx16,f_idx16_out;
	long a32;
	char rm;
	int calround;


	UARTSendString("\r\nABS_Calculate_Rotary");

#if CSA_TUNNING==1
	UARTSendString("\r\nCSA=");
	UARTSendInt16(csa);
	para_CSA16=csa;
#endif

	f_idx16_out=0;
	for (c_idx16=0;c_idx16<NUM_COARSE;c_idx16++)
	{
		//f_idx=(var_C_EA*para_N + c_idx*para_N - para_CSA*para_M*para_N - var_F_EA*para_M)/ (para_M);
		//a32=((ULONG)C_EA16*(ULONG)para_N16 + (((ULONG)c_idx16*para_N16)<<16)  - (ULONG)para_CSA16*para_M16*para_N16 - (ULONG)F_EA16*para_M16);
		a32=(long)C_EA16*NUM_FINE;
		(*((int*) &a32))+=((long)c_idx16*NUM_FINE);	 	//a32+=((long)c_idx16*NUM_FINE)<<16;
		a32-=(long)para_CSA16;
		a32-=(long)F_EA16*NUM_COARSE;
		if((a32>>15)&(1)==1){
			calround = (*((int*) &a32))+1;}
		  else{
			calround = (*((int*) &a32));
		  }  
		rm= calround % 3;  						//a32=a32>>16;	rm=a32%3;
		f_idx16=(calround/3) & (NUM_FINE-1);  	//f_idx16=(a32/3) & (NUM_FINE-1);

		if (rm==0)
		{
			f_idx16_out=f_idx16;
		}

#if CSA_TUNNING==1
		UARTSendString("\r\nA16:");
		UARTSendInt16(a32);
		UARTSendString("  rm:");
		UARTSendByte(rm);
		UARTSendString("  idx:");
		UARTSendInt16(f_idx16);
#endif

	}

	return(f_idx16_out); 

}

////////计算直线绝对值
#if COAR_USE_GMR==1
BYTE ABS_Calculate_GMR(USHORT gmr,USHORT F_EA16 )
{
	BYTE idx;

	F_EA16=F_EA16>>5;
	gmr -=F_EA16;
	idx=gmr>>11;

	UARTSendString("\r\nIDX=");
	UARTSendByte(idx);
	return(idx);

}
#endif



void ABS_Process_Rotary(void)//计算旋转绝对值
{
#define AVENUM		8
#define AVESHIFT	3
	BYTE i;
	xdata USHORT a16;
	USHORT sum;
	xdata USHORT coar,cfine,idx;
	long mturn;
	#if COAR_USE_GMR==1
	int xdata x,y;
	#endif


	UARTSendString("\r\nABS_Process_Rotary");
	dumpdata_uart=1;

	// Sample Coarse
	#if COAR_USE_GMR==1
	GMR_sample(&x,&y);
	coar=GMR_angle(x,y);
	UARTSendString("\r\nGMR  :");
	UARTSendInt16(coar);

	bitCLR32(OUTPUT_SEL,15);	// Clear ADC Manual
	MACRO_ADCin_sel(4);
	#else
	set_WaveGen_Rotary(1);

	set_AFE_Mode(AFE_MODE_COARSE_ROT);
	//////////////补偿粗圈李萨如圆
   wReg32(rw32LASSA_SIN_OFFSET,0x06710);
	 wReg32(rw32LASSA_COS_OFFSET,0xf6d28);
	etimer1_delay(200000);
	Dump_Data(AVENUM, 2, databufpt );

	sum=0;
	for (i=0;i<AVENUM;i++)
	{
		*(((BYTE*)&a16)+1) = *((BYTE*)(&databufpt[i])+1);
		*(((BYTE*)&a16)+0) = *((BYTE*)(&databufpt[i])+2);
		sum+=a16;
	}
	coar=sum<<1;//计算粗圈角度
	UARTSendString("\r\nCoars:");
	UARTSendInt16(coar);
	#endif


#if 1

	// Sample Fine 

	set_WaveGen_Rotary(0);
//////////////补偿细圈李萨如圆
   wReg32(rw32LASSA_SIN_OFFSET,0);
	 wReg32(rw32LASSA_COS_OFFSET,0);
	set_AFE_Mode(AFE_MODE_FINE);
	etimer1_delay(200000);
	Dump_Data(AVENUM, 2, databufpt );	  // Dump cordic output

	sum=0;
	for (i=0;i<AVENUM;i++)
	{
		*(((BYTE*)&a16)+1) = *((BYTE*)(&databufpt[i])+1);
		*(((BYTE*)&a16)+0) = *((BYTE*)(&databufpt[i])+2);
		sum+=a16;
	}

	cfine=sum<<1;//计算细圈角度
	UARTSendString("\r\nFine:");
	UARTSendInt16(cfine);

	#if COAR_USE_GMR==1
	idx=ABS_Calculate_GMR(coar,cfine);//计算直线绝对值
	#else
	idx=ABS_Calculate_Rotary(coar,cfine);//计算旋转绝对值
	#endif


	UARTSendString("\r\nIDX=");
	UARTSendInt16(idx);

	rwReg(rwANGLE_QUAD)= (*((BYTE*)&cfine))>>6;
	UARTSendString("\r\nQ=");
	UARTSendByte(rwReg(rwANGLE_QUAD));

	rwReg(rw32ENCODER_INDEX+3)=0;
	rwReg(rw32ENCODER_INDEX+2)=0;
	rwReg(rw32ENCODER_INDEX+1)=0;
	rwReg(rw32ENCODER_INDEX+0)=idx;


	rReg32(rw32POSITION_DATA,mturn);
	UARTSendString("\r\nPOSI:");
	UARTSendInt32(mturn);


	HIGH(sum)= *((BYTE*)&mturn+1);
	LOW(sum)= *((BYTE*)&mturn+2);
	UARTSendString("\r\nPOSI16:");
	UARTSendInt16(sum);
	sum=sum>>5;
	UARTSendString("\r\nPOSI16>5:");
	UARTSendInt16(sum);

	if (LOW(sum)<182)
	{
		i=0;
		UARTSendString("\r\nZone0");
	}
	else if (LOW(sum)>198)
	{
		 i=1;
		 UARTSendString("\r\nZone1");
	}
	else
	{
		mturn= multi_Read(1) & 0x03;
		if (mturn==3)
		{
			i=1;
			UARTSendString("\r\nZone3-1");
		}
		else
		{
			i=0;
			UARTSendString("\r\nZone3-0");
		}
	}

	
	mturn= multi_Read(0);
   	mturn=mturn-i;
	mturn=mturn<<5;
	if  ((*((BYTE*)&mturn+1)) & 0x10)
	{
		 *((BYTE*)&mturn+0) =0xff;
		 *((BYTE*)&mturn+1) |=0xf0;
	}

	*((BYTE*)&mturn+3) |= idx;
	
	rwReg(rw32ENCODER_INDEX+3)=*((BYTE*)&mturn+0);
	rwReg(rw32ENCODER_INDEX+2)=*((BYTE*)&mturn+1);
	rwReg(rw32ENCODER_INDEX+1)=*((BYTE*)&mturn+2);
	rwReg(rw32ENCODER_INDEX+0)=*((BYTE*)&mturn+3);

#endif
}

typedef struct
{
	BYTE dacset;
	BYTE headset;

} _abs_lin_type;

code _abs_lin_type abs_lin_tab[40]={
0x01,0,0x02,0,0x03,0,0x04,0,0x05,0,0x06,0,0x07,0,0x08,0,
0x01,1,0x02,1,0x03,1,0x04,1,0x05,1,0x06,1,0x07,1,0x08,1,
0x01,2,0x02,2,0x03,2,0x04,2,0x05,2,0x06,2,0x07,2,0x08,2,
0x01,3,0x02,3,0x03,3,0x04,3,0x05,3,0x06,3,0x07,3,0x08,3,
0x01,4,0x02,4,0x03,4,0x04,4,0x05,4,0x06,4,0x07,4,0x08,4
};


USHORT calcu_amp(USHORT len, BYTE xdata* pt)
{
	USHORT sum,mean,j,amp;

	sum=0;
	for (j=0;j<len;j++)
	{
		sum+=pt[j];
	}
	mean=sum/len;
	//UARTSendString("\r\nmean=");
	//UARTSendInt16(mean);


	sum=0;
	for (j=0;j<len;j++)
	{
		if (pt[j]>mean)
			sum+= (pt[j]-mean);
		else
		    sum+= (mean-pt[j]);
	}
	amp=sum/len;
	UARTSendString("  amp=");
	UARTSendInt16(amp);

	return(amp);
}

//int bdata linenc;

int bdata linenc;
sbit linenc0=linenc^0;
sbit linenc1=linenc^1;
sbit linenc2=linenc^2;
sbit linenc3=linenc^3;
sbit linenc4=linenc^4;
sbit linenc5=linenc^5;
sbit linenc6=linenc^6;
sbit linenc7=linenc^7;
sbit linenc8=linenc^8;
sbit linenc9=linenc^9;
sbit linenc10=linenc^10;
sbit linenc11=linenc^11;
sbit linenc12=linenc^12;
sbit linenc13=linenc^13;
sbit linenc14=linenc^14;
sbit linenc15=linenc^15;

//#define TESTQUAD
void ABS_Process_Linear(void)
{
#define SMPNUM 40
#define AVENUM		8
#define AVESHIFT	3
#define COFF		768
#define IDXOFF		2

	BYTE i;
	BYTE h,d,a;
	BYTE xdata ampbuf[SMPNUM];
	BYTE xdata outbit[20];

	USHORT a16;
	ULONG sum;
	USHORT xdata cfine;
	BYTE xdata cfineh2;
	ULONG xdata posi;


	UARTSendString("\r\nABS_Process_Linear");

	dumpdata_uart=0;
	
	Init_WaveGen(1);
	set_AFE_Mode(AFE_MODE_COARSE_LIN);
	set_AFE_Gain(0x07);

//	bitSET32(OUTPUT_SEL,11);
//	bitSET32(OUTPUT_SEL,7);

	for (i=0;i<40;i++)
	{
		UARTSendString("\r\nCH:>");
		UARTSendByte(i);

		set_WaveGen_Linear(abs_lin_tab[i].dacset);
		MACRO_Abs_sel(abs_lin_tab[i].headset);

		etimer1_delay(2000);
		Dump_Data(SMPNUM, 0, databufpt);
		
		ampbuf[i]=calcu_amp(SMPNUM,(BYTE*) databufpt);
	}

	for (h=0;h<5;h++)
	{
		for (d=0;d<4;d++)
		{
			i=h*8+d;
			if (ampbuf[i]<=ampbuf[i+4])
				a=1;
			else
				a=0;

			outbit[h*4+d]=a;
		}
	}

	for (i=0;i<20;i++)
	{
		UARTSendString("\r\nOutBit");
		UARTSendByte(i);
		UARTSendString("=");
		UARTSendByte(outbit[i]);
	}


#if 1
	// =========== Set to Fine ===================
	Init_WaveGen(2);
	set_AFE_Mode(AFE_MODE_FINE);
	set_AFE_Gain(0x00);

	etimer1_delay(1000);
	Dump_Data(16, 2, databufpt);

/*
codic: |3  2|22221111|1111110|000000000|
codic: |1  4|32109876|5432109|876543210|
a16:        |11111100|0000000|
a16:        |54321098|7654321|
cfine:          |11111100|00000000|
cfine:          |54321098|76543210|

*/

	sum=0;
	for (i=0;i<AVENUM;i++)
	{
		*(((BYTE*)&a16)+1) = *((BYTE*)(&databufpt[i])+1);
		*(((BYTE*)&a16)+0) = *((BYTE*)(&databufpt[i])+2);
		
		sum+=a16;

		//UARTSendString("\r\nbuf:");
		//UARTSendByte(*((BYTE*)(&databuf[i])+3));
		//UARTSendByte(*((BYTE*)(&databuf[i])+2));
		//UARTSendByte(*((BYTE*)(&databuf[i])+1));
		//UARTSendByte(*((BYTE*)(&databuf[i])+0));

	}

	cfine=sum>>AVESHIFT;
	cfine=cfine<<4;
	UARTSendString("\r\nCfine=");
	UARTSendInt16(cfine);

	posi=cfine;

	// =========== Calculate bit serial ===================
	//cfine =~cfine;
	cfine=cfine+COFF;
	//UARTSendString("\r\n~Cfine+OFF=");
	//UARTSendInt16(cfine);

	cfineh2=((cfine>>14) & 3);
	UARTSendString("\r\nCfineh2=");
	UARTSendByte(cfineh2);

	rwReg(rwANGLE_QUAD)=cfineh2;

	#ifdef TESTQUAD
	for (cfineh2=0;cfineh2<4;cfineh2++)
	#endif
	{

	switch(cfineh2)
	{
		case 0:
			linenc15=0;			linenc14=outbit[2];	linenc13=outbit[3];	linenc12=outbit[0];
			linenc11=outbit[6];	linenc10=outbit[7];	linenc9 =outbit[4];	linenc8 =outbit[10];
			linenc7 =outbit[11];linenc6 =outbit[8];	linenc5 =outbit[14];linenc4 =outbit[15];
			linenc3 =outbit[12];linenc2 =outbit[18];linenc1 =outbit[19];linenc0 =outbit[16];
		break;

		case 1:
  			linenc15=0;			linenc14=outbit[3];	linenc13=outbit[0];	linenc12=outbit[1];
			linenc11=outbit[7];	linenc10=outbit[4];	linenc9 =outbit[5];	linenc8 =outbit[11];
			linenc7 =outbit[8];	linenc6 =outbit[9];	linenc5 =outbit[15];linenc4 =outbit[12];
			linenc3 =outbit[13];linenc2 =outbit[19];linenc1 =outbit[16];linenc0 =outbit[17];
		break;

		case 2:
  			linenc15=0;			linenc14=outbit[0];	linenc13=outbit[1];	linenc12=outbit[2];
			linenc11=outbit[4];	linenc10=outbit[5];	linenc9 =outbit[6];	linenc8 =outbit[8];
			linenc7 =outbit[9];	linenc6 =outbit[10];linenc5 =outbit[12];linenc4 =outbit[13];
			linenc3 =outbit[14];linenc2 =outbit[16];linenc1 =outbit[17];linenc0 =outbit[18];

		break;

		case 3:
  			linenc15=0;			linenc14=outbit[1];	linenc13=outbit[2];	linenc12=outbit[3];
			linenc11=outbit[5];	linenc10=outbit[6];	linenc9 =outbit[7];	linenc8 =outbit[9];
			linenc7 =outbit[10];linenc6 =outbit[11];linenc5 =outbit[13];linenc4 =outbit[14];
			linenc3 =outbit[15];linenc2 =outbit[17];linenc1 =outbit[18];linenc0 =outbit[19];

		break;
	}

	UARTSendString("\r\nOut2lkup:");
	UARTSendByte(*(((BYTE*)(&linenc))+1));
	UARTSendByte(*(((BYTE*)(&linenc))+0));

	rwReg(rw32ROM_LKUP_INPUT+1)=*(((BYTE*)(&linenc))+1);
	rwReg(rw32ROM_LKUP_INPUT+0)=*(((BYTE*)(&linenc))+0);

	etimer1_set(ETMCTR(100000));
	while (!MACRO_ETIMER1M0_CHECK)
	{
		if (((rwReg(rROM_LKUP_STATUS))&1) ==1)
		{
			//UARTSendString("\r\nLookup Done!");
			break;
		}
	}

	rReg16(rw32ROM_LKUP_OUTPUT,a16);

	UARTSendString("\r\nIndex=");
	UARTSendInt16(a16);

	a16=a16+IDXOFF;
	a16=a16>>2;

	UARTSendString("\r\nIndex+off/4=");
	UARTSendInt16(a16);


	*(((BYTE*)&posi)+1)=   *(((BYTE*)&a16)+1);
	*(((BYTE*)&posi)+0)=   *(((BYTE*)&a16)+0);
	UARTSendString("\r\nPOSI=");
	UARTSendInt32(posi);
	}

	rwReg(rw32ENCODER_INDEX+3)=0;
	rwReg(rw32ENCODER_INDEX+2)=0;
	rwReg(rw32ENCODER_INDEX+1)=*(((BYTE*)&a16)+0);
	rwReg(rw32ENCODER_INDEX+0)=*(((BYTE*)&a16)+1);;

#endif
}


#if 0
	if(32'hF5555555>=angle_inc_reg&&angle_inc_reg>32'hB5555555)
		signal15bit<={bit1,bit2,bit3,bit5,bit6,bit7,bit9,   bit10,bit11,bit13,bit14,bit15,bit17,bit18,bit19};
	else if(32'hB5555555>=angle_inc_reg&&angle_inc_reg>32'h75555555)
		signal15bit<={bit0,bit1,bit2,bit4,bit5,bit6,bit8,   bit9,bit10,bit12,bit13,bit14,bit16,bit17,bit18};
	else if(32'h75555555>=angle_inc_reg&&angle_inc_reg>32'h35555555)
		signal15bit<={bit3,bit0,bit1,bit7,bit4,bit5,bit11,  bit8,bit9,bit15,bit12,bit13,bit19,bit16,bit17};
	else 
		signal15bit<={bit2,bit3,bit0,bit6,bit7,bit4,bit10,  bit11,bit8,bit14,bit15,bit12,bit18,bit19,bit16};

#endif



void ABS_Process_Linear_test(BYTE i)
{
#define SMPNUM 40
#define AVENUM		8
#define AVESHIFT	3
#define COFF		768
#define IDXOFF		2

//	BYTE i;
//	BYTE h,d,a;
	BYTE xdata ampbuf[SMPNUM];
//	BYTE xdata outbit[20];

//	USHORT a16;
//	ULONG sum;
//	USHORT xdata cfine;
//	BYTE xdata cfineh2;
//	ULONG xdata posi;


	UARTSendString("\r\nABS_Process_Linear_Test");

	dumpdata_uart=0;
	
	Init_WaveGen(1);
	set_AFE_Mode(AFE_MODE_COARSE_LIN);
	set_AFE_Gain(0x07);

//	bitSET32(OUTPUT_SEL,11);
//	bitSET32(OUTPUT_SEL,7);

//	for (i=0;i<40;i++)
	{
		UARTSendString("\r\nCH:>");
		UARTSendByte(i);

		rwReg(rw32DAC_ENABLE)=abs_lin_tab[i].dacset;
		MACRO_Abs_sel(abs_lin_tab[i].headset);

		etimer1_delay(1000);
		Dump_Data(SMPNUM, 0, databufpt);
		
		ampbuf[i]=calcu_amp(SMPNUM,(BYTE*) databufpt);
	}

	#if 0
	for (h=0;h<5;h++)
	{
		for (d=0;d<4;d++)
		{
			i=h*8+d;
			if (ampbuf[i]<=ampbuf[i+4])
				a=1;
			else
				a=0;

			outbit[h*4+d]=a;
		}
	}

	for (i=0;i<20;i++)
	{
		UARTSendString("\r\nOutBit");
		UARTSendByte(i);
		UARTSendString("=");
		UARTSendByte(outbit[i]);
	}
	#endif
}

/////////////李萨如圆补偿
void set_Modem_Regs(void) 
{

	UARTSendString("\r\nset_Modem_Regs");

	if (prod_curr.prod_class==PROD_CLASS_LINEAR)
	{
		rwReg(rwMODEM_CTRL_RO_LIN)=0;
	}
	else
	{
		rwReg(rwMODEM_CTRL_RO_LIN)=0;
	}

	rwReg(rwANGLE_RES) =prod_curr.prod_angle_resolution;

	wReg32(rw32POSITION_OFFSET,0);
	
	wReg16(rw16MODEM_ADIN_OFF,0x8000);
	rwReg(rwMONITOR_ADIN_MAX)=0;
	rwReg(rwMONITOR_ADIN_MIN)=0xff;
	wReg32(rw32ENCODER_ANGLE_B,0);
	rwReg(rwBYPASSAW)=0;
//李萨如圆补偿
//	wReg32(rw32LASSA_SIN_OFFSET,0x0d3);
//	wReg32(rw32LASSA_COS_OFFSET,0x00a3e);
//	wReg32(rw32LASSA_SIN_OFFSET,0);
//	wReg32(rw32LASSA_COS_OFFSET,0);
	wReg32(rw32LASSA_SIN_OFFSET,0x000);
	wReg32(rw32LASSA_COS_OFFSET,0x000);

	rwReg(rwDELAY_COMP_BYPASS)=1;

}


BYTE set_Product(_PROD_CFG* pd_cfg)
{

	UARTSendString("\r\n\nProduct Setup...");
	// ========== Linear ========
	if (pd_cfg->prod_class==PROD_CLASS_LINEAR)
	{
		UARTSendString("\r\n PROD_CLASS_LINEAR");
		if (pd_cfg->prod_mode==PROD_MODE_ABS)
		{
			UARTSendString("\r\n  PROD_MODE_ABS");
			if (pd_cfg->prod_abs_mode==PROD_ABS_MODE_READBIN)
			{
				UARTSendString("\r\n  PROD_ABS_MODE_READBIN");
			
			}
			else if (pd_cfg->prod_abs_mode==PROD_ABS_MODE_EXT) 
			{
				UARTSendString("\r\n  PROD_ABS_MODE_EXT");


			}
			else
			{
				UARTSendString("\r\n  Invalid ABS!!");
				return(0xff);
			}

		}
		else if (pd_cfg->prod_mode==PROD_MODE_INC)
		{
			UARTSendString("\r\n  PROD_MODE_INC");


		}
		else
		{
			UARTSendString("\r\n  Invalid Mode!!");
			return(0xff);


		}


	}
	// ========== Rotary ========
	else if (pd_cfg->prod_class==PROD_CLASS_ROTARY)
	{
		UARTSendString("\r\n PROD_CLASS_ROTARY");
		if (pd_cfg->prod_mode==PROD_MODE_ABS)
		{
			UARTSendString("\r\n  PROD_MODE_ABS");
			if (pd_cfg->prod_abs_mode==PROD_ABS_MODE_FINECOAR)
			{
				UARTSendString("\r\n  PROD_ABS_MODE_FINECOAR");
			
			}
			else if (pd_cfg->prod_abs_mode==PROD_ABS_MODE_MAG) 
			{
				UARTSendString("\r\n  PROD_ABS_MODE_MAG");

			}
			else
			{
				UARTSendString("\r\n  Invalid ABS!!");
				return(0xff);
			}

		}
		else if (pd_cfg->prod_mode==PROD_MODE_INC)
		{
			UARTSendString("\r\n  PROD_MODE_INC");

		}
		else
		{
			UARTSendString("\r\n  Invalid Mode!!");
			return(0xff);


		}


	}
	else if (pd_cfg->prod_class==PROD_CLASS_OPTICAL)
	{
		UARTSendString("\r\n  PROD_CLASS_OPTICAL");

	}
	else
	{
		UARTSendString("\r\n  Invalid Type!!");
		return(0xff);
	}

//	// ==== Set Filter
//	set_Modem_Filter(0);		//FilterMode 1: LPF 0: All Wave

	return(0);
}

// ================ AFE Setup Code ======================================

void set_AFE_Mode(BYTE mode)
{
	UARTSendString("\r\nset_AFE_Mode");
	//rwReg(REG_CLK_SETUP)=0x78;
	//bitSET32(REG_CLK_SETUP,7);

	etimer1_delay(1000);
//mode =  AFE_MODE_COARSE_ROT;
	switch (mode)
	{
		case AFE_MODE_FINE:     //细圈
			set_AFE_Gain(AFE_FINE_gain);               //////////放大倍数
			UARTSendString("\r\n AFE_MODE_FINE");
			MACRO_Ext_fb_sel(0);
			MACRO_Ain_sel(MAIN_SEL_FINE);////MAIN_SEL_FINE=1
			#if ASIC==0
			MACRO_Ainput_sel(MAINPUT_SEL_NORMAL);
			#endif
			MACRO_ADCin_sel(MADCIN_SEL_PGA);
			bitSET32(REG_ADC_CFG2,1); 	// IA1 PD
			bitSET32(REG_ADC_CFG2,2); 	// IA2 high gain
			bitCLR32(REG_ADC_CFG2,3); 	// IA2 PowerOn
		

		
		break;

		case AFE_MODE_COARSE_ROT:  //粗圈
			set_AFE_Gain(AFE_COARSE_gain);               //////////放大倍数
			UARTSendString("\r\n AFE_MODE_COARSE_ROT");
			MACRO_Ext_fb_sel(0);                              //d_muxb=1
			MACRO_Ain_sel(MAIN_SEL_COARSE);
			#if ASIC==0
			MACRO_Ainput_sel(MAINPUT_SEL_NORMAL);
			#endif
			MACRO_ADCin_sel(MADCIN_SEL_PGA);                ///d_muxa=1
			bitSET32(REG_ADC_CFG2,3); 	// IA2 PD
			bitSET32(REG_ADC_CFG2,0); 	// IA1 high gain
			bitCLR32(REG_ADC_CFG2,1); 	// IA1 PowerOn
	

		break;

		case AFE_MODE_COARSE_LIN:
			UARTSendString("\r\n AFE_MODE_COARSE_LIN");
			MACRO_Ain_sel(MAIN_SEL_ABS);
			#if ASIC==0
			MACRO_Ainput_sel(MAINPUT_SEL_NORMAL);
			#endif
			MACRO_ADCin_sel(MADCIN_SEL_PGA);
			MACRO_Abs_sel(0);
		break;

		case AFE_MODE_FEEDBACK:
			UARTSendString("\r\n AFE_MODE_FEEDBACK");
			MACRO_Ain_sel(MAIN_SEL_FINE);
			#if ASIC==0
			MACRO_Ainput_sel(MAINPUT_SEL_FEEDBACK);
			#endif
			MACRO_ADCin_sel(MADCIN_SEL_PGA);
			MACRO_Ext_fb_sel(0);
		break;

		case AFE_MODE_OPTICAL:
			UARTSendString("\r\n AFE_MODE_OPTICAL");
			MACRO_Abs_sel(0);

		break;

	}

}

#if 0
void set_AFE_Feedback_ch(BYTE ch)
{
	UARTSendString("\r\nAFE_FB:");
	UARTSendByte(ch);
	MACRO_Ext_fb_sel(ch);
}
#endif

//设置第二级放大器
void set_AFE_Gain(BYTE g)
{
	UARTSendString("\r\nAFE_Gain:");
	UARTSendByte(g);
	MACRO_PGA_set(g);
	bitSET32(REG_ADC_CFG2,0); 	// IA1 high gain

}

//extern BYTE test1;
// =============== Init Product ======================================
//初始化编码器设置
void Init_Dcode(void)
{
	UARTSendString("\r\nInit_Dcode...");

	wReg32(REG_APAMP_CFG1,0x7c0ffff);//apamp 96m/960=100K运放

	set_Modem_Regs();     

	set_Modem_Filter(FLT_MODE);	// 0: AllWave
	Init_WaveGen(0);

//	set_AFE_Mode(AFE_MODE_FINE);
//	set_AFE_Gain(AFE_FINE_gain);               //////////放大倍数

	MACRO_GRATING_HOLD;
	wReg32(REG_APAMP_CFG2,0);
	fldWRITE32(REG_APAMP_CFG2,10,8,7);		//trim bias//偏置设定
	rwReg(REG_APAMP_CFG2+2)=0x80;			// offsetdac


	if (prod_curr.prod_class==PROD_CLASS_ROTARY)
	{
		ABS_Process_Rotary();
	}
	else
	{
		ABS_Process_Linear();
	}

	// BISS init
//	rwReg(0x2884)=0x22;
//	rwReg(0x2888)=0x20;
//	rwReg(0x28c0)=0x1;
//	rwReg(0x28c8)=0;
//	rwReg(0x28c9)=3;
//	rwReg(0x28ca)=1;
//	rwReg(0x2800)=0;


}

#if FEEDBACK_TEST==1
void feedbackdump(BYTE ch)
{

	set_AFE_Mode(AFE_MODE_FEEDBACK);

	dumpdata_uart=1;

	set_AFE_Gain(0);

	
	MACRO_Ext_fb_sel(ch);
	etimer1_delay(1000);
	UARTSendString("\r\nFEEDBACK_ADC_SAMP\r\n");
	UARTSendByte(ch);
	Dump_Data(160, 3, databufpt);

	UARTSendString("\r\nDumpDone\r\n");
}
#endif

#if COAR_USE_GMR==1//直线
void GMR_sample(int xdata* gmrx, int xdata* gmry)
{
#if 0
#define _UARTSendString(x) UARTSendString(x)
#define _UARTSendInt16(x)  UARTSendInt16(x)
#else
#define _UARTSendString(x)
#define _UARTSendInt16(x)
#endif
	USHORT x;
	USHORT gmr[4];
	BYTE i;
	
	_UARTSendString("\r\nGMR_sample");
	
	#if ASIC==0
	bitSET32(OUTPUT_SEL,15);	// Set ADC Manual
	#endif

	MACRO_GRATING_SMP;
 	delay_soft8(15);
	MACRO_GRATING_HOLD;

	for (i=0;i<4;i++)
	{
		MACRO_ADCin_sel(i);
 		delay_soft8(10);
		//bitSET32(ADC_DATA,0);
 		delay_soft8(100);
		//HIGH(x)=rwReg(ADC_DATA+1);
		//LOW(x)=rwReg(ADC_DATA);
		gmr[i]=x;
		_UARTSendString("\r\nGRAT:");
   	 	_UARTSendInt16(x);
	}

	*gmrx=(int)(gmr[0]-gmr[1]);
	*gmry=(int)(gmr[2]-gmr[3]);
	_UARTSendString("\r\nx:");
   	_UARTSendInt16(*gmrx);
	_UARTSendString("\r\ny:");
   	_UARTSendInt16(*gmry);
}

//直线角度
USHORT GMR_angle(int gmrx,int gmry)
{
	float xdata a;
	USHORT aint;

	a=atan2(gmry,gmrx);
	if (a<0)
		a=a+6.28318530;

	a=a*10430.37835;
	aint=(USHORT)(a+0.5);
	aint=~aint;
	UARTSendString("\r\nAngle:");
    UARTSendInt16(aint);

    return(aint);
}
#endif


////李萨如圆
#if 0
void Lissa_Calib(void)
{
	ULONG tmp;

	UARTSendString("\r\nLissa_Calib:");

	bitSET32(rw32LASSA_CLR,0);
	bitCLR32(rw32LASSA_CLR,0);

	bitSET32(rw32LASSA_START,0);
	etimer1_delay(1000000);
	bitCLR32(rw32LASSA_START,0);

	rReg32(rw32LASSA_SIN_MAX,tmp);
	UARTSendString("\r\nsinMax:");
    UARTSendInt32(tmp);
	rReg32(rw32LASSA_SIN_MIN,tmp);
	UARTSendString("\r\nsinMin:");
    UARTSendInt32(tmp);
	rReg32(rw32LASSA_COS_MAX,tmp);
	UARTSendString("\r\ncosMax:");
    UARTSendInt32(tmp);
	rReg32(rw32LASSA_COS_MIN,tmp);
	UARTSendString("\r\ncosMin:");
    UARTSendInt32(tmp);
}
#endif

#if 0
void GRATING_test(void)
 {
	USHORT x;

	bitSET32(OUTPUT_SEL,15);	// Set ADC Manual

	MACRO_GRATING_SMP;
 	delay_soft8(15);
	MACRO_GRATING_HOLD;

	MACRO_ADCin_sel(0);
 	delay_soft8(10);
	bitSET32(ADC_DATA,0);
 	delay_soft8(100);
	HIGH(x)=rwReg(ADC_DATA+1);
	LOW(x)=rwReg(ADC_DATA);
	UARTSendString("\r\n\r\nGRAT:");
    UARTSendInt16(x);

	MACRO_ADCin_sel(1);
 	delay_soft8(10);
	bitSET32(ADC_DATA,0);
 	delay_soft8(100);
	HIGH(x)=rwReg(ADC_DATA+1);
	LOW(x)=rwReg(ADC_DATA);
	UARTSendString("\r\nGRAT:");
    UARTSendInt16(x);

	MACRO_ADCin_sel(2);
 	delay_soft8(10);
	bitSET32(ADC_DATA,0);
 	delay_soft8(100);
	HIGH(x)=rwReg(ADC_DATA+1);
	LOW(x)=rwReg(ADC_DATA);
	UARTSendString("\r\nGRAT:");
    UARTSendInt16(x);

	MACRO_ADCin_sel(3);
 	delay_soft8(10);
	bitSET32(ADC_DATA,0);
 	delay_soft8(100);
	HIGH(x)=rwReg(ADC_DATA+1);
	LOW(x)=rwReg(ADC_DATA);
	UARTSendString("\r\nGRAT:");
    UARTSendInt16(x);


}
#endif

#if 0
void bandgaptest()//基准
{
	USHORT bi;
	MACRO_ADCin_sel(5);

   	dumpdata_uart=1;
	for (bi=0;bi<0x100;bi+=2)
	{
		rwReg(REG_BGBUF_CFG)=bi;

		UARTSendString("\r\nBG=");	
		UARTSendByte(bi);	
		etimer1_delay(100000L);
		Dump_Data(1024,3,databufpt);
	}
}
#endif



#if 1
void asicparameter()  ////ASIC参数
{
	USHORT carrier,ia,d_mux,d_mux1,pga,op,gain,DAC0,VREF,resolution;
	

	carrier		=	rwReg(REG_APAMP_CFG1+1);//载波
//	UARTSendString("\r\ncarrier=");
	UARTSendByte(carrier);	
	
	ia		=	rwReg(REG_ADC_CFG2);
//	UARTSendString("\r\nia=");
	UARTSendByte(ia);	
	
	DAC0	=	rwReg(REG_ADC_CFG2+2);
//	UARTSendString("\r\nDAC0=");
	UARTSendByte(DAC0);	
	
	op		=	rwReg(REG_ADC_CFG2+1);
//	UARTSendString("\r\nop=");
	UARTSendByte(op);	
	
	pga		=	rwReg(REG_ADC_CFG1+3);
//	UARTSendString("\r\npga=");
	UARTSendByte(pga);	
	
	d_mux1=	rwReg(REG_ADC_CFG1+1);
//	UARTSendString("\r\nd_mux1=");
	UARTSendByte(d_mux1);	
	
	d_mux	=	rwReg(REG_ADC_CFG1);
//	UARTSendString("\r\nd_mux=");
	UARTSendByte(d_mux);	
	
	gain	=	rwReg(REG_ADC_CFG1+2);
//	UARTSendString("\r\ngain=");
	UARTSendByte(gain);	
	
	VREF	=	rwReg(REG_BGBUF_CFG);
//	UARTSendString("\r\nVREF=");
	UARTSendByte(VREF);	
	
	resolution	=	rwReg(rwANGLE_RES);//分辨率
//	UARTSendString("\r\nresolution=");
	UARTSendByte(resolution);	
	
//	if(protocol==1) UARTSendString("\r\nbiss");
//	if(protocol==2) UARTSendString("\r\nssi");	
//	if(protocol==3) UARTSendString("\r\nTTL");
//	if(protocol==4) UARTSendString("\r\ntamagawa");
	UARTSendByte(protocol);	//协议
	UARTSendByte(BOOL);     //标志位
}

#endif















void errtest()
{
#define LEN 0x0e00
	USHORT i;
	USHORT amax,amin;
	ULONG a32;

   	dumpdata_uart=0;
	//UARTSendString("\r\nError Test...");	
	Dump_Data(LEN,2,databufpt);

	amax=0;
	amin=0xffff;
	for (i=1;i<LEN;i++)
	{
		*(((BYTE*)(&a32))+3) = *(((BYTE*)(databufpt+i))+0);
		*(((BYTE*)(&a32))+2) = *(((BYTE*)(databufpt+i))+1);
		*(((BYTE*)(&a32))+1) = *(((BYTE*)(databufpt+i))+2);
		*(((BYTE*)(&a32))+0) = *(((BYTE*)(databufpt+i))+3);

		
		
		if (a32>amax)
			amax=a32;
		if (a32<amin)
			amin=a32;
	}

	UARTSendString("\r\nM=");	
	UARTSendInt16(amax);	
	UARTSendString("  N=");	
	UARTSendInt16(amin);	
	UARTSendString("  ER=");	
	UARTSendInt16(amax-amin);	


}




#if TEMP_COMPEN==1     //测温度
USHORT getTemp()
{
	USHORT temp,tmin,tmax;
	unsigned long tsum;
	USHORT i;

	tmin=0xffff;
	tmax=0;
	tsum=0;
	
	for (i=0;i<66;i++)
	{
		bitSET32(REG_ADC_DATA1,16);
//		_nop_();
//		_nop_();
//		_nop_();
//		_nop_();
//		etimer1_delay(10);
		delay_soft8(10);

		HIGH(temp)=rwReg(REG_ADC_DATA1+3);
		LOW(temp)=rwReg(REG_ADC_DATA1+2);

		temp=temp>>4;
		tsum+= temp;

		if (temp>tmax)
			tmax=temp;
		if (temp<tmin)
			tmin=temp;

//		etimer1_delay(10);

	}
	tsum=tsum-tmin-tmax;
	temp=tsum>>6;

//	for (i=0;i<256;i++)
//	{
//		UARTSendString("\r\nTT=");	
//		UARTSendInt16(databufpt[i]);	
//		etimer1_delay(1000L);
//	}



   	return(temp);
}
#endif

#if 0
void temptest()
{
	USHORT ct,temp;

	for (ct=0;ct<16;ct++)
	{
	    temp=getTemp();
		UARTSendString("\r\nTP=");	
		UARTSendInt16(temp);	
		etimer1_delay(1000L);
	}
}
#endif

void multi_Init()
{
	fldWRITE32(rw32PERI_SPI1_CFG,2,0,6);	// spi1 clock scale   PERIPHOFFSET  0x30+0x210
	fldWRITE32(rw32PERI_SPI1_CFG,4,3,0);	// spi1 mode

	bitSET32(rwIOMUX_CTRL,4); 				// set spi1
	bitSET32(rwIOMUX_CTRL,5); 				// set spi1
	bitSET32(rwIOMUX_CTRL,6); 				// set spi1

	SPISE=0;//P2^7
}


void multi_Write(BYTE add, USHORT dat)
{
	SPISE=1;
	delay_soft8(250);
	
	rwReg(rw32PERI_SPI1_TX_DATA)=add;
	rwReg(rw32PERI_SPI1_TX_DATA+1)=0x80;
	rwReg(rw32PERI_SPI1_CTL)=1;
	delay_soft8(250);	delay_soft8(250);//delay_soft8(250);delay_soft8(250);

	rwReg(rw32PERI_SPI1_TX_DATA)=LOW(dat);
	rwReg(rw32PERI_SPI1_TX_DATA+1)=HIGH(dat);
	rwReg(rw32PERI_SPI1_CTL)=1;
	delay_soft8(250); delay_soft8(250);	 //delay_soft8(250);
	SPISE=0;
}

USHORT multi_Read(BYTE add)
{
	USHORT r;

	SPISE=1;
	delay_soft8(250);

	rwReg(rw32PERI_SPI1_TX_DATA)=add;
	rwReg(rw32PERI_SPI1_TX_DATA+1)=0x00;
	rwReg(rw32PERI_SPI1_CTL)=1;

	delay_soft8(250);//delay_soft8(250);
	delay_soft8(250);//delay_soft8(250);

	rwReg(rw32PERI_SPI1_CTL)=1;
	delay_soft8(250); //delay_soft8(250);
	delay_soft8(250); // delay_soft8(250);

	HIGH(r)=rwReg(rw32PERI_SPI1_RX_DATA+1);
	LOW(r)=rwReg(rw32PERI_SPI1_RX_DATA);

	SPISE=0;
	return(r);
}