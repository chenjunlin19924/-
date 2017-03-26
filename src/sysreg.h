#ifndef _SYSREG_H_
#define _SYSREG_H_

#include "cfg_sys.h"

typedef unsigned char BYTE;
typedef unsigned int USHORT;
typedef int SHORT;
typedef unsigned long ULONG;

typedef unsigned char xdata XBYTE;
typedef unsigned int xdata XUSHORT;
typedef int xdata XSHORT;

//typedef void code* t_func (void);
//typedef void code* t_func_para (BYTE xdata*, BYTE xdata*);

typedef union
{
	ULONG u32;
	BYTE u8[4];
} u32_8;

typedef union
{
	USHORT u16;
	BYTE u8[2];
} u16_8;

#define rwReg(x)  (*(( volatile BYTE xdata *)(x)))

#define wReg32(x,d)	 do { \
*((volatile BYTE xdata *)(x)+3)=(d) >>24; \
*((volatile BYTE xdata *)(x)+2)=(d) >>16; \
*((volatile BYTE xdata *)(x)+1)=(d) >>8; \
*((volatile BYTE xdata *)(x))=d; \
} while(0)


#define wReg32var(x,d)	 do { \
*((volatile BYTE xdata *)(x)+3)=*((BYTE*)&d +0); \
*((volatile BYTE xdata *)(x)+2)=*((BYTE*)&d +1); \
*((volatile BYTE xdata *)(x)+1)=*((BYTE*)&d +2); \
*((volatile BYTE xdata *)(x))=  *((BYTE*)&d +3); \
} while(0)


#define wReg16(x,d)	 do { \
*((volatile BYTE xdata *)(x))=d; \
*((volatile BYTE xdata *)(x)+1)=(d)>>8; \
} while(0)

#define wReg16H(x,d)	 do { \
*((volatile BYTE xdata *)(x)+1)=(d)>>8; \
*((volatile BYTE xdata *)(x))=d; \
} while(0)

#define rReg32(x,d)	 do { \
(*((BYTE*)&d +3))=*(((volatile BYTE xdata *)(x))+0); \
(*((BYTE*)&d +2))=*(((volatile BYTE xdata *)(x))+1); \
(*((BYTE*)&d +1))=*(((volatile BYTE xdata *)(x))+2); \
(*((BYTE*)&d +0))=*(((volatile BYTE xdata *)(x))+3); \
} while(0)

#define rReg16(x,d)	 do { \
(*((BYTE*)&d +1))=*((volatile BYTE xdata *)(x)+0); \
(*((BYTE*)&d +0))=*((volatile BYTE xdata *)(x)+1); \
} while(0)

#define rwRegC(x)  (*(( volatile BYTE code *)(x)))

#define bitSET32(x,y) rwReg(x+(y)/8) |= 1<<(y%8)
#define bitCLR32(x,y) rwReg(x+(y)/8) &= ~(1<<(y%8))
#define bitTST32(x,y) (rwReg(x+(y)/8) & (1<<((y)%8)))

#define FLDGEN(x,y) (((1<<(y))-1)^((1<<(x+1))-1))	  // x>y
#define fldWRITE8(add,x,y,d) rwReg(add)=rwReg(add)&	(~FLDGEN((x),(y))) | ((d)<<(y))
#define fldWRITE32(add,x,y,d) rwReg(add+(x)/8)=rwReg(add+(x)/8)&(~FLDGEN(((x)&7),((y)&7))) | ((d)<<((y)&7))	  // CAN NOT cross byte bondry
#define fldREAD8(add,x,y) ((rwReg(add)&(FLDGEN((x),(y))))>>(y))
#define fldREAD32(add,x,y) ((rwReg(add+(x)/8)&(FLDGEN(((x)&7),((y)&7))))>>((y)&7))

#define LOW(x)  (*(((BYTE*)&x)+1))
#define HIGH(x) (*(((BYTE*)&x)+0))

// ================ Registers Base Address =================
#define FLASHSTART			0x8000
#define RAMSTART			0x4000

#define PERIPHOFFSET 		0x3000
#define SYSREGOFFSET 		0x2C00
#define IFREGOFFSET 		0x2800
#define ANAREGOFFSET 		0x2400
#define FLASHCFGOFFSET 		0x2180
#define FLASHOFFSET 		0x2000
#define MODEMDATAOFFSET 	0x0000

#define FLASH_MAGIC			(FLASHSTART+0x0000)
#define FLASH_INT			(FLASHSTART+0x0100)


//======================================= System Register Define ==========================
// ========= 0x2c00 =========
#define rw32CLK_CTRL  		SYSREGOFFSET+0x0000
#define pSYS_CLK	0
#define pUART1		8
#define pUART2		9
#define pI2C		10
#define pTIMER1		11
#define pTIMER2		12
#define pWDT		13
#define pPWM1		14
#define pPWM2 		15
#define pSPIM1		16
//#define pSPIM2		17//removed
#define pMODEM		18
#define pIIF		19
#define pDAC		20
#define pEFC		21
#define pPCLK		22
#define pCKMON_EN		23
#define pCKMON_L		24
#define pCKMON_H		25

// ========= 0x2c04 =========
#define rw32RST_CTRL  		SYSREGOFFSET+0x0004
#define pUART1_RST 	0
#define pUART2_RST	1
#define pI2C_RST  	2
#define pSPIM1_RST	3
#define pSOFT_HRST	4
#define pSPIM2_RST	5
#define pMODEM_RST	6
#define pIIF_RST	7

// ========= 0x2c08 =========
#define rw32SYS_CTRL  		SYSREGOFFSET+0x0008
#define pSTBYEN		0
#define pSTPL		1
#define pSTPH		2
#define pEPBUSY		3
#define pSYS_CTRL_IIS_UART2 16//0:iis, 1:uart2

#define rw32PLL_CTR  		SYSREGOFFSET+0x000C

#define rwOSC_CTRL0  		SYSREGOFFSET+0x0010
#define rwOSC_CTRL1  		SYSREGOFFSET+0x0011

#define rwIOMUX_CTRL  		SYSREGOFFSET+0x0014	   //0-15 mode select  16-31 output enable
/*
	mode 0  mode 1
0	P0_0	TXD2
1	P0_1	RXD2
2	P0_2	SCL
3	P0_3	SDA
4	P0_4	LPM_SCK
5	P0_5	LPM_MOSI
6	P0_6	LPM_MISO
7	P0_7	EXTINT1
8	P2_0	EXTINT2
9	P2_1	EXTINT3
10	P2_2	EXTINT4
11	P2_3	PWM1
12	P2_4	PWM2
13	P2_5	
14	P2_6	-
15	P2_7	-


*/

#define rwPULL_CTRL0  		SYSREGOFFSET+0x0018
#define rwPULL_CTRL1  		SYSREGOFFSET+0x0019

#define rw32ADC_DMA_CTR		SYSREGOFFSET+0x0028
#define pDMAWID		30	// 0: 8bit 1:32bit
#define pDMAEN		31


#define MACRO_set_DMA_ADD(x)		wReg16(rw32ADC_DMA_CTR,x)
#define MACRO_set_DMA_LEN(x)	 do { \
rwReg(rw32ADC_DMA_CTR+2)=x;	\
fldWRITE8(rw32ADC_DMA_CTR+3,5,0,x>>8); \
} while(0)




//=========================== IFREGOFFSET Register Define =================================
#define rw32IF_CTRL				IFREGOFFSET+0x0000
#define pIF_CTRL_MODE_H			2//2-0 ,0:biss,1:ssi,2:rs485,3:ttl
#define pIF_CTRL_MODE_L			0
#define pIF_CTRL_TRANS_SYS_INTERVENE	8
#define rw32IF_DATA_FETCH_BYPASS  	IFREGOFFSET+0x04
#define rw32IF_POS_DATA_MAX			IFREGOFFSET+0x08
#define rw32IF_POS_DATA_MIN			IFREGOFFSET+0x0c
#define rw32IF_SYS_POS_DATTA_L		IFREGOFFSET+0x10	//31:0
#define rw32IF_SYS_POS_DATTA_H		IFREGOFFSET+0x14	//35:32
#define rw32IF_INT_EN				IFREGOFFSET+0x20
#define pIF_INT_ADDR					0
#define pIF_INT_WR						1
#define pIF_INT_WR_LAST					2
#define pIF_INT_RD						3
#define pIF_INT_POS_DATA_MAX			4
#define pIF_INT_POS_DATA_MIN			5
#define pIF_INT_BISS_CTRLD_CRC4B_ERR	6
#define pIF_INT_RS485_RX				8
#define pIF_INT_RS485_TX				9
#define pIF_INT_REQ_POS					16	  
#define rw32IF_INT_STATUS			IFREGOFFSET+0x24	  //write 1 to clear

#define rw32IF_DATA_FETCH_DIV_NUM	IFREGOFFSET+0x40
#define rw32IF_DATA_FETCH_DIV_CLK	IFREGOFFSET+0x44

#define rw32IF_BISS_CTRL			IFREGOFFSET+0x80
#define pIF_BISS_CTRL_EDGE_SEL		0
#define pIF_BISS_CTRL_WARN_BIT		8
#define pIF_BISS_CTRL_ERR_BIT		16
#define rw32IF_BISS_TRANS_LEN		IFREGOFFSET+0x84   //38??иж34??иж30??иж28??иж26??иж22??иж20oбзa18
#define rw32IF_BISS_DATA_LEN		IFREGOFFSET+0x88   //36??иж32??иж28??иж26??иж24??иж20??иж18oбзa16
#define rw32IF_BISS_TIMEOUT_CYCLES	IFREGOFFSET+0x8c   //def 0x800
#define rw32IF_BISS_RESET_CYCLES	IFREGOFFSET+0x90   //def 0x10000
#define rw32IF_BISS_SLAVE_ID	   	IFREGOFFSET+0x94
#define rw32IF_BISS_CMD		  		IFREGOFFSET+0xa0
#define pIF_BISS_CMD_WR				7
#define r32IF_BISS_WDATA	  		IFREGOFFSET+0xa4   //master write data,
#define rw32IF_BISS_RDATA	  		IFREGOFFSET+0xa8   //slave write
#define rw32IF_BISS_DATA_CFG	  	IFREGOFFSET+0xc0
#define pIF_BISS_DATA_CFG_CRC_SEL			0
#define pIF_BISS_DATA_CFG_DATA_ALIGNMENT	8

#define rw32IF_BISS_CRC4B_SCD 	  	IFREGOFFSET+0xc4
#define rw32IF_BISS_CRC6B_SCD	  	IFREGOFFSET+0xc8
#define rw32IF_BISS_4B_CTRLD	  	IFREGOFFSET+0xcc

#define rw32IF_SSI_CTRL				IFREGOFFSET+0x100
#define rw32IF_SSI_DATA_WIDTH	   	IFREGOFFSET+0x104
#define rw32IF_SSI_TIMEOUT_CYCLES	IFREGOFFSET+0x108
#define rw32IF_SSI_RESET_CYCLES	   	IFREGOFFSET+0x10c

#define rw32IF_RS485_CFG	   		IFREGOFFSET+0x140 
#define pIF_RS485_CFG_TX_RESET		0
#define pIF_RS485_CFG_RX_RESET		1
#define pIF_RS485_CFG_PE_SEL_H		3	//parity check ,0:odd,1:even,2:zero,3:one
#define pIF_RS485_CFG_PE_SEL_L		2
#define pIF_RS485_CFG_TX_PE_EN		4
#define pIF_RS485_CFG_RX_PE_EN		5
#define pIF_RS485_CFG_STOP_BIT_SEL	6
#define pIF_RS485_MODE_TIANJI		7	 //1:tianji,0:tamagawa
#define pIF_RS485_CFG_TX_INT_EN		8
#define pIF_RS485_CFG_TX_INT_CLR	9
#define pIF_RS485_CFG_RX_INT_EN		12
#define pIF_RS485_CFG_RX_INT_CLR	13
#define pIF_RS485_CFG_ALMC_H		23	 //23:16,tamagawa almc byte
#define pIF_RS485_CFG_ALMC_L		16
#define pIF_RS485_CFG_HANDOVER		15
#define rw32IF_RS485_MATCH	  		IFREGOFFSET+0x144
#define rw32IF_RS485_TXD			IFREGOFFSET+0x148
#define rw32IF_RS485_RXD			IFREGOFFSET+0x14c

#define rw32IF_TTL_MULTI_PARA		IFREGOFFSET+0x180	//16:0
#define rw32IF_TTL_REV_PARA			IFREGOFFSET+0x184	//12:0
#define rw32IF_TTL_ROTARY_PARA		IFREGOFFSET+0x188	//3:0 rotary small turn num
#define pIF_TTL_ROTARY_EN			8



//====================================== Peripho Register Define ==========================
// ================ Timer Registers =================
#define rw32TIMER1_MATCH0		PERIPHOFFSET+0x0008
#define rw32TIMER1_MATCH1		PERIPHOFFSET+0x000C
#define rw32TIMER1_CTR			PERIPHOFFSET+0x0010
#define pETIMER_CTL_en					0
#define pETIMER_CTL_m0_rst_cnt			1
#define pETIMER_CTL_m1_rst_cnt			2
#define pETIMER_CTL_clr_cnt				3 
#define r32TIMER1_COUNT			PERIPHOFFSET+0x0014

#define rw32TIMER2_MATCH0		PERIPHOFFSET+0x0018
#define rw32TIMER2_MATCH1		PERIPHOFFSET+0x001C
#define rw32TIMER2_CTR			PERIPHOFFSET+0x0020
#define r32TIMER2_COUNT			PERIPHOFFSET+0x0024

// ================ WDT Registers =================
#define rw32WDT_MATCH			PERIPHOFFSET+0x0028
#define rw32WDT_CTL				PERIPHOFFSET+0x002c
#define pWDT_CTL_en					0
#define pWDT_CTL_clr_en				1
#define pWDT_CTL_rst_en				2
#define pWDT_CTL_clr				3

#define r32WDT_CNT				PERIPHOFFSET+0x0030	// 20 bits

// ================ UART1 Registers =================
#define rw32UART1_MATCH			PERIPHOFFSET+0x0040
#define rUART1_RDATA			PERIPHOFFSET+0x0044
#define rwUART1_WDATA			PERIPHOFFSET+0x0048
#define r32UART1_STATUS			PERIPHOFFSET+0x004C
#define rw32UART1_CONTR			PERIPHOFFSET+0x0050
// ================ UART2 Registers =================
#define rw32UART2_MATCH			PERIPHOFFSET+0x0054
#define rUART2_RDATA			PERIPHOFFSET+0x0058
#define rwUART2_WDATA			PERIPHOFFSET+0x005c
#define r32UART2_STATUS			PERIPHOFFSET+0x0060
#define rw32UART2_CONTR			PERIPHOFFSET+0x0064



// ================ Interrupt Registers =================
#define r32PERI_INT_STATUS			PERIPHOFFSET+0x008C
#define w32PERI_INT_CLR				PERIPHOFFSET+0x0090
#define rw32PERI_INT_ENABLE			PERIPHOFFSET+0x0094	  
#define PERI_INT_WDT				13
#define PERI_INT_TMR2_MR1			12
#define PERI_INT_TMR2_MR0			11
#define PERI_INT_TMR1_MR1			10
#define PERI_INT_TMR1_MR0			9
#define PERI_INT_I2C_MASTER			8
#define PERI_INT_UART2_TX			7
#define PERI_INT_UART2_RX			6
//#define PERI_INT_SPIM2_TX			5//removed
//#define PERI_INT_SPIM2_RX			4
#define PERI_INT_SPIM1_TX			3
#define PERI_INT_SPIM1_RX			2
#define PERI_INT_UART1_TX			1
#define PERI_INT_UART1_RX			0



// ================ i2c master Registers =================
#define rw32PERI_I2C_CTL			PERIPHOFFSET+0x00c0
#define	pI2C_CTL_srst				0
#define	pI2C_CTL_en					1

#define rw32PERI_I2C_CMD			PERIPHOFFSET+0x00c4
#define	pI2C_CMD_start				0
#define	pI2C_CMD_stop				1
#define	pI2C_CMD_write				2
#define	pI2C_CMD_read				3
#define	pI2C_CMD_ack				4

#define rw32PERI_I2C_TX_DATA		PERIPHOFFSET+0x00c8
#define r32PERI_I2C_RX_DATA			PERIPHOFFSET+0x00cc
#define rw32PERI_I2C_PRE_SCALE		PERIPHOFFSET+0x00d0	 //16 bits
#define r32PERI_I2C_STATUS			PERIPHOFFSET+0x00d4
#define	pI2C_STATUS_arbitlost		0
#define	pI2C_STATUS_opdone			1
#define	pI2C_STATUS_busy			2

#define rw32PERI_I2C_CHK_POS		PERIPHOFFSET+0x00d8 //16 bits

#if 0 //removed
// ================ gpio master Registers =================
#define rw32PERI_GPIO_DIR			 PERIPHOFFSET+0x154
#define rw32PERI_GPIO_INPUT			 PERIPHOFFSET+0x158
#define rw32PERI_GPIO_SET			 PERIPHOFFSET+0x15c
#define rw32PERI_GPIO_CLR			 PERIPHOFFSET+0x160
#define rw32PERI_GPIO_OUT_STATUS	 PERIPHOFFSET+0x164
//#define rw32PERI_GPIO_EDGE		 	PERIPHOFFSET+0x168
//#define rw32PERI_GPIO_INT_EN			PERIPHOFFSET+0x16c
//#define rw32PERI_GPIO_INT_CLR			PERIPHOFFSET+0x170
//#define rw32PERI_GPIO_INT_STATUS		PERIPHOFFSET+0x174
#endif

// ================ pwm1 master Registers =================
#define rw32PERI_PWM1_MACH0			PERIPHOFFSET+0x180
#define rw32PERI_PWM1_MACH1			PERIPHOFFSET+0x184
#define rw32PERI_PWM1_CNTL			PERIPHOFFSET+0x188
#define pPWM1_CTL_EN				0
#define pPWM1_CTL_PINLEVEL			1

#define rw32PERI_PWM1_COUNTER 		PERIPHOFFSET+0x18c //32 bits

// ================ pwm2 master Registers =================
#define rw32PERI_PWM2_MACH0			PERIPHOFFSET+0x190
#define rw32PERI_PWM2_MACH1			PERIPHOFFSET+0x194
#define rw32PERI_PWM2_CNTL			PERIPHOFFSET+0x198
#define pPWM2_CTL_EN				0
#define pPWM2_CTL_PINLEVEL			1

#define rw32PER2_PWM1_COUNTER 		PERIPHOFFSET+0x19c //32 bits


// ================ spi1 master Registers =================
#define rw32PERI_SPI1_CTL			PERIPHOFFSET+0x0200
#define	pSPI1_CTL_en				0
#define rw32PERI_SPI1_TX_DATA		PERIPHOFFSET+0x0204	//16 bits
#define rw32PERI_SPI1_RX_DATA		PERIPHOFFSET+0x0208	//16 bits
#define rw32PERI_SPI1_STATUS		PERIPHOFFSET+0x020C
#define	pSPI1_STATUS_wfifo_full		1
#define	pSPI1_STATUS_wfifo_empty	2	 
#define rw32PERI_SPI1_CFG			PERIPHOFFSET+0x0210
//0-2 scale
//3-4 mode



// ================  Registers MACROs ===========================
#define MACRO_ETIMER1M0_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,9)
#define MACRO_ETIMER1M1_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,10)
#define MACRO_ETIMER2M0_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,11)
#define MACRO_ETIMER2M1_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,12)
#define MACRO_ETIMER1M0_INT_DIS  	bitCLR32(rw32PERI_INT_ENABLE,9)
#define MACRO_ETIMER1M1_INT_DIS  	bitCLR32(rw32PERI_INT_ENABLE,10)
#define MACRO_ETIMER2M0_INT_DIS  	bitCLR32(rw32PERI_INT_ENABLE,11)
#define MACRO_ETIMER2M1_INT_DIS  	bitCLR32(rw32PERI_INT_ENABLE,12)
#define MACRO_ETIMER1M0_INT_CLR  	bitSET32(w32PERI_INT_CLR,9)
#define MACRO_ETIMER1M1_INT_CLR  	bitSET32(w32PERI_INT_CLR,10)
#define MACRO_ETIMER2M0_INT_CLR  	bitSET32(w32PERI_INT_CLR,11)
#define MACRO_ETIMER2M1_INT_CLR  	bitSET32(w32PERI_INT_CLR,12)
#define MACRO_ETIMER1M0_CHECK    	bitTST32(r32PERI_INT_STATUS,9)
#define MACRO_ETIMER1M1_CHECK    	bitTST32(r32PERI_INT_STATUS,10)
#define MACRO_ETIMER2M0_CHECK    	bitTST32(r32PERI_INT_STATUS,11)
#define MACRO_ETIMER2M1_CHECK    	bitTST32(r32PERI_INT_STATUS,12)

#define MACRO_UART1RX_INT_EN   		bitSET32(rw32PERI_INT_ENABLE,0)
#define MACRO_UART1RX_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,0)
#define MACRO_UART1RX_CHECK    		bitTST32(r32PERI_INT_STATUS,0)
#define MACRO_UART1RX_INT_CLR  		bitSET32(w32PERI_INT_CLR,0)
#define MACRO_UART1TX_INT_EN   		bitSET32(rw32PERI_INT_ENABLE,1)
#define MACRO_UART1TX_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,1)
#define MACRO_UART1TX_CHECK    		bitTST32(r32PERI_INT_STATUS,1)
#define MACRO_UART1TX_INT_CLR  		bitSET32(w32PERI_INT_CLR,1)

#define MACRO_UART2RX_INT_EN   		bitSET32(rw32PERI_INT_ENABLE,6)
#define MACRO_UART2RX_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,6)
#define MACRO_UART2RX_CHECK    		bitTST32(r32PERI_INT_STATUS,6)
#define MACRO_UART2RX_INT_CLR  		bitSET32(w32PERI_INT_CLR,6)
#define MACRO_UART2TX_INT_EN   		bitSET32(rw32PERI_INT_ENABLE,7)
#define MACRO_UART2TX_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,7)
#define MACRO_UART2TX_CHECK    		bitTST32(r32PERI_INT_STATUS,7)
#define MACRO_UART2TX_INT_CLR  		bitSET32(w32PERI_INT_CLR,7)

#define MACRO_SPIM1FULL_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,2)
#define MACRO_SPIM1FULL_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,2)
#define MACRO_SPIM1FULL_CHECK    	bitTST32(r32PERI_INT_STATUS,2)
#define MACRO_SPIM1FULL_INT_CLR  	bitSET32(w32PERI_INT_CLR,2)
#define MACRO_SPIM1EMPTY_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,3)
#define MACRO_SPIM1EMPTY_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,3)
#define MACRO_SPIM1EMPTY_CHECK    	bitTST32(r32PERI_INT_STATUS,3)
#define MACRO_SPIM1EMPTY_INT_CLR  	bitSET32(w32PERI_INT_CLR,3)

#define MACRO_SPIM2FULL_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,4)
#define MACRO_SPIM2FULL_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,4)
#define MACRO_SPIM2FULL_CHECK    	bitTST32(r32PERI_INT_STATUS,4)
#define MACRO_SPIM2FULL_INT_CLR  	bitSET32(w32PERI_INT_CLR,4)
#define MACRO_SPIM2EMPTY_INT_EN   	bitSET32(rw32PERI_INT_ENABLE,5)
#define MACRO_SPIM2EMPTY_INT_DIS   	bitCLR32(rw32PERI_INT_ENABLE,5)
#define MACRO_SPIM2EMPTY_CHECK    	bitTST32(r32PERI_INT_STATUS,5)
#define MACRO_SPIM2EMPTY_INT_CLR  	bitSET32(w32PERI_INT_CLR,5)

#define MACRO_IIC_INT_EN   			bitSET32(rw32PERI_INT_ENABLE,8)
#define MACRO_IIC_INT_DIS   		bitCLR32(rw32PERI_INT_ENABLE,8)
#define MACRO_IIC_CHECK    			bitTST32(r32PERI_INT_STATUS,8)
#define MACRO_IIC_INT_CLR  			bitSET32(w32PERI_INT_CLR,8)


//====================================== Modem Register Define ==========================
#define rwMODEM_CTRL_AW_LP		MODEMDATAOFFSET+0x00		
#define rwMODEM_CTRL_RO_LIN		MODEMDATAOFFSET+0x01
#define rw16MODEM_ADIN_OFF		MODEMDATAOFFSET+0x04
#define rwMONITOR_ADIN_MAX		MODEMDATAOFFSET+0x08
#define rwMONITOR_ADIN_MIN		MODEMDATAOFFSET+0x0C
#define rw32ENCODER_ANGLE_B		MODEMDATAOFFSET+0x10
#define rwMODEM_DMA_SEL			MODEMDATAOFFSET+0x14
#define rw32CORDIC_ANG_OUT		MODEMDATAOFFSET+0x18

#define rwBYPASSAW				MODEMDATAOFFSET+0x20
#define rw32LPF_PARA_1A			MODEMDATAOFFSET+0x40
#define rw32LPF_PARA_1B			MODEMDATAOFFSET+0x44
#define rw32LPF_PARA_1C			MODEMDATAOFFSET+0x48
#define rw32LPF_PARA_2A			MODEMDATAOFFSET+0x4c
#define rw32LPF_PARA_2B			MODEMDATAOFFSET+0x50
#define rw32LPF_PARA_2C			MODEMDATAOFFSET+0x54
#define rw32LPF_PARA_3A			MODEMDATAOFFSET+0x58
#define rw32LPF_PARA_3B			MODEMDATAOFFSET+0x5c
#define rw32LPF_PARA_3C			MODEMDATAOFFSET+0x60

#define rw32LASSA_SIN_OFFSET	MODEMDATAOFFSET+0x80
#define rw32LASSA_COS_OFFSET	MODEMDATAOFFSET+0x84
#define rw32LASSA_START			MODEMDATAOFFSET+0x88
#define rw32LASSA_CLR			MODEMDATAOFFSET+0x8c
#define rw32LASSA_SIN_MAX		MODEMDATAOFFSET+0x90
#define rw32LASSA_SIN_MIN		MODEMDATAOFFSET+0x94
#define rw32LASSA_COS_MAX		MODEMDATAOFFSET+0x98
#define rw32LASSA_COS_MIN		MODEMDATAOFFSET+0x9C

#define rw32ROM_LKUP_INPUT		MODEMDATAOFFSET+0xc0
#define rROM_LKUP_STATUS		MODEMDATAOFFSET+0xc4
#define rw32ROM_LKUP_OUTPUT		MODEMDATAOFFSET+0xc8
#define rwANGLE_RES				MODEMDATAOFFSET+0xcc
#define rwANGLE_QUAD			MODEMDATAOFFSET+0xd0
#define rw32ENCODER_INDEX		MODEMDATAOFFSET+0xd4
#define rw32POSITION_OFFSET		MODEMDATAOFFSET+0xd8
#define rw32POSITION_DATA		MODEMDATAOFFSET+0xdc

#define rwDELAY_COMP_BYPASS		MODEMDATAOFFSET+0xe4
#define rwDELAY_NUM				MODEMDATAOFFSET+0xe8

#define rwMODEM_RAM_ROM_SEL		MODEMDATAOFFSET+0x200
#define rw32DAC_ENABLE			MODEMDATAOFFSET+0xf00
#define rwDAC_SINCOS_PARA_COFF	MODEMDATAOFFSET+0xf08
#define rwDAC_SINCOS_PARA_SOFF	MODEMDATAOFFSET+0xf09
#define rwDAC_SINCOS_PARA_CFACTOR	MODEMDATAOFFSET+0xf0a
#define rwDAC_SINCOS_PARA_SFACTOR	MODEMDATAOFFSET+0xf0b
#define rwDAC_SERIAL_SCALE_CSCALE	MODEMDATAOFFSET+0xf0c
#define rwDAC_SERIAL_SCALE_SSCALE	MODEMDATAOFFSET+0xf0d
#define rw16DAC_DELAY_DAC0		MODEMDATAOFFSET+0xf10
#define rw16DAC_DELAY_DAC1		MODEMDATAOFFSET+0xf12
#define rw16DAC_DELAY_DAC2		MODEMDATAOFFSET+0xf14
#define rw16DAC_DELAY_DAC3		MODEMDATAOFFSET+0xf16
#define rw16DAC_DELAY_DAC4		MODEMDATAOFFSET+0xf18
#define rw16DAC_DELAY_DAC5		MODEMDATAOFFSET+0xf1a
#define rw16DAC_DELAY_DAC6		MODEMDATAOFFSET+0xf1c
#define rw16DAC_DELAY_DAC7		MODEMDATAOFFSET+0xf1e
#define rwDAC0_FACTOR			MODEMDATAOFFSET+0xf20
#define rwDAC1_FACTOR			MODEMDATAOFFSET+0xf24
#define rwDAC2_FACTOR			MODEMDATAOFFSET+0xf28
#define rwDAC3_FACTOR			MODEMDATAOFFSET+0xf2c
#define rwDAC4_FACTOR			MODEMDATAOFFSET+0xf30
#define rwDAC5_FACTOR			MODEMDATAOFFSET+0xf34
#define rwDAC6_FACTOR			MODEMDATAOFFSET+0xf38
#define rwDAC7_FACTOR			MODEMDATAOFFSET+0xf3c

#define rwMODEM_RAM				MODEMDATAOFFSET+0x1000

#define rwSINWAVE_MASTER_ST		MODEMDATAOFFSET+0xd00
#define rwSINCOS_ST				MODEMDATAOFFSET+0xda0

//=================================== Analog Interface ==========================
#if ASIC==0
#define REG_CLK_SETUP			ANAREGOFFSET+0x00
#define REG_GRATING_SEL			ANAREGOFFSET+0x04
#define pAinput_sel		4
#define pGrating_sel_a	3
#define pGrating_sel_b	2
#define pGrating_sel_c	1
#define pGrating_sel_d	0

#define OUTPUT_SEL				ANAREGOFFSET+0x08
#define ADC_DATA				ANAREGOFFSET+0x0C
// ==== AFE Macros ====
//ADCIN_SEL
#define MACRO_ADCin_sel(x) 	(fldWRITE32(OUTPUT_SEL,2,0,x))
#define MADCIN_SEL_PGA	4
#define MADCIN_SEL_BPF	5
#define MADCIN_SEL_SH0	0
#define MADCIN_SEL_SH1	1
#define MADCIN_SEL_SH2	2
#define MADCIN_SEL_SH3	3

// AIN_SEL
#define MACRO_Ain_sel(x) 	(fldWRITE32(OUTPUT_SEL,6,4,x))
#define MAIN_SEL_FINE		0
#define MAIN_SEL_COARSE		1
#define MAIN_SEL_ABS		2

// ABS_SEL
#define MACRO_Abs_sel(x) 	(fldWRITE32(OUTPUT_SEL,10,8,x))

// PGA_SEL
#define MACRO_PGA_set(x) 	(fldWRITE32(OUTPUT_SEL,18,16,x))

// FEEDBACK_SEL
#define MACRO_Ext_fb_sel(x) 	(fldWRITE32(OUTPUT_SEL,14,12,x))

// AINPUT_SEL
#define MACRO_Ainput_sel(x) 	(fldWRITE32(REG_GRATING_SEL,4,4,x))
#define MAINPUT_SEL_NORMAL	 0
#define MAINPUT_SEL_FEEDBACK 1

// Grating
#define MACRO_GRATING_SMP  (rwReg(REG_GRATING_SEL)=rwReg(REG_GRATING_SEL)&0xf0)
#define MACRO_GRATING_HOLD (rwReg(REG_GRATING_SEL)=rwReg(REG_GRATING_SEL)|0x0f)

#else // ASIC==1

//////////////////////  ASIC analog registers ///////////////
#define REG_CLK_SETUP			ANAREGOFFSET+0x00
#define REG_APAMP_CFG1			ANAREGOFFSET+0x04
#define REG_APAMP_CFG2			ANAREGOFFSET+0x08
#define REG_BGBUF_CFG			ANAREGOFFSET+0x0C
#define REG_ADC_CFG1			ANAREGOFFSET+0x10
#define REG_ADC_CFG2			ANAREGOFFSET+0x14
#define REG_ADC_DATA1			ANAREGOFFSET+0x18
#define REG_ADC_DATA2			ANAREGOFFSET+0x1C

// ==== AFE Macros ====
//ADCIN_SEL
#define MACRO_ADCin_sel(x) 	(fldWRITE32(REG_ADC_CFG1,2,0,x))
#define MADCIN_SEL_PGA	1
#define MADCIN_SEL_BPF	5
#define MADCIN_SEL_SH0	0
#define MADCIN_SEL_SH1	1
#define MADCIN_SEL_SH2	2
#define MADCIN_SEL_SH3	3

// AIN_SEL
#define MACRO_Ain_sel(x) 	(fldWRITE32(REG_ADC_CFG1,9,8,x))
#if FINE_COR_REVERSE == 1
#define MAIN_SEL_FINE		1
#define MAIN_SEL_COARSE		0
#else
#define MAIN_SEL_FINE		0
#define MAIN_SEL_COARSE		1
#endif
#define MAIN_SEL_ABS		2

// ABS_SEL
#define MACRO_Abs_sel(x) 	(fldWRITE32(REG_ADC_CFG1,14,12,x))

// PGA_SEL
#define MACRO_PGA_set(x) 	(fldWRITE32(REG_ADC_CFG1,19,16,x))

// FEEDBACK_SEL
#define MACRO_Ext_fb_sel(x) 	(fldWRITE32(REG_ADC_CFG1,7,4,x))

// AINPUT_SEL
//#define MACRO_Ainput_sel(x) 	(fldWRITE32(REG_GRATING_SEL,4,4,x))
//#define MAINPUT_SEL_NORMAL	 0
//#define MAINPUT_SEL_FEEDBACK 1

// Grating
#define MACRO_GRATING_SMP  (bitSET32(REG_ADC_CFG1,24))
#define MACRO_GRATING_HOLD (bitCLR32(REG_ADC_CFG1,24))


#endif



#endif