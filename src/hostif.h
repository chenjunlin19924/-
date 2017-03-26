#ifndef _HOSTIF_H
#define	_HOSTIF_H



enum {	HOSTIF_MODE_BISS=0,
	 	HOSTIF_MODE_SSI,
		HOSTIF_MODE_RS485,
		HOSTIF_MODE_TTL,
		HOSTIF_MODE_NONE
	}; //biss :0

#define FLASH_PAGE_SIZE  128

extern char xdata hostif_buf[FLASH_PAGE_SIZE];
extern char xdata gothostcmd;

int hostif_init_RS485(char mode);
int hostif_RS485_tx(unsigned char *d,int len);

int hostif_init_BISS();
int hostif_init_TTL();
int hostif_init_SSI();

void isr_hostif(void);
void isr_hostif_pos(void);


#endif