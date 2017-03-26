; ---- Flag to select Flash Interrupt or RAM interrupt
GLOBFLAG DATA	20H

; ---- Globle bit flags ---------------------------
INTRAM	 	BIT 0x20.0	  ; 1: RAM Int 0: Flash Int
FLASHRAM	BIT 0x20.1	  ;	1: NVFlash_Write_arb call RAM routine 0: use ROM

; ========= Interrupt Flash Enters ===================
 				EXTRN CODE (EXTINT0_FLASH_C)
				EXTRN CODE (TIMER0_FLASH_C)
				EXTRN CODE (EXTINT1_FLASH_C)
				EXTRN CODE (TIMER1_FLASH_C)
				EXTRN CODE (UART0_FLASH_C)
				EXTRN CODE (TIMER2_FLASH_C)
				EXTRN CODE (UART1_FLASH_C)
				EXTRN CODE (EXTINT2_FLASH_C)
				EXTRN CODE (EXTINT3_FLASH_C)
				EXTRN CODE (EXTINT4_FLASH_C)
				EXTRN CODE (EXTINT5_FLASH_C)
				EXTRN CODE (EXTINT6_FLASH_C)

				EXTRN CODE (?C_START)
			    EXTRN IDATA (STACK)
				PUBLIC  STARTUP8 


?RESERV_DATA	SEGMENT DATA			// Reserved data area for boot code
				RSEG	?RESERV_DATA
				DS		10 	

				CSEG	AT 0x8100
								
//				ORG 	0x8000
//FLASHFLAG:		DB 		0x55,0x81,0x7e,0x01

				ORG 	0x8100
STARTUP8:		JMP		STARTUP81

				ORG 	0x8103
			    LJMP    EXTINT0_FLASH_C

				ORG 0x810b
			    LJMP    TIMER0_FLASH_C

				ORG 0x8113
			    LJMP    EXTINT1_FLASH_C

				ORG 0x811b
			    LJMP    TIMER1_FLASH_C

				ORG 0x8123
			    LJMP    UART0_FLASH_C

				ORG 0x812B
			    LJMP    TIMER2_FLASH_C

				ORG 0x8133
			    LJMP    UART1_FLASH_C

				ORG 0x814b
			    LJMP    EXTINT2_FLASH_C

				ORG 0x8153
			    LJMP    EXTINT3_FLASH_C

				ORG 0x815b
			    LJMP    EXTINT4_FLASH_C

				ORG 0x8163
			    LJMP    EXTINT5_FLASH_C

				ORG 0x816b
			    LJMP    EXTINT6_FLASH_C


STARTUP81: 		MOV     SP,#STACK-1
				CLR		A
				MOV		GLOBFLAG,A
				LJMP    ?C_START
				end
