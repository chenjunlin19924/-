#ifndef __SPIFLASH__
#define __SPIFLASH__

extern void spiWriteByte(BYTE a);
extern BYTE spiReadByte(void);
extern void spiReadData(long add, USHORT len, BYTE xdata* dout);
extern void spiWriteData(long add, USHORT len, BYTE xdata* din);
extern BYTE spiRDSR(void);
extern void spiWRSR(BYTE sr);
extern void spiErase(long add, BYTE mode);  //  mode: 0: Sector 1: Block
extern void spiBoot(void);



#endif
