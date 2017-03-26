#ifndef _PRODCFG_H_
#define _PRODCFG_H_


#ifdef _CMAIN_
code _PROD_CFG prod_default={PROD_CLASS_ROTARY,PROD_MODE_ABS,PROD_ABS_MODE_FINECOAR,PROD_INTERFACE_BISS,0x0f}; 
//code _PROD_CFG prod_default={PROD_CLASS_LINEAR,PROD_MODE_ABS,PROD_ABS_MODE_FINECOAR,PROD_INTERFACE_BISS,0x10}; 

#else
extern code _PROD_CFG prod_default;

#endif

#endif