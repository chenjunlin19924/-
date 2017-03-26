#ifndef _DCODE_H_
#define _DCODE_H_

extern void Dump_Data(USHORT len, BYTE sel, ULONG xdata* pt);
extern void Disp_angle(void);
extern void set_Modem_Regs(void);
extern void Init_Dcode(void);
extern void Init_WaveGen_MasterTable(BYTE type);
extern void Init_WaveGen_SINCOSTable(void);
extern void Init_WaveGen(BYTE mode);		// 0: Normal(90deg phase shift)  1: for Linear ABS det (same phase)
extern BYTE set_Product(_PROD_CFG* pd_cfg);
extern void set_Modem_Filter(BYTE FilterMode);		//FilterMode 1: LPF 0: All Wave
extern void set_LPF_coef(BYTE freq);

extern void set_AFE_Mode(BYTE mode);
extern void set_AFE_Feedback_ch(BYTE ch);
extern void set_AFE_Gain(BYTE g);
extern void set_WaveGen_Rotary(BYTE mode);  // 0: Fine 1: Corse
extern void set_WaveGen_Linear(BYTE mode);  // 0: Fine 1-8: Corse


//extern USHORT ABS_Calculate(ULONG var_C_EA,ULONG var_F_EA );
extern BYTE ABS_Calculate_Rotary(USHORT u_C_EA,USHORT u_F_EA );
extern void ABS_Process_Rotary(void);

extern void ABS_Process_Linear(void);
extern void GRATING_test(void);
extern void GMR_sample(int xdata* gmrx, int xdata* gmry);
extern USHORT GMR_angle(int gmrx,int gmry);
extern BYTE ABS_Calculate_GMR(USHORT gmr,USHORT F_EA16 );
extern void feedbackdump(BYTE ch);
extern void ABS_Process_Linear_test(BYTE i);
extern void Lissa_Calib(void);

extern void asicparameter();
extern void bandgaptest();
extern void temptest();
extern USHORT getTemp();
extern void multi_Init();
extern void multi_Write(BYTE add, USHORT dat);
extern USHORT multi_Read(BYTE add);

extern void errtest();

#endif
