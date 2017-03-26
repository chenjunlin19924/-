#ifndef _GLOBVAR_H_
#define _GLOBVAR_H_

#undef GLOBAL
#ifdef _CMAIN_
#define GLOBAL 
#else
#define GLOBAL extern
#endif

typedef void code* t_func (void);
typedef void code* t_func_para (BYTE xdata*, BYTE xdata*); 


typedef struct
{
	BYTE prod_class;
	BYTE prod_mode;
	BYTE prod_abs_mode;
	BYTE prod_interface_type;
	BYTE prod_angle_resolution;

} _PROD_CFG;


// ======== ROM utility Pointer ===================

#define NVFlash_Write_add_rom ((t_func_para*) (*( (USHORT code* )0x0104 )))  
#define NVFlash_Write_add_ram ((t_func_para*) (*( (USHORT xdata*)0x4170 )))  
#define Check_DMA ((t_func*) (*( (USHORT code* )0x0112 )))  

#if CSA_TUNNING==1
GLOBAL xdata USHORT csa;
#endif

GLOBAL xdata ULONG databuf[MAXDUMPBUF+1];
GLOBAL ULONG xdata* databufpt;   // used for 4 byte alignment

GLOBAL BYTE ui_state;
GLOBAL BYTE ui_cmd, ui_count, ui_checksum;
GLOBAL USHORT ui_add16,ui_len16;
GLOBAL USHORT xdata ui_loop16;
GLOBAL xdata BYTE ui_ubuf[131];

GLOBAL bit disp_ang_flag;
GLOBAL BYTE xdata dumpdata_uart;

GLOBAL xdata _PROD_CFG	prod_curr;

GLOBAL USHORT  protocol  ; //1 biss  2 ssi  3 TTL  4 tamagawa

#define ST_IDLE				0
#define ST_CHECK_CMD		1
#define ST_WRITE_ONE		0x80
#define ST_WRITE_N			0x81
#define ST_WRITE_N1			0x82
#define ST_READ_ONE			0x90
#define ST_READ_N			0x91
#define ST_RUN				0xA0
#define ST_FLASH			0xB0
#define ST_VERSION			0xC0
#define ST_FBTEST			0xFD
#define ST_TEST				0xFE
#define ST_DUMP				0x10
#define ST_DISP_CONT		0x11
#define ST_PROD_SET			0x12
#define ST_READ_REG32		0x13
#define ST_READ_REG8		0x14
#define ST_WRITE_REG32		0x15
#define ST_WRITE_REG8		0x16
#define ST_READ_SPI			0x17
#define ST_WRITE_SPI		0x18


#define ST_DEBUG1			0x20
#define ST_DEBUG2			0x21
#define ST_DEBUG3			0x22
#define ST_DEBUG4			0x23
#define ST_DEBUG5			0x24	// CSA Tunning
#define ST_DEBUG6			0x25
#define ST_DEBUG7			0x26
#define ST_DEBUG8			0x27
#define ST_DEBUG9			0x28

#define ST_TEST_TIANJI		0x30
#define ST_TEST_TAMAGAWA	0x31
#define ST_TEST_BISS		0x32
#define ST_TEST_TTL		0x33
#define ST_TEST_START_AUTO_SET  0x34
#define ST_TEST_STOP_AUTO_SET  0x35

#define CMD_WRITE_ONE		0x80
#define CMD_WRITE_N			0x81
#define CMD_READ_ONE		0x90
#define CMD_READ_N			0x91
#define CMD_RUN				0xA0
#define CMD_FLASH			0xB0
#define CMD_VERSION			0xC0
#define CMD_FBTEST			0xFD
#define CMD_TEST			0xFE
#define CMD_NOP				0xFF
#define CMD_DUMP			0x10
#define CMD_DISP_CONT		0x11
#define CMD_PROD_SET		0x12
#define CMD_READ_REG32		0x13
#define CMD_READ_REG8		0x14
#define CMD_WRITE_REG32		0x15
#define CMD_WRITE_REG8		0x16
#define CMD_READ_SPI		0x17
#define CMD_WRITE_SPI		0x18
#define CMD_DEBUG1			0x20
#define CMD_DEBUG2			0x21
#define CMD_DEBUG3			0x22
#define CMD_DEBUG4			0x23
#define CMD_DEBUG5			0x24
#define CMD_DEBUG6			0x25
#define CMD_DEBUG7			0x26
#define CMD_DEBUG8			0x27
#define CMD_DEBUG9			0x28

#define CMD_TEST_TIANJI		0x30
#define CMD_TEST_TAMAGAWA	0x31
#define CMD_TEST_BISS		0x32
#define CMD_TEST_TTL		0x33
#define CMD_TEST_START_AUTO_SET  0x34

// Macro for AFE/Modem block
#define AFE_MODE_FINE		0x01
#define AFE_MODE_COARSE_ROT	0x02
#define AFE_MODE_COARSE_LIN	0x03
#define AFE_MODE_FEEDBACK	0x04
#define AFE_MODE_OPTICAL	0x05


// Product configration
#define PROD_CLASS_LINEAR	0x01
#define PROD_CLASS_ROTARY	0x02
#define PROD_CLASS_OPTICAL	0x03
#define PROD_MODE_ABS		0x01
#define PROD_MODE_INC		0x02
#define PROD_ABS_MODE_READBIN	0x01
#define PROD_ABS_MODE_EXT		0x02
#define PROD_ABS_MODE_FINECOAR	0x01
#define PROD_ABS_MODE_MAG		0x02
#define PROD_INTERFACE_BISS		0x00
#define PROD_INTERFACE_TTL		0x01
#define PROD_INTERFACE_485		0x02



#endif 