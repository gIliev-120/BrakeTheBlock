#include <basedef.h>
#include <Board.h>
#include <stdio.h>

#include <lib_at91sam7_pio.h>
#include <lib_at91sam7_pmc.h>
#include <lib_at91sam7_usart.h>

#include <fixedsys.h>
#include <lcd_GE8.h>
int timer =14;
int pointz=0;
Color_t		bckgColor=LCD_COLOR_WHITE; 	 	
int blocks[8][4]={
	{5,0,25,10},
	{30,0,50,10},
	{60,0,80,10},
	{90,0,110,10},
	{5,15,25,25},
	{30,15,50,25},
	{60,15,80,25},
	{90,15,110,25},
};
int x10 = 55;
int x20 = 75;
int yCircle=105;
int game_over = 1;
char array[10] = {0};

#define   cu32BitN(N)        ( (U32) ( 1 << (N) ) )

/*! RXD0 value */
#define   RXD0		(cu32BitN(0))
/*! TXD0 value */
#define   TXD0		(cu32BitN(1))
/*! RTS0 value */
#define   RTS0		(cu32BitN(3))
/*! CTS0 value */
#define   CTS0		(cu32BitN(4))

	int				_row, _col;
	char			c;
	U32				status;
	
	AT91PS_PIO    	pPioA  	= AT91C_BASE_PIOA;
	AT91PS_PMC    	pPMC   	= AT91C_BASE_PMC;
	AT91PS_USART 	pUART0 	= AT91C_BASE_US0;
	
	AT91PS_PIO    p_pPioA  = AT91C_BASE_PIOA;
	AT91PS_PIO    p_pPioB  = AT91C_BASE_PIOB;
	AT91PS_PMC    p_pPMC   = AT91C_BASE_PMC;
	AT91PS_SYS    p_pSys   = AT91C_BASE_SYS;

void UART_WriteString(char *str)
{
	int		i;
	
	for(i=0; i<strlen(str); i++) {
		while (!AT91F_US_TxReady(pUART0));
		AT91F_US_PutChar(pUART0, str[i]);
	}
}



int main()		
{	
	
	
	int i;
	
	/* Initialize the Atmel AT91SAM7X256 (watchdog, PLL clock, default interrupts, etc.) */
	AT91F_LowLevel_Init();

	AT91F_US_Configure(pUART0, MCK, AT91C_US_ASYNC_MODE, 9600L, 0 );
	AT91F_PMC_EnablePeripheralClock(pPMC, 1 << AT91C_ID_US0 );
    AT91F_US_EnableRx(pUART0);
    AT91F_US_EnableTx(pUART0);
	AT91F_PIO_Disable( pPioA, RXD0 | TXD0 | RTS0 | CTS0  );
	AT91F_PIO_A_RegisterSelection( pPioA, RXD0 | TXD0 | RTS0 | CTS0  );	

	//enable the clock of the PIO
	p_pPMC->PMC_PCER = 1 << AT91C_ID_PIOA;
	//enable the clock of the PIO
	p_pPMC->PMC_PCER = 1 << AT91C_ID_PIOB;

	// GPIO init
	p_pPioA->PIO_ODR    = 0xffffffff;   // All as input
	p_pPioB->PIO_ODR    = 0xffffffff;   // All as input
	p_pSys->PIOA_PPUDR  = 0xffffffff;   // Disable Pull-up resistor
	p_pSys->PIOB_PPUDR  = 0xffffffff;   // Disable Pull-up resistor

#define	BIT_SW1 0x01000000
#define	BIT_SW2 0x02000000
#define	BIT_SW3 0x03000000

#define	BIT_JUP 0x0000200
#define	BIT_JDOWN 0x0000100
#define	BIT_JLEFT 0x000080

#define	BIT_JRIGHT 0x0004000
#define	BIT_JPUSH 0x00008000


	// BUTTON SW1
	p_pPioB->PIO_ODR |= BIT_SW1; //Configure in Input
	p_pPioB->PIO_PER |= BIT_SW1; //Enable PB24

	// BUTTON SW2
	p_pPioB->PIO_ODR |= BIT_SW2; //Configure in Input
	p_pPioB->PIO_PER |= BIT_SW2; //Enable PB25
											   
	// BUTTON SW2
	p_pPioB->PIO_ODR |= BIT_SW3; //Configure in Input
	p_pPioB->PIO_PER |= BIT_SW3; //Enable PB25	  PB24

  
	// JOYSTICK PUSH 
	p_pPioA->PIO_ODR |= BIT_JPUSH; //Configure in Input
	p_pPioA->PIO_PER |= BIT_JPUSH; //Enable PA8

	// JOYSTICK LEFT 
	p_pPioA->PIO_ODR |= BIT_JLEFT; //Configure in Input
	p_pPioA->PIO_PER |= BIT_JLEFT; //Enable PA8
	
	// JOYSTICK RIGHT 
	p_pPioA->PIO_ODR |= BIT_JRIGHT; //Configure in Input
	p_pPioA->PIO_PER |= BIT_JRIGHT; //Enable PA8
	
	// JOYSTICK UP 
	p_pPioA->PIO_ODR |= BIT_JUP; //Configure in Input
	p_pPioA->PIO_PER |= BIT_JUP; //Enable PA8

	// JOYSTICK DWON 
	p_pPioA->PIO_ODR |= BIT_JDOWN; //Configure in Input
	p_pPioA->PIO_PER |= BIT_JDOWN; //Enable PA8				 

	/* Init the LCD */
	InitLCD();
	LCD_ClearScreen( LCD_COLOR_WHITE );

	
	/* add your program here ... */
	
	
	/* ... and here */
	LCDSetRect(x10,110,x20,120,1,LCD_COLOR_BLACK);
	
	for(i=0; i<8; i++){
		LCDSetRect(
			blocks[i][0],
			blocks[i][1],
			blocks[i][2],
			blocks[i][3],
			1,
			LCD_COLOR_BLACK
		);
	}
	while (true) {	  
		
		
		

		if(!((p_pPioA->PIO_PDSR) & BIT_JLEFT)){
			if(x10>0){
				LCDSetRect(x10,110,x20,120,1,bckgColor);
				LCDSetRect(x10-5,110,x20-5,120,1,LCD_COLOR_BLACK);
				x10-=5;
				x20-=5;
			}
		}

		if(!((p_pPioA->PIO_PDSR) & BIT_JRIGHT)){
			if(x20<130){
				LCDSetRect(x10,110,x20,120,1,bckgColor);
				LCDSetRect(x10+5,110,x20+5,120,1,LCD_COLOR_BLACK);
				x10+=5;
				x20+=5;	
			}
		}

		if(!((p_pPioA->PIO_PDSR) & BIT_JPUSH)){
			  yCircle=93;						  
			  timer-=1;
			  
			  sprintf(array,"%d ",timer);
			  
			  LCD_WriteString(array, FONT_DESCRIPTOR, 100, 110, LCD_COLOR_BLACK, LCD_COLOR_RED);
				if (timer==0){
				UART_WriteString(array);
				break;
				
				
				}

			  for(;yCircle>-20;yCircle-=5){
			  
			  LCDSetCircle(x10+8,yCircle,5,LCD_COLOR_RED);
			  LCDSetCircle(x10+8,yCircle,4,LCD_COLOR_RED);
			  LCDSetCircle(x10+8,yCircle,3,LCD_COLOR_RED);
			  LCDSetCircle(x10+8,yCircle,2,LCD_COLOR_RED);
			  LCDSetCircle(x10+8,yCircle,1,LCD_COLOR_RED);

			  LCDSetCircle(x10+8,yCircle+10,5,LCD_COLOR_WHITE);
			  LCDSetCircle(x10+8,yCircle+10,4,LCD_COLOR_WHITE);
			  LCDSetCircle(x10+8,yCircle+10,3,LCD_COLOR_WHITE);
			  LCDSetCircle(x10+8,yCircle+10,2,LCD_COLOR_WHITE);
			  LCDSetCircle(x10+8,yCircle+10,1,LCD_COLOR_WHITE);
									  
			  	
			  }
			  

		}



	}
	 			 while(true){
					if(game_over==1){
					 LCD_ClearScreen( LCD_COLOR_WHITE );
					 LCD_WriteString("GAME OVER", FONT_DESCRIPTOR, 11, 10, LCD_COLOR_BLACK, bckgColor);
				 	}
					game_over+=1;
				 }
	/* Actually, the execution never gets here */
	return -1;
}
