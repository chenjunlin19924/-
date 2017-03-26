#ifndef _FLASHMARK_H_
#define _FLASHMARK_H_

extern code BYTE FLASHFLAG[4];

extern code USHORT vermain;
extern code USHORT versub;

extern code BYTE datestr[];
extern code BYTE timestr[];
extern code BYTE gapcode[230];			  // Fill to 0x80ff

#endif