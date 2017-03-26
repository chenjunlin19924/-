#include "Evatronix\reg515.H"
#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"

code BYTE FLASHFLAG[4]={0x55,0x81,0x7e,0x01};
//code BYTE FLASHFLAG[4]={0x00,0x00,0x00,0x00};

code USHORT vermain=VER_MAIN;
code USHORT versub=VER_SUB;

code BYTE datestr[]=__DATE2__;
code BYTE timestr[]=__TIME__;
code BYTE gapcode[230];			  // Fill to 0x80ff