#include "Evatronix\reg515.H"
#include "sysreg.h"
#include "cfg_sys.h"	
#include "globvar.h"

#include "uart.h"
#include "timer.h"
#include "flashmark.h"
#include "spiflash.h"
#include "dcode.h"
#include "hostif.h"

//extern void spitest(void);
//extern void spiWriteByte(BYTE a);



#define _UARTSendString(x)

extern xdata ULONG databuf[];
extern char start_test;


void uart_ui_loop(void)
{
	USHORT i;
	ULONG xdata tmp32;
//	int xdata x,y;

	
	// ================ UART Command Processing =========================
		switch (ui_state)
	   	{
	   		case ST_IDLE:
				if (uart_check() != 0)
				{
					 ui_state=ST_CHECK_CMD;

				}	
				break;
		  	// ================ Check Command ==================
			case ST_CHECK_CMD:
				if (uart_check() != 0)
				{
					ui_cmd=uart_read();
					ui_state=ui_cmd;
					ui_count=0;
					
				}
				else
				{
					ui_state=ST_IDLE;
				}

				break;

			// ============================= FLASH WRITE ===============
			case ST_FLASH:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 131)
					{

						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						ui_checksum=0x55;
						for (ui_count=0;ui_count<130;ui_count++)
						{
							ui_checksum=ui_checksum ^ ui_ubuf[ui_count];
						}

						if (ui_checksum != ui_ubuf[130])
						{
							uart_put_char(0xfd);	// ui_checksum error
							ui_state=ST_IDLE;
							break;
						}

						if ((ui_add16 & 0x7F) != 0)
						{
							uart_put_char(0xfe);	// address error
							ui_state=ST_IDLE;
							break;
						}

						NVFlash_Write_add_rom(&ui_ubuf[2],(BYTE xdata*) ui_add16);
						//NVFlash_Write_add_ram(&ui_ubuf[2],(BYTE xdata*) ui_add16);
						
						for (ui_count=0;ui_count<128;ui_count++)
						{
							if (ui_ubuf[ui_count+2] != (rwReg(ui_add16+ui_count)))
							{
								uart_put_char(0xfc);	// flash write error
								ui_state=ST_IDLE;
								break;
							}
						}

						if (ui_count==128)
						{
						//	uart_put_char(0x00);	// flash write done
							UARTSendString("\r\nFDone!");

						}

						ui_state=ST_IDLE;
					}
				}
			break;

	   		
			// ====================== Write One ==================
			case ST_WRITE_ONE:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 3)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						rwReg(ui_add16)=ui_ubuf[2];
						ui_state=ST_IDLE;
					}
				}

			break;

			// ============================= Write N ===============
			case ST_WRITE_N:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 4)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						ui_len16=ui_ubuf[2]<<8 | ui_ubuf[3];
						ui_state=ST_WRITE_N1;
					}
				}
			break;

			case ST_WRITE_N1:
				if (uart_check() != 0)
				{
					ui_count=uart_read();
					rwReg(ui_add16)=ui_count;
					ui_add16++;
					ui_len16--;

					if (ui_len16 == 0)
					{
						ui_state=ST_IDLE;
					}
				}
			break;

			// ============================= Read One ===============
			case ST_READ_ONE:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 2)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						uart_put_char(rwReg(ui_add16));
						ui_state=ST_IDLE;
					}
				}
			break;

			// ============================= Read N ===============
			case ST_READ_N:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 4)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						ui_len16=ui_ubuf[2]<<8 | ui_ubuf[3];
						//if (ui_len16 != 0)
						{
							while(ui_len16 != 0)
							{
								uart_put_char(rwReg(ui_add16));
								ui_add16++;
								ui_len16--;
							}
						}
						ui_state=ST_IDLE;
					}
				}
			break;

			// ============================= RUN ===============
			case ST_RUN:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 2)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						((void(code *)(void)) ui_add16) ();
						ui_state=ST_IDLE;
					}
				}
			break;


			// ============================= Version Info ===============
			case ST_VERSION:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 1)
					{
						if (ui_ubuf[0]==0)
						{
							uart_put_char((vermain>>8)&0xff);
							uart_put_char(vermain&0xff);


							errtest();
						}
						
						if (ui_ubuf[0]==1)
						{
							uart_put_char((versub>>8)&0xff);
							uart_put_char(versub&0xff);
						}
						
						if (ui_ubuf[0]==2)
						{
							for (ui_count=0;ui_count<8;ui_count++)
								uart_put_char(datestr[ui_count]);
						}
						
						if (ui_ubuf[0]==3)
						{
							for (ui_count=0;ui_count<8;ui_count++)
								uart_put_char(timestr[ui_count]);
						}
							
						ui_state=ST_IDLE;
					}
				}
			break;

			// ============================= Dump Data ===============
			case ST_DUMP:		//10
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 5)
					{
						ui_len16=ui_ubuf[1]*256+ui_ubuf[2];
						ui_loop16=ui_ubuf[3]*256+ui_ubuf[4];
						
						ui_add16=ui_len16;
						if (ui_ubuf[0]==0)
						   ui_add16=ui_add16>>2;
						if (ui_add16<=MAXDUMPBUF)
						{
							for (i=0;i<ui_loop16;i++)
							{
								dumpdata_uart=1;
								Dump_Data(ui_len16,ui_ubuf[0],databufpt);
								dumpdata_uart=0;
							}
						}
						else
						{
						 	UARTSendString("\r\nTooLong!");
						}
						ui_state=ST_IDLE;
					}
				}
			break;

			// ============================= Display data continue ===============
			case ST_DISP_CONT:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 1)
					{
						if (ui_ubuf[0]==0)
						{
							disp_ang_flag=0;
						}
						if (ui_ubuf[0]==1)
						{
							disp_ang_flag=1;
						}
						ui_state=ST_IDLE;
					}

				}
			break;
			// ============================= Product Setup ===============
			case ST_PROD_SET:
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 3)
					{
						prod_curr.prod_class=ui_ubuf[0];
						prod_curr.prod_mode=ui_ubuf[1];
						prod_curr.prod_abs_mode=ui_ubuf[2];
						ui_checksum=set_Product(&prod_curr);
						//uart_put_char(ui_checksum);
						Init_Dcode();
						ui_state=ST_IDLE;
					}
				}
			break;


			case ST_READ_REG32:			// 0x13
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 2)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						rReg32(ui_add16,tmp32); 

						UARTSendString("\r\nR:");
						UARTSendInt16(ui_add16);
						UARTSendString("=");
						UARTSendInt32(tmp32);

						ui_state=ST_IDLE;
					}
				}
			break;

			case ST_READ_REG8:			// 0x14
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 2)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						UARTSendString("\r\nR:");
						UARTSendInt16(ui_add16);
						UARTSendString("=");
						UARTSendByte(rwReg(ui_add16));

						ui_state=ST_IDLE;
					}
				}
			break;


			case ST_WRITE_REG32:			// 0x15
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 6)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						
						wReg32var(ui_add16,*((ULONG*)&ui_ubuf[2]));
						UARTSendString("\r\nWrite:");
						UARTSendInt16(ui_add16);
						UARTSendString("<");
						UARTSendInt32(*((ULONG*)&ui_ubuf[2]));

						ui_state=ST_IDLE;
					}
				}
			break;

			case ST_WRITE_REG8:			// 0x16
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 3)
					{
						ui_add16=ui_ubuf[0]<<8 | ui_ubuf[1];
						
						rwReg(ui_add16)=ui_ubuf[2];
						UARTSendString("\r\nWrite:");
						UARTSendInt16(ui_add16);
						UARTSendString("<");
						UARTSendByte(ui_ubuf[2]);

						ui_state=ST_IDLE;
					}
				}
			break;

			case ST_READ_SPI:			// 0x17
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 1)
					{
						UARTSendString("\r\nRead:");
						UARTSendByte(ui_ubuf[0]);
						UARTSendString(":");

						i=multi_Read(ui_ubuf[0]);

						UARTSendInt16(i);

						ui_state=ST_IDLE;
					}
				}
			break;

			case ST_WRITE_SPI:			// 0x18
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 3)
					{

						UARTSendString("\r\nWrite:");
						UARTSendByte(ui_ubuf[0]);
						UARTSendString("<");
						UARTSendByte(ui_ubuf[1]);
						UARTSendByte(ui_ubuf[2]);

						LOW(i)=ui_ubuf[2];
						HIGH(i)=ui_ubuf[1];
						multi_Write(ui_ubuf[0],i);

						ui_state=ST_IDLE;
					}
				}
			break;
				
				
			case ST_DEBUG1:		//0x20
				//bandgaptest();
				ui_state=ST_IDLE;
			break;
			case ST_DEBUG2:		//0x21
				//mlttest();
			  asicparameter();
				ui_state=ST_IDLE;
			break;

			case ST_TEST_TIANJI:
				hostif_init_RS485(1);	//tianji
				_UARTSendString("\ntianji");
				ui_state=ST_IDLE;
			break;
			case ST_TEST_TAMAGAWA:
				hostif_init_RS485(0);  //tamagawa
				_UARTSendString("\ntamagawa");
				ui_state=ST_IDLE;
			break;
			case ST_TEST_BISS:
				 hostif_init_BISS();
				_UARTSendString("\nbiss");
			   #if 0
				delay_soft_500us(2);
				//position data max min int test
				bitSET32(rw32IF_INT_EN,pIF_INT_POS_DATA_MAX);	
				bitSET32(rw32IF_INT_EN,pIF_INT_POS_DATA_MIN);	//
				rwReg(rwANGLE_RES)=5;
				delay_soft_500us(2);
				wReg32(rw32IF_POS_DATA_MAX,0xd0000);
				wReg32(rw32ENCODER_INDEX,0xff000000);
				delay_soft_500us(2);
				wReg32(rw32IF_POS_DATA_MIN,0xc0000);
				wReg32(rw32ENCODER_INDEX,0x00000000);
			  #endif


				ui_state=ST_IDLE;
			break;
			case ST_TEST_TTL:
				 hostif_init_TTL();
				_UARTSendString("\nttl");
				ui_state=ST_IDLE;
			break;
			case ST_TEST_START_AUTO_SET:
				 start_test = 1;
				 ui_state=ST_IDLE;
			break;
			case ST_TEST_STOP_AUTO_SET:
				start_test = 0;
				ui_state=ST_IDLE;
			break;

			case ST_DEBUG9:		//0x25
				if (uart_check() != 0)
				{
					ui_ubuf[ui_count]=uart_read();
					ui_count++;

					if (ui_count == 1)
					{
						ABS_Process_Linear_test(ui_ubuf[0]);
						ui_state=ST_IDLE;
					}
				}
			break;

			default:
				 ui_state=ST_IDLE;
			break;


	   	}
}

