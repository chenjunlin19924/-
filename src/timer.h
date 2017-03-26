
#ifndef _TIMER_H_
#define _TIMER_H_

//extern void TimerStart(USHORT *clock);
//extern USHORT TimerRead(USHORT *clock);
//extern void ETimerStart(ULONG *clock);
//extern ULONG ETimerRead(ULONG *clock);

//extern void tickinit();	  // Using Timer2 for Tick
extern void delayclk(ULONG dly);	 // 0.01us
extern void etimer1_set(ULONG time);
extern void etimer1_delay(ULONG time);	// us
extern void delay_soft8(BYTE x);
//extern void delay_soft32(ULONG x);
//extern void delay_soft_500us(ULONG x);

extern void init_timer2();
extern void timer2_interrupt();

#endif