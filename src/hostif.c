#include "sysreg.h"
#include "uart.h"
#include "cfg_sys.h"
#include "timer.h"
#include "hostif.h"
#include "globvar.h"
#include "Evatronix\reg515.H"

#define HOSTIF_DEBUG

#ifdef HOSTIF_DEBUG
#define LOG UARTSendString 
#else
#define LOG()
#endif

char xdata hostif_buf[FLASH_PAGE_SIZE];
char xdata gothostcmd=0;
 

//cmd 
//taiji	 	0x32
//tamagawa  [2:0],0x02
//========RS485=====================================================================
//#define RS485_BAUD		115200//1000000
#define RS485_BAUD		2500000//1000000


static unsigned char xdata host_cnt; 
//static unsigned int  xdata host_tmp;

int hostif_init_RS485(char mode)   //0:tamagawa, 1:tianji
{	
	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_NONE);
	if(mode)
		bitSET32(rw32IF_RS485_CFG,pIF_RS485_MODE_TIANJI); //switch to taiji,	
	else
	{
		bitCLR32(rw32IF_RS485_CFG,pIF_RS485_MODE_TIANJI);//switch to tamagawa,
		LOG("hostif 485 tamagawa\r\n");
	}
	//set to iis
	bitCLR32(rw32SYS_CTRL,pSYS_CTRL_IIS_UART2);

	//set baud
	rwReg(rw32IF_RS485_MATCH+0)=(SYSTEM_CLOCK_PLL*16/RS485_BAUD-256) & 0xff;	
	rwReg(rw32IF_RS485_MATCH+1)=(SYSTEM_CLOCK_PLL*16/RS485_BAUD-256)>>8  & 0xff;	
	rwReg(rw32IF_RS485_MATCH+2)=(SYSTEM_CLOCK_PLL*16/RS485_BAUD-256)>>16 & 0xff;	
	rwReg(rw32IF_RS485_MATCH+3)=(SYSTEM_CLOCK_PLL*16/RS485_BAUD-256)>>24 & 0xff;
  
	
	
		
	//我补充的程序
	wReg32(rw32IF_RS485_CFG,0x1010);
	// wReg32(rw32IF_RS485_CFG,0x0020);
	// wReg32(rw32IF_RS485_CFG,0x0030);
	//fldWRITE32(rw32IF_RS485_CFG,pIF_RS485_CFG_ALMC_H,pIF_RS485_CFG_ALMC_L,0x55);///
  fldWRITE32(rw32IF_RS485_CFG,31,28,0x01);

	
	
	//test	almc data
	fldWRITE32(rw32IF_CTRL,pIF_RS485_CFG_ALMC_H,pIF_RS485_CFG_ALMC_L,0x00);

	//bitSET32(rw32IF_CTRL,pIF_CTRL_TRANS_SYS_INTERVENE);	//software provide pos info
	//bitSET32(rw32IF_INT_EN,pIF_INT_REQ_POS);	//
	//EX4=1;	  //host_if_req_position
	
	//cfg 	
	//bitSET32(rw32IF_RS485_CFG,pIF_RS485_CFG_TX_INT_EN);
	bitSET32(rw32IF_RS485_CFG,pIF_RS485_CFG_RX_INT_EN);
	
	//pIF_RS485_CFG_TX_INT_CLR
	bitSET32(rw32IF_INT_EN,pIF_INT_RS485_RX);  //enable rx int

	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_RS485);	//set to rs485 mode
	LOG("hostif 485 init\r\n");
	protocol=4; //485
	return 0;
}

int hostif_RS485_tx(unsigned char *d,int len)
{
	int xdata i;
	for(i=0;i<len;i++)
	{
		rwReg(rw32IF_RS485_TXD)=d[i];
		while(!bitTST32(rw32IF_INT_STATUS,pIF_INT_RS485_TX));
		bitSET32(rw32IF_RS485_CFG,pIF_RS485_CFG_TX_INT_CLR);  
	}
	host_cnt = 0;
	bitSET32(rw32IF_RS485_CFG,pIF_RS485_CFG_HANDOVER); 
	return 0;
}

static void isr_RS485(void)
{
	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_RS485_RX))
	{ 		
	#if 0
		hostif_buf[host_cnt] =rwReg(rw32IF_RS485_RXD);
		//todo ,indicate that data have been received accroding user protocol
		if(hostif_buf[host_cnt]==0x23)
			gothostcmd = 1;
		host_cnt++;
		bitSET32(rw32IF_RS485_CFG,pIF_RS485_CFG_RX_INT_CLR); 
	#else
	//LOG("485 isr\r\n");
	hostif_buf[0] =rwReg(rw32IF_RS485_RXD);
	bitSET32(rw32IF_RS485_CFG,pIF_RS485_CFG_RX_INT_CLR);
	bitSET32(rw32IF_RS485_CFG,pIF_RS485_CFG_HANDOVER);
	#endif
	}
}


//========BISS=====================================================================
#define BISS_SLAVE_ID		0 //3bits
#define BISS_DATA_LEN		0x20
#define BISS_TRANS_LEN		0x22

static unsigned char xdata biss_addr; //def  addr in one bank
static unsigned int  xdata biss_bank_addr;	 //bank addr

static void biss_check_init_reg();
#define BANK_SEL_ADDR  0x40
static BYTE xdata biss_bank;
void biss_write_bankdata();
void set_codec_data(void);

int hostif_init_BISS()
{	 	
	ULONG xdata tmp;

	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_NONE);	
	//set slave id ,use default
	//rwReg(rw32IF_BISS_SLAVE_ID)=BISS_SLAVE_ID;

	rwReg(rw32IF_BISS_DATA_CFG)=0;

	//set to iis
	bitCLR32(rw32SYS_CTRL,pSYS_CTRL_IIS_UART2);

	//set data width
	//rwReg(rw32IF_BISS_DATA_LEN)=  BISS_DATA_LEN;
	//rwReg(rw32IF_BISS_TRANS_LEN)= BISS_TRANS_LEN;

	//data alignment
	//bitSET32(rw32IF_BISS_DATA_CFG,pIF_BISS_DATA_CFG_DATA_ALIGNMENT);

	fldWRITE32(rw32IF_INT_STATUS,pIF_INT_RD,pIF_INT_ADDR,0xf); //write 1 to clear
	fldWRITE32(rw32IF_INT_EN,pIF_INT_RD,pIF_INT_ADDR,0xf);	 //enable int

	//bitSET32(rw32IF_CTRL,pIF_CTRL_TRANS_SYS_INTERVENE);	//software provide pos info
	//bitSET32(rw32IF_INT_EN,pIF_INT_REQ_POS);	//
	//EX4=1;	  //host_if_req_position

	//set mode
	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_BISS);	//set to biss mode
	
	//biss_bank=rwReg(BISS_DATA_ADDR+BANK_SEL_ADDR);

	biss_check_init_reg();
	biss_bank = 0;
	LOG("hostif biss init\r\n");
  protocol=1; //biss
	tmp = 0x800  ;
	wReg32var(rw32IF_BISS_TIMEOUT_CYCLES ,tmp);
	//uart_put_char(0xf0);
	return 0;
}

extern BYTE xdata biss_bank0_def[FLASH_PAGE_SIZE];
static void isr_biss(void)
{
	//int intstatus;
	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_BISS_CTRLD_CRC4B_ERR)) //crc error, reset
	{
		LOG("c\r\n");
		biss_addr= 0;
		host_cnt = 0;

		fldWRITE32(rw32IF_INT_STATUS,pIF_INT_RD,pIF_INT_ADDR,0xf);
	 	return;
	}

	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_ADDR))//addr
	{
		LOG("a\n\r");
		biss_addr=rwReg(rw32IF_BISS_CMD);
		//UARTSendByte(biss_addr&0x7f);

		biss_bank_addr = BISS_DATA_ADDR - biss_bank*FLASH_PAGE_SIZE;

		//if(bitTST32(biss_addr,pIF_BISS_CMD_WR)) //write  // don't use this statement, compile error.
		if(bitTST32(rw32IF_BISS_CMD,pIF_BISS_CMD_WR)) //write
		{
			biss_addr&=0x7f;
			host_cnt = 0;  			
		}
		else  //read
		{
			host_cnt = 0;
			
			//rwReg(rw32IF_BISS_RDATA) = rwReg(biss_bank_addr+biss_addr+host_cnt++);	 //BISS_DATA_ADDR
			if(biss_addr+host_cnt<BISS_REGISTER_BANK_LEN)
				rwReg(rw32IF_BISS_RDATA) = rwReg(biss_bank_addr+biss_addr+host_cnt);
			else if(biss_addr+host_cnt==BISS_REGISTER_BANK_LEN)			
				rwReg(rw32IF_BISS_RDATA) =	biss_bank; 
			else
				rwReg(rw32IF_BISS_RDATA) = rwReg(BISS_DATA_REGISTER0+biss_addr+host_cnt);
	
			host_cnt++;
		}

		bitSET32(rw32IF_INT_STATUS,pIF_INT_ADDR);
	}
	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_WR))//wd
	{
		LOG("w\n\r");
		hostif_buf[biss_addr+host_cnt++]=rwReg(r32IF_BISS_WDATA);
		bitSET32(rw32IF_INT_STATUS,pIF_INT_WR);
	}
	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_WR_LAST))//wd last
	{
		LOG("wl\r\n");
		hostif_buf[biss_addr+host_cnt++]=rwReg(r32IF_BISS_WDATA);

		if(hostif_buf[biss_addr]==0x23)//don't write flash, switch to uart 
		{
			gothostcmd = 1;
		}
		else  if((biss_addr==BANK_SEL_ADDR)&&(host_cnt == 1))
		{
			biss_bank = hostif_buf[biss_addr];//don't write flash
		}
		else
		{
			biss_write_bankdata();
			//todo set codec register 
			set_codec_data();
		}
		
		bitSET32(rw32IF_INT_STATUS,pIF_INT_WR_LAST);
	}
	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_RD))//rd
	{
		LOG("r\n\r");
		if(biss_addr+host_cnt<BISS_REGISTER_BANK_LEN)
			rwReg(rw32IF_BISS_RDATA) = rwReg(biss_bank_addr+biss_addr+host_cnt);
		else if(biss_addr+host_cnt==BISS_REGISTER_BANK_LEN)			
			rwReg(rw32IF_BISS_RDATA) =	biss_bank; 
		else
			rwReg(rw32IF_BISS_RDATA) = rwReg(BISS_DATA_REGISTER0+biss_addr+host_cnt);

		host_cnt++;
		bitSET32(rw32IF_INT_STATUS,pIF_INT_RD);
	}

	return;
}


int hostif_init_TTL()
{	 
	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_NONE);	

	wReg32(rw32IF_TTL_MULTI_PARA,0x10000);
	
	
		/////补充
	wReg32(rw32IF_TTL_REV_PARA,0x78);//1/4脉冲宽度
	wReg32(rw32IF_TTL_ROTARY_PARA,0x00005);//32电周期
	//////////////

	bitSET32(rw32IF_TTL_ROTARY_PARA,pIF_TTL_ROTARY_EN);
	//set mode
	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_TTL);	//set to biss mode
	

	LOG("hostif ttl init\r\n");
	protocol=3; //TTL
	return 0;
}
void isr_pos_max_min()
{
	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_POS_DATA_MAX))
	{
		LOG("max\r\n");
		wReg32(rw32IF_POS_DATA_MAX,0xfffff);
		bitSET32(rw32IF_INT_STATUS,pIF_INT_POS_DATA_MAX);
	}

	if(bitTST32(rw32IF_INT_STATUS,pIF_INT_POS_DATA_MIN))
	{
		LOG("min\r\n");
		wReg32(rw32IF_POS_DATA_MIN,0x00000);
		bitSET32(rw32IF_INT_STATUS,pIF_INT_POS_DATA_MIN);
	}
}
void isr_hostif(void)
{
	if(fldREAD32(rw32IF_INT_STATUS,pIF_INT_RD,pIF_INT_ADDR))
	{
		isr_biss();
	}
	if(fldREAD32(rw32IF_INT_STATUS,pIF_INT_RS485_TX,pIF_INT_RS485_RX))
	{
		isr_RS485();
	}
	if(fldREAD32(rw32IF_INT_STATUS,pIF_INT_POS_DATA_MIN,pIF_INT_POS_DATA_MAX))
	{
	   	isr_pos_max_min();
	}
}
char xdata t=0;
void isr_hostif_pos(void)
{
	//read pos info frome somewhere,then write to 2814 and 2810	   [35:0]
	//todo
	LOG("pos\r\n");
	rwReg(rw32IF_SYS_POS_DATTA_H)=0xf;
	wReg32(rw32IF_SYS_POS_DATTA_L,0x11223344+t);
	t++;
}
int hostif_init_SSI()
{	 	
	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_NONE);	
	wReg32(rw32IF_CTRL,0x0004);
	wReg32(rw32IF_SSI_CTRL,0x0001);//1?úê±?óé?éy??·￠?íêy?Y
	wReg32(rw32IF_SSI_DATA_WIDTH,0x001f);//16??êy?Y
	wReg32(rw32IF_SSI_TIMEOUT_CYCLES,0x07d0);//timerout 20us
	wReg32(rw32IF_SSI_RESET_CYCLES,0x2df0);//í??ú560us???′
	wReg32(rw32IF_CTRL,0x0001);
	fldWRITE32(rw32IF_CTRL,pIF_CTRL_MODE_H,pIF_CTRL_MODE_L,HOSTIF_MODE_SSI);
	LOG("hostif ssi init\r\n");
	protocol=2; //ssi
	return 0;
}

void biss_write_bankdata(void)	 //biss_bank, biss_addr,host_cnt
{

	int xdata tmpaddr;
	int xdata i;
	int xdata max_data_len;
#if 0
	if(biss_bank == 0)
	{			
		max_data_len = 	FLASH_PAGE_SIZE;
		tmpaddr =  BISS_DATA_ADDR;
	}
	else
	{
		max_data_len = 	BISS_REGISTER_BANK_LEN;
		tmpaddr =  BISS_DATA_ADDR - biss_bank*FLASH_PAGE_SIZE;
	}
	for(i=0;i<biss_addr;i++)
		hostif_buf[i]=rwReg(tmpaddr+i);
	
	for(i=biss_addr+host_cnt;i<max_data_len;i++)
		hostif_buf[i]=rwReg(tmpaddr+i);

	NVFlash_Write_add_rom(hostif_buf,(BYTE xdata*) tmpaddr);
#else
	if((biss_bank == 0)||(biss_addr>=BISS_REGISTER_BANK_LEN)||(biss_addr+host_cnt<BISS_REGISTER_BANK_LEN))
	{
		if((biss_bank == 0)||(biss_addr>=BISS_REGISTER_BANK_LEN))
		{
			max_data_len = 	FLASH_PAGE_SIZE;
			tmpaddr = 	 BISS_DATA_REGISTER0;
		}
		else
		{
			max_data_len = 	BISS_REGISTER_BANK_LEN;	
			tmpaddr =  BISS_DATA_ADDR - biss_bank*FLASH_PAGE_SIZE;
		}

		for(i=0;i<biss_addr;i++)
		hostif_buf[i]=rwReg(tmpaddr+i);
	
		for(i=biss_addr+host_cnt;i<max_data_len;i++)
			hostif_buf[i]=rwReg(tmpaddr+i);

		NVFlash_Write_add_rom(hostif_buf,(BYTE xdata*) tmpaddr);
	}
	else
	{		 		
		tmpaddr =  BISS_DATA_ADDR - biss_bank*FLASH_PAGE_SIZE;

		for(i=0;i<biss_addr;i++)
			hostif_buf[i]=rwReg(tmpaddr+i);

		NVFlash_Write_add_rom(hostif_buf,(BYTE xdata*) tmpaddr);

		for(i=0;i<BISS_REGISTER_BANK_LEN;i++)
			hostif_buf[i]=rwReg(BISS_DATA_REGISTER0+i);

		for(i=biss_addr+host_cnt;i<FLASH_PAGE_SIZE;i++)
			hostif_buf[i]=rwReg(BISS_DATA_REGISTER0+i);

		NVFlash_Write_add_rom(hostif_buf,(BYTE xdata*) BISS_DATA_REGISTER0);
	}
#endif
	return ;

}

//char hex_to_bcd(char data)
//{
//
//}
//
char bcd_to_hex(char bcd)
{
	int xdata ret;
	
	ret = (bcd & 0x0f);
	ret += ((bcd>>4)&0x0f)*10 ;
	return   ret ;
}

extern void set_Modem_Regs(void);

void set_codec_data(void)
{
	int xdata i;
	
	//if(biss_bank != 0)
	//	return;

	for(i= biss_addr;i<biss_addr+host_cnt;i++)
	{
		 switch(i){
		 	case  BISS_DATA_MULTITURN_RES_ADDR:
				prod_curr.prod_angle_resolution = 32 - bcd_to_hex(hostif_buf[i]) - 5;
			break;

			case BISS_DATA_SINGLETURN_RES_ADDR:
				prod_curr.prod_angle_resolution = bcd_to_hex(hostif_buf[i]) -5;
			break;
			case   BANK_SEL_ADDR:
				biss_bank= hostif_buf[i];
			break;
			default :
			break;
		 }
	}
	//Init_Dcode();
	//set_Modem_Regs();
	rwReg(rwANGLE_RES) =prod_curr.prod_angle_resolution;
	return;
}


   
BYTE xdata biss_bank0_def[FLASH_PAGE_SIZE]={
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x00
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x10
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x20
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x30
0x00,				//0x40,bank select
0x02,				//0x41,eds,
0x61,	 		//0x42,profile for multiturn resolution
0x28,	 		//0x43,profile for singleturn
0x53,0x5A,0x56,0x05,	 	//0x44-47,binary ,serial number + production no.
0x04,0x44,0x43,			//0x48-4a,bcd,serial number
0x44,0x43,0x02,			//0x4b-4d,bcd,production No
0x06,0x08,0x01,0x00,		//0x4e-51,bcd,dd mm yyyy production date
0x00,0x00,0x10,0x05,		//0x52-55,bcd,article No.
0x12,			//0x56,bcd,multiturn resolution
0x20,			//0x57,bcd,singleturn resolution
0x00,				//0x58,bcd,alignment bits, 
0x20,0x48,			//0x59-5a,bcd,lines per revolution
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x5B,				//0x67,temperature value-64
0x42,				//fault register
0x40,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x44,			//0x78,ASCII product id
0x43,			//0x79,ASCII product id
0x01,			//0x7a,hex product id
0x34,			//0x7b,hex,resoluteon
0x07,			//0x7c,hex,timeout
0x80,			//0x7d,hex, BISS-C
0x00,0x00				//0x7e-0x7f,ASCII,manufacturer code
};

BYTE xdata biss_bank1_def[BISS_REGISTER_BANK_LEN]={
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x00
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x10
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x20
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};
BYTE xdata biss_bank2_def[BISS_REGISTER_BANK_LEN]={
0x01,0x02,0x04,0x07,0xFF,0xFF,0x40,0xFF,   0xFF,0xFF,0xFF,0xFF,0x14,0xFF,0xFF,0xFF,	//0x00
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x10
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x20
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};
BYTE xdata biss_bank3_def[BISS_REGISTER_BANK_LEN]={
0xFF,0xFF,0xFF,0xFF,0x01,0x02,0xFF,0xFF,   0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	//0x00
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x43,	//0x10
0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0x11,0x94,0xFF,0xFF,	//0x20
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

static void biss_check_init_reg()
{
	int xdata i;

	for(i=0;i<FLASH_PAGE_SIZE;i++)
		hostif_buf[i] = 0;

	if((rwReg(BISS_DATA_ADDR +0x42) !=0x61)
	 	||(rwReg(BISS_DATA_ADDR +0x43) !=0x28))
	{
		NVFlash_Write_add_rom(biss_bank0_def,(BYTE xdata*) BISS_DATA_ADDR);
		
		NVFlash_Write_add_rom(biss_bank1_def,(BYTE xdata*) BISS_DATA_REGISTER1);
		NVFlash_Write_add_rom(biss_bank2_def,(BYTE xdata*) BISS_DATA_REGISTER2);
		NVFlash_Write_add_rom(biss_bank3_def,(BYTE xdata*) BISS_DATA_REGISTER3);

	//	for(i=1;i<BISS_MAX_REGISTER_BANK;i++)
	//		NVFlash_Write_add_rom(hostif_buf,(BYTE xdata*) BISS_DATA_ADDR-i*FLASH_PAGE_SIZE);
			
	}
	return;
}

#if 0
BYTE xdata biss_bank2_def_06=0xa0;	//maximum of biss timeout
BYTE xdata biss_bank2_def_0c=0x14;	//longest processing time

BYTE xdata biss_bank3_def_04=0x01;  //feedback bit 1=error=1
BYTE xdata biss_bank3_def_05=0x02;	//feedback bit2=warming=2
BYTE xdata biss_bank3_def_08=0x00;	//encoder type=rotative=0
ULONG xdata biss_bank3_def_1c=0x01;	//crc polynome
ULONG xdata biss_bank3_def_20=0x01;	//crc initial value
USHORT xdata biss_bank3_def_2c=0x01;//maximum mechanical speed
#endif