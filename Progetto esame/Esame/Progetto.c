/* PWM Pins: 	
							PA0(A0)		controlled by TIM2
							PA1(A1)		controlled by TIM3
							PA4(A2)		controlled by TIM4
							PA5(D13)	controlled by TIM5
*/
//



//GPIOA  	boundary address is 0x40020000
//USART2 	boundary address is 0x40004400
//RCC    	boundary address is 0x40023800
//TIM2 		boundary address is 0x40000000
//TIM3 		boundary address is 0x40000400
//TIM4 		boundary address is 0x40000800
//TIM5 		boundary address is 0x40000C00


#include <stdio.h>


#define RCC_APB1ENR *(long*) 	0x40023840				//RCC APB1 enable register
#define RCC_AHB1ENR *(long*) 	0x40023830       	//RCC AHB1 enable register
#define GPIOA_MODER *(long*) 	0x40020000       	//GPIOA mode register
#define GPIOA_ODR		*(long*) 	0x40020014				//GPIOA Output data register
#define GPIOA_AFRL  *(long*) 	0x40020020       	//GPIOA AF register low 
#define USART2_CR1  *(long*) 	0x4000440C       	//USART2 control register 1
#define USART2_BRR  *(long*) 	0x40004408       	//USART2 baud rate register 
#define USART2_SR   *(long*) 	0x40004400       	//USART2 status register 
#define USART2_DR   *(long*) 	0x40004404       	//USART2 data register
#define NVIC_ISER0	*(long*)	0xE000E100				//NVIC ISER0
#define NVIC_ISER1	*(long*) 	0xE000E104				//NVIC ISER1 
#define MCUID1      *(long*) 	0x1FFF7A10       	//ID register 1
#define MCUID2      *(long*) 	0x1FFF7A14       	//ID register 2
#define MCUID3      *(long*) 	0x1FFF7A18       	//ID register 3
	
//Timers Control Registers
#define TIM2_CR1 *(long*) 0x40000000
#define TIM3_CR1 *(long*)	0x40000400
#define TIM4_CR1 *(long*) 0x40000800
#define TIM5_CR1 *(long*)	0x40000C00

//Timers Status Registers
#define TIM2_SR *(long*) 	0x40000010
#define TIM3_SR *(long*)	0x40000410
#define TIM4_SR *(long*) 	0x40000810
#define TIM5_SR *(long*)	0x40000C10
	
//Timers Prescalers
#define TIM2_PSC *(long*) 0x40000028						
#define TIM3_PSC *(long*)	0x40000428
#define TIM4_PSC *(long*) 0x40000828
#define TIM5_PSC *(long*)	0x40000C28

//Timers Auto Reload Registers
#define TIM2_ARR *(long*) 0x4000002C
#define TIM3_ARR *(long*)	0x4000042C
#define TIM4_ARR *(long*) 0x4000082C
#define TIM5_ARR *(long*)	0x40000C2C

//Timers DMA/Interrupt Enable Registers
#define TIM2_DIER *(long*)	0x4000000C				
#define TIM3_DIER *(long*)	0x4000040C
#define TIM4_DIER *(long*) 	0x4000080C
#define TIM5_DIER *(long*)	0x40000C0C

//Timers Event Generation Register
#define TIM2_EGR *(long*)	0x40000014					
#define TIM3_EGR *(long*)	0x40000414
#define TIM4_EGR *(long*) 0x40000814
#define TIM5_EGR *(long*)	0x40000C14

	

void printstring(char*);
void menu(void);
void SetDuty(int);
void SetFreq(int);

extern void USART2_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TIM4_IRQHandler(void);
extern void TIM5_IRQHandler(void);

void ModifyBaud(void);												
void MCUIDRead(void);
void error(void);

static int ReceivedCommand = 0;
static int submenu=0;
static int SelectedSignal;

static int TIM2_ARRvalueON	=	0x1388;
static int TIM2_ARRvalueOFF	=	0x1388;

static int TIM3_ARRvalueON	=	0x1388;
static int TIM3_ARRvalueOFF	=	0x1388;

static int TIM4_ARRvalueON	=	0x1388;
static int TIM4_ARRvalueOFF	=	0x1388;

static int TIM5_ARRvalueON	=	0x1388;
static int TIM5_ARRvalueOFF	=	0x1388;


int main(void){

	RCC_APB1ENR |=  0x2000F;                    		//USART2, TIM2, TIM3, TIM4, TIM5 clock enable 
	RCC_AHB1ENR |=  (1<<0);                     		//GPIOA clock enable 
	GPIOA_MODER |=  0x5A5;     											//Set PA2 e PA3 as AF, PA0/1/4/5 as Output
	GPIOA_AFRL  =		0x7700;													//Set PA2 e PA3 as AF07
	USART2_CR1  |=  0x202C; 												//USART2 enable, Rx Tx enable, RNEIE (Read register not empty interrupt enable)
	USART2_BRR  =  	0x683;                     			//USART2 baud rate set to 9600
	
	TIM2_DIER		|= (1<<0);													//TIM2 Update Interrupt Enable
	TIM3_DIER		|= (1<<0);													//TIM3 Update Interrupt Enable
	TIM4_DIER		|= (1<<0);													//TIM4 Update Interrupt Enable
	TIM5_DIER		|= (1<<0);													//TIM5 Update Interrupt Enable
	
	TIM2_PSC 		= 0xF;															//TIM2 Prescaler set to 16    so fCount = 16 MHz / 16 = 1 MHz
	TIM3_PSC 		= 0xF;															//TIM3 Prescaler set to 16
	TIM4_PSC 		= 0xF;															//TIM4 Prescaler set to 16
	TIM5_PSC 		= 0xF;															//TIM5 Prescaler set to 16
	
	TIM2_ARR		= 0;																//TIM2 ARR set to 0
	TIM3_ARR		= 0;																//TIM3 ARR set to 0
	TIM4_ARR		= 0;																//TIM4 ARR set to 0
	TIM5_ARR		= 0;																//TIM5 ARR set to 0
	
	TIM2_CR1		|= 0x5;															//TIM2 Counter Enable, UG Interrupt Disable, TIM2_ARR Not Buffered 
	TIM3_CR1		|= 0x5;															//TIM3 Counter Enable, UG Interrupt Disable, TIM3_ARR Not Buffered 
	TIM4_CR1		|= 0x5;															//TIM4 Counter Enable, UG Interrupt Disable, TIM4_ARR Not Buffered 
	TIM5_CR1		|= 0x5;															//TIM5 Counter Enable, UG Interrupt Disable, TIM5_ARR Not Buffered 


	NVIC_ISER1	|= 	(1<<6)	|	(1<<18);							//USART2 and TIM5	NVIC Interrupt Enable
	NVIC_ISER0	|= 0x70000000;											//TIM2/3/4 				NVIC Interrupt Enable 
	
	printstring("Benvenuto nel programma per la generazione di segnali PWM tramite la scheda STM32F401RE\n");
	menu();
	
	while(1) {
		
		if(ReceivedCommand) {
			
			printstring("\n\n");

			switch (submenu) {

				case 0 : {											//Menu Iniziale
					
					switch(ReceivedCommand)	{
						
						case '1' :	{													//definire il numero di segnali PWM attivi (0 di default)
							printstring("All'attivazione, il nuovo segnale PWM ha frequenza di default pari a 50 Hz e Duty Cycle pari a 50%\n"
													"Selezionare il numero di segnali PWM da tenere attivi:\n"
													"- Digitare 0 per: 0 segnali PWM attivi\n"
													"- Digitare 1 per: 1 segnale PWM attivo\n"
													"- Digitare 2 per: 2 segnali PWM attivi\n"
													"- Digitare 3 per: 3 segnali PWM attivi\n"
													"- Digitare 4 per: 4 segnali PWM attivi\n");
							submenu=1;
						}
						break;
					
						case '2' :	{													//modificare i parametri di un segnale PWM attivo
							printstring("Selezionare il segnale di cui modificare i parametri:\n"
													"- Digitare 1 per: segnale 1\n"
													"- Digitare 2 per: segnale 2\n"
													"- Digitare 3 per: segnale 3\n"
													"- Digitare 4 per: segnale 4\n");	
							submenu = 2;
						}
						break;
					
						case '3' :	{													//definire il baud rate della comunicazione seriale (9600 di default)
							ModifyBaud();
						}
						break;
					
						case '4' :	{													//leggere l'ID del microcontrollore
							MCUIDRead();
						}
						break;
					
						default :		error();
					}
		//
				}
				break;
//
				case 1 : {											//Submenu scelta numero di segnali PWM attivi
				
				switch(ReceivedCommand) {
				
					case '0' :	{											//0 segnali PWM attivi
						TIM2_ARRvalueON		= 0;																//TIM2 ARR set to 0
						TIM2_ARRvalueOFF	= 0;
						TIM2_ARR 					= 0;
						
						TIM3_ARRvalueON		= 0;																//TIM3 ARR set to 0
						TIM3_ARRvalueOFF	= 0;
						TIM3_ARR					= 0;
						
						TIM4_ARRvalueON		= 0;																//TIM4 ARR set to 0
						TIM4_ARRvalueOFF	= 0;
						TIM4_ARR					= 0;
						
						TIM5_ARRvalueON		= 0;																//TIM5 ARR set to 0
						TIM5_ARRvalueOFF	= 0;
						TIM5_ARR					= 0;
						
						GPIOA_ODR 	&= (~( (1<<0) | (1<<1) | (1<<4) | (1<<5) ));				//Sets all GPIO Outputs to 0
						printstring("0 Segnali PWM Attivi\n\n");
						menu();
					}
					break;
					
					case '1' :	{											//1 segnale PWM attivo
						
						TIM2_ARRvalueON		= 	0x2710;				//TIM2 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM2_ARRvalueOFF	= 	0x2710;
						TIM2_ARR 					= 	TIM2_ARRvalueON;
						TIM2_EGR 					|=	(1<<0);				//Force Update

						
						
						TIM3_ARRvalueON		= 	0;						//TIM3 ARR set to 0
						TIM3_ARRvalueOFF	= 	0;
						TIM3_ARR					= 	0;
						
						TIM4_ARRvalueON		= 	0;						//TIM4 ARR set to 0
						TIM4_ARRvalueOFF	= 	0;
						TIM4_ARR					= 	0;
						
						TIM5_ARRvalueON		= 	0;						//TIM5 ARR set to 0
						TIM5_ARRvalueOFF	= 	0;
						TIM5_ARR					= 	0;

						GPIOA_ODR 	&= (~( (1<<1) | (1<<4) | (1<<5) ));	//Sets all GPIO Outputs to 0 except for PA0		
						printstring("1 Segnale PWM Attivo, prelevabile su PA0\n\n");
						menu();
					}
					break;
					
					case '2' :	{											//2 segnali PWM attivi
						
						TIM2_ARRvalueON		= 	0x2710;				//TIM2 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM2_ARRvalueOFF	= 	0x2710;
						TIM2_ARR 					=		0x2710;
						TIM2_EGR 					|= 	(1<<0);					//Force Update

						
						TIM3_ARRvalueON		= 	0x2710;					//TIM3 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM3_ARRvalueOFF	= 	0x2710;
						TIM3_ARR					= 	0x2710;
						TIM3_EGR 					|= 	(1<<0);					//Force Update
						
						TIM4_ARRvalueON		= 	0;							//TIM4 ARR set to 0
						TIM4_ARRvalueOFF	= 	0;
						TIM4_ARR					= 	0;
						
						TIM5_ARRvalueON		= 	0;							//TIM5 ARR set to 0
						TIM5_ARRvalueOFF	= 	0;
						TIM5_ARR					= 	0;

						GPIOA_ODR 	&= (~( (1<<4) | (1<<5) ));					//Sets all GPIO Outputs to 0 except for PA0/1
						printstring("2 Segnali PWM Attivi, prelevabili su PA0 e PA1\n\n");
						menu();
					}
					break;
					
					case '3' :	{											//3 segnali PWM attivi
						
						TIM2_ARRvalueON		= 	0x2710;					//TIM2 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM2_ARRvalueOFF	= 	0x2710;					
						TIM2_ARR 					= 	0x2710;
						TIM2_EGR 					|= 	(1<<0);					//Force Update

						
						TIM3_ARRvalueON		= 	0x2710;					//TIM3 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM3_ARRvalueOFF	= 	0x2710;
						TIM3_ARR					= 	0x2710;
						TIM3_EGR 					|= 	(1<<0);					//Force Update
						
						TIM4_ARRvalueON		= 	0x2710;					//TIM4 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM4_ARRvalueOFF	= 	0x2710;
						TIM4_ARR					= 	0x2710;
						TIM4_EGR 					|= 	(1<<0);					//Force Update
						
						TIM5_ARRvalueON		= 	0;							//TIM5 ARR set to 0
						TIM5_ARRvalueOFF	= 	0;
						TIM5_ARR					= 	0;

						GPIOA_ODR 	&= (~ (1<<5) );											//Sets PA5 to 0
						printstring("3 Segnali PWM Attivi, prelevabili su PA0, PA1 e PA4\n\n");
						menu();
					}
					break;
					
					case '4' :	{											//4 segnali PWM attivi
						
						TIM2_ARRvalueON		= 	0x2710;					//TIM2 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM2_ARRvalueOFF	= 	0x2710;					
						TIM2_ARR 					= 	0x2710;
						TIM2_EGR 					|= 	(1<<0);					//Force Update

						
						TIM3_ARRvalueON		= 	0x2710;					//TIM3 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM3_ARRvalueOFF	= 	0x2710;
						TIM3_ARR					= 	0x2710;
						TIM3_EGR 					|= 	(1<<0);					//Force Update
						
						TIM4_ARRvalueON		= 	0x2710;					//TIM4 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM4_ARRvalueOFF	= 	0x2710;
						TIM4_ARR					= 	0x2710;
						TIM4_EGR 					|= 	(1<<0);					//Force Update
						
						TIM5_ARRvalueON		= 	0x2710;					//TIM5 ARR set to 5000 (fPWM = 50 Hz, Duty = 50%)
						TIM5_ARRvalueOFF	= 	0x2710;
						TIM5_ARR					= 	0x2710;
						TIM5_EGR 					|= 	(1<<0);					//Force Update

						printstring("4 Segnali PWM Attivi, prelevabili su PA0, PA1, PA4 e PA5\n\n");
						menu();
					}
					break;
					
					default : error();
				}
		//	
			}
				break;
//
				case 2 : {											//Submenu scelta segnale PWM di cui modificare i parametri
				
					switch (ReceivedCommand) {							//Assegna il numero del segnale selezionato alla variabile "SelectedSignal"
						
						case '1' :	{											//Modify signal 1
							if(TIM2_ARR)
								SelectedSignal=1;
							else {
								printstring("Il segnale selezionato non è attualmente attivo.\n\n");
								menu();
							}
							ReceivedCommand=0;
						}
						break;
						
						case '2' : 	{											//Modify signal 2
							if(TIM3_ARR)
								SelectedSignal=2;
							else {
									printstring("Il segnale selezionato non è attualmente attivo.\n\n");
									menu();
							}
							ReceivedCommand=0;
						}
						break;
						
						case '3' : 	{											//Modify signal 3
							if(TIM4_ARR)
								SelectedSignal=3;
							else {
								printstring("Il segnale selezionato non è attualmente attivo.\n\n");
								menu();
							}
							ReceivedCommand=0;
						}
						break;
						
						case '4' :	{											//Modify signal 4
							if(TIM5_ARR)
								SelectedSignal=4;
							else {
									printstring("Il segnale selezionato non è attualmente attivo.\n\n");
									menu();
							}
							ReceivedCommand=0;
						}
						break;
							
						default : error();						
					}
		//
					if (SelectedSignal) {
						printstring("Scegliere il parametro da modificare:\n"
												"- Digitare 1 per modificare il Duty Cycle del segnale selezionato\n"
												"- Digitare 2 per modificare la Frequenza  del segnale selezionato\n");	
						submenu=4;
					}
		//	
				}
				break;
//
				case 3 : {											//Submenu scelta Baud Rate
																	/*	USARTDIV = fCLK / ( 16 * BR )*/
				switch(ReceivedCommand) {		
					case '1' :										//BR = 2400 -> USARTDIV = 416,667 -> DIV_Mantissa = 416 = 0b000110100000 ; DIV_Frac = 11 = 0b1011
						USART2_BRR = 0x1A0B;
					break;
					
					case '2' :										//BR = 4800 -> USARTDIV = 208,333 -> DIV_Mantissa = 208 = 0b11010000 ; 	DIV_Frac = 5 = 0b0101
						USART2_BRR = 0xD05;
					break;
					
					case '3' :										//BR = 9600 -> USARTDIV = 104.17 	-> DIV_mantissa = 104 = 0b01101000 ;  DIV_frac =~ 3 = 0b0011 
						USART2_BRR = 0x683;
					break;
					
					case '4' :										//BR = 14400 -> USARTDIV = 69,444 -> DIV_Mantissa = 69 = 0b01000101 ; DIV_Frac = 7 = 0b0111
						USART2_BRR = 0x457;
					break;
					
					case '5' :										//BR = 19200 -> USARTDIV = 52,083 -> DIV_Mantissa = 52 = 0b00110100 ; DIV_Frac = 1 = 0b0001
						USART2_BRR = 0x341;
					break;
					
					default :		error();
				}
		//
				menu();
			}
				break;
//	
				case 4 : {											//Submenu scelta parametro da modificare								

				switch (ReceivedCommand) {
					
					case '1' :
						printstring("Selezionare il nuovo valore del Duty Cycle:\n"
												"- Digitare 1 per: 10%\n"
												"- Digitare 2 per: 30%\n"
												"- Digitare 3 per: 50%\n"
												"- Digitare 4 per: 70%\n"
												"- Digitare 5 per: 90%\n"
					);
					submenu=5;
					break;
					
					case '2' :
						printstring("Selezionare il nuovo valore della frequenza:\n"
												"- Digitare 1 per: 1 Hz\n"
												"- Digitare 2 per: 10 Hz\n"
												"- Digitare 3 per: 20 Hz\n"
												"- Digitare 4 per: 100 Hz\n"
												"- Digitare 5 per: 1 kHz\n"
												"- Digitare 6 per: 10 kHz\n");
						submenu=6;
					break;
					
					default: error();		
				}
		//
			}
				break;
//
				case 5 : {											//Submenu scelta nuovo valore Duty Cycle
					
					switch (ReceivedCommand) {
						case '1' :									//Duty = 10%
							SetDuty(10);
						break;
						
						case '2' :									//Duty = 30%
							SetDuty(30);
						break;
						
						case '3' :									//Duty = 50%
							SetDuty(50);
						break;
						
						case '4' :									//Duty = 70%
							SetDuty(70);
						break;
						
						case '5' :									//Duty = 90%
							SetDuty(90);
						break;
						
						default : error();
				}
		//			
			}
				break;
//
				case 6 : {											//Submenu scelta nuovo valore di Frequenza
					switch (ReceivedCommand) {
						case '1' : 								//New Frequency is 1 Hz
							SetFreq(1);
							break;
						
						case '2' : 								//New Frequency is 10 Hz
							SetFreq(10);
						break;
						
						case '3' : 								//New Frequency is 20 Hz
							SetFreq(20);
						break;
						
						case '4' : 								//New Frequency is 100 Hz
							SetFreq(100);
						break;
						
						case '5' : 								//New Frequency is 1 kHz
							SetFreq(1000);
						break;
						
						case '6' : 								//New Frequency is 10 kHz
							SetFreq(10000);
						break;
					}
//
					menu();
				}
				break;
			}
//
			ReceivedCommand=0;
		}
//
	}
//
}
//

void menu() {
	printstring("Selezionare l'opzione desiderata:\n"
							"- Digitare 1 per : definire il numero di segnali PWM attivi (0 di default)\n"
							"- Digitare 2 per : modificare i parametri di un segnale PWM attivo\n"
							"- Digitare 3 per : definire il baud rate della comunicazione seriale (9600 di default)\n"
							"- Digitare 4 per : leggere l'ID del microcontrollore\n\n");
	submenu=0;
}
//

void printstring(char* str){
 
	while(*str !=   '\0') {
			USART2_SR  &=   (~(1<<6));                  				//USART 2 Transmission complete flag set to 0
			USART2_DR = *str;
			while(! (( USART2_SR & (1<<6) ) >>6)  );						//Wait for transmission complete   
			str++;
	}
}
//

void USART2_IRQHandler () {
	
	ReceivedCommand = USART2_DR;
}
//

void TIM2_IRQHandler() {							//TIM2 controls PWM signal on PA0
	TIM2_SR &= (~(1<<0));											//TIM2 Interrupt Flag set to 0 
	GPIOA_ODR ^= (1<<0);
	
	if (GPIOA_ODR	&	1)
		TIM2_ARR 		= TIM2_ARRvalueON;
	else
		TIM2_ARR 		= TIM2_ARRvalueOFF;
}
//

void TIM3_IRQHandler() {							//TIM3 controls PWM signal on PA1
	TIM3_SR &= (~(1<<0));											//TIM3 Interrupt Flag set to 0 
	GPIOA_ODR		^= (1<<1);										
	
	if (GPIOA_ODR 	& (1<<1))
		TIM3_ARR 		= TIM3_ARRvalueON;
	else
		TIM3_ARR 		= TIM3_ARRvalueOFF;
}
//

void TIM4_IRQHandler() {							//TIM4 controls PWM signal on PA4
	TIM4_SR &= (~(1<<0));											//TIM4 Interrupt Flag set to 0 
	GPIOA_ODR		^= (1<<4);										

	if (GPIOA_ODR 	& (1<<4))
		TIM4_ARR 		= TIM4_ARRvalueON;
	else 
		TIM4_ARR 		= TIM4_ARRvalueOFF;
}
//

void TIM5_IRQHandler() {							//TIM5 controls PWM signal on PA5
	TIM5_SR &= (~(1<<0));											//TIM5 Interrupt Flag set to 0 
	GPIOA_ODR 	^= (1<<5);

	if (GPIOA_ODR 	& (1<<5))
		TIM5_ARR 		= TIM5_ARRvalueON;
	else
		TIM5_ARR 		= TIM5_ARRvalueOFF;
}
//

void MCUIDRead () {													
	int i;
	char ID[200];
	char* point = &ID[0];
	
	printstring("L'ID del microcontrollore è :\n");
		
	for(i=31;i>=0;i--) {
		if(MCUID3 & (long)(1<<i))
			sprintf(point, "1");
		else
			sprintf(point, "0");

		point++;

		if(! ( i % 4 ) ) {
			sprintf(point, " ");
			point++;
		}
	}


		for(i=31;i>=0;i--) {
		if(MCUID2 & (long)(1<<i))
			sprintf(point, "1");
		else
			sprintf(point, "0");

		point++;

		if(! ( i % 4 ) ) {
			sprintf(point, " ");
			point++;
		}
	}


		for(i=31;i>=0;i--) {
		if(MCUID1 & (long)(1<<i))
			sprintf(point, "1");
		else
			sprintf(point, "0");

		point++;

		if( (! ( i % 4 ) ) && (i!=0) ) {
			sprintf(point, " ");
			point++;
		}
	}

	sprintf(point, "\n \n \0");
	printstring(ID);
	menu();	
}
//

void ModifyBaud () {											
	submenu = 3;
	printstring("Selezionare il nuovo valore da impostare per il Baud Rate (BR): \n");
	printstring("- Digitare 1 per: BR = 2400\n");
	printstring("- Digitare 2 per: BR = 4800\n");
	printstring("- Digitare 3 per: BR = 9600\n");
	printstring("- Digitare 4 per: BR = 14400\n");
	printstring("- Digitare 5 per: BR = 19200\n\n");
}
//

void SetFreq(int NewFreq) {						//Set New Frequency Value
	int Duty=0;																//In percentage
//	char a[100];

	switch (SelectedSignal) {
		
		case 1 :																//Modify Signal 1 Frequency
			Duty = 100*(int)TIM2_ARRvalueON / ((int)TIM2_ARRvalueON + (int)TIM2_ARRvalueOFF);
			TIM2_ARRvalueON = 10000 * Duty / NewFreq;
			TIM2_ARRvalueOFF = ( 100 - Duty ) * 10000 / NewFreq;
//			sprintf(a, "Duty = %d\n Ton = %d\n Toff = %d\n", Duty, TIM2_ARRvalueON, TIM2_ARRvalueOFF);
//			printstring(a);
			break;
		
		case 2 :																//Modify Signal 2 Frequency
			Duty = 100*(int)TIM3_ARRvalueON / ((int)TIM3_ARRvalueON + (int)TIM3_ARRvalueOFF);
			TIM3_ARRvalueON = 10000 * Duty / NewFreq;
			TIM3_ARRvalueOFF = ( 100 - Duty ) * 10000 / NewFreq;
			break;
		
		case 3 :																//Modify Signal 3 Frequency
			Duty = 100*(int)TIM4_ARRvalueON / ((int)TIM4_ARRvalueON + (int)TIM4_ARRvalueOFF);
			TIM4_ARRvalueON = 10000 * Duty / NewFreq;
			TIM4_ARRvalueOFF = ( 100 - Duty ) * 10000 / NewFreq;
			break;
		
		case 4 :																//Modify Signal 4 Frequency
			Duty = 100*(int)TIM5_ARRvalueON / ((int)TIM5_ARRvalueON + (int)TIM5_ARRvalueOFF);
			TIM5_ARRvalueON = 10000 * Duty / NewFreq;
			TIM5_ARRvalueOFF = ( 100 - Duty ) * 10000 / NewFreq;
			break;

	}
	SelectedSignal=0;
}
//

void SetDuty(int Duty) {							//Set New Duty Value
	
	int Ton;
//	GPIOA_ODR ^= (1<<5);
	switch (SelectedSignal) {							//Scelta numero del segnale PWM di cui modificare il Duty
		
		case 1 :	{															//Imposta nuovo Duty per il segnale 1								
			Ton = TIM2_ARRvalueON;
			TIM2_ARRvalueON = (int)( ((int)TIM2_ARRvalueON + (int)TIM2_ARRvalueOFF) * Duty / 100);
			TIM2_ARRvalueOFF = TIM2_ARRvalueOFF + Ton - TIM2_ARRvalueON;
		}
		break;
		
		case 2 :	{															//Imposta nuovo Duty per il segnale 2
		//	GPIOA_ODR ^= (1<<5);
			Ton = TIM3_ARRvalueON;
			TIM3_ARRvalueON = (int)((TIM3_ARRvalueON + TIM3_ARRvalueOFF)*Duty/100);
			TIM3_ARRvalueOFF = TIM3_ARRvalueOFF+Ton-TIM3_ARRvalueON;
		}
		break;
		
		case 3 :	{															//Imposta nuovo Duty per il segnale 3
			Ton = TIM4_ARRvalueON;
			TIM4_ARRvalueON = (int)((TIM4_ARRvalueON + TIM4_ARRvalueOFF)*Duty/100);
			TIM4_ARRvalueOFF = TIM4_ARRvalueOFF+Ton-TIM4_ARRvalueON;
		}
		break;
		
		case 4 : {														//Imposta nuovo Duty per il segnale 4
			Ton = TIM5_ARRvalueON;
			TIM5_ARRvalueON = (int)((TIM5_ARRvalueON + TIM5_ARRvalueOFF)*Duty/100);
			TIM5_ARRvalueOFF = TIM5_ARRvalueOFF+Ton-TIM5_ARRvalueON;
		}
		break;
	}

		SelectedSignal = 0;
		menu();		
}
//

void error () {
	printstring("Errore: Comando Non Valido\n\n");
	menu();
}
//