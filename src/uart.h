
#ifndef _UART_H_
#define _UART_H_

extern void uart_init(void);
extern void uart_put_char(BYTE c);
//extern void IRQ_Uart(void);
extern BYTE uart_check(void);
extern BYTE uart_read(void);

extern void UARTSendString(char *ptr);
extern void UARTSendByte(BYTE c);
extern void UARTSendInt16(USHORT c);
extern void UARTSendInt32(unsigned long c);
extern void UARTSendInt16DEC(int c);
extern void UARTSendByteDec(BYTE c);

#endif