//WORKING version 13.09.2016.
//miSRT_2

#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

//#define FOSC 8000000//1843200// Clock Speed
//#define BAUD 9600
//#define MYUBRR FOSC/16/BAUD-1
//#define MYUBRR 0x0033

#define D4 eS_PORTC0
#define D5 eS_PORTC1
#define D6 eS_PORTC2
#define D7 eS_PORTC3
#define RS eS_PORTJ0
#define EN eS_PORTJ1

#include <avr/io.h>
#include <avr/iom2560.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <inttypes.h>
//#include <avr/signal.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include "globals.h"

void Hardware_Init();
void USART_Init();
void Lcd4_Init();
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void InitTimer1();
void InitTimer3();
void InitTimer4();
void Go_To_Zero_Init();
int Go_To (int X_Axis, int Y_Axis);

int temp = 0;
int X_Axis = 0;// number of steps X
int Y_Axis = 0;// number of steps Y
char *Screens[8] = {"40 Grams  ", "45 Grams  ", "55 Grams  ", "65 Grams  ", "70 Grams  ", "80 Grams  ", 
								  "90 Grams  ", "100 Grams "};
int CountScreen = 0;
volatile uint8_t tot_overflow = 0;
int STOP_X = 0;// stop X flag, switch ON
int STOP_Y = 0;// stop Y flag, switch ON
int Grams = 0;
//volatile int Init_Grams_Left = 0;
int Global_Weight_Left = 0;
int Global_Weight_Right = 0;
int Sum_L = 0;
int Sum_R = 0;
errorFill_L[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
errorFill_R[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int Weight_Calculated_Right = 0;
char MyWeight_Right[16];
int Weight_Calculated_Left = 0;
char MyWeight_Left[16];
char Screen_Buffer[16];
double TMP_count = 0;
long temp_timer = 0;

ISR( TIMER1_COMPA_vect )//compare
{
	PORTF ^= 0x02;
	////TCNT1 = 0;
	
	//OCR1A = TMP_count;
	//if (TMP_count == 50)//if (TMP_count == temp_timer)
	//{
		////PORTF ^= 0x02;
		//TMP_count = 0;
		//temp_timer++;
	//}
	//else if (temp_timer == 65000)
	//{
		//TCCR1B = 0b11111000;
		//Lcd4_Clear();
		//Lcd4_Set_Cursor(1,0);
		//Lcd4_Write_String("STOP");
		//_delay_ms(2000);
		//OCR1A = 0;
		//TMP_count = 0;
		//temp_timer = 0;
	//}
	//TMP_count ++;
}
ISR( TIMER3_COMPA_vect)//compare
{
	//PORTF ^= 0x02;
	PORTF ^= 0x08;
}
ISR( TIMER4_COMPA_vect )//compare
{
	tot_overflow++;
	cli();
	
	if (tot_overflow >= 2)//10
	{
		char temp = 0;
		if ((bit_is_clear(PINL, PL4)))/////////AIR pressure alarm
		{
			PORTF ^= (1<<PF7);
			//PORTF |= (1 << PF7);//Red ON
			_delay_ms(300);
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("AIR OFF");
			temp = 1;
		}
		else if (temp == 1) 
		{
			PORTF &= ~(1 << PF7);//Red OFF
			temp = 0;
		}
		Lcd4_Clear();
		if (!(bit_is_clear(PINK, PK0)) && STOP_X == 0)// X Left "X Zero"
		{
			TCCR1B = 0b11111000;// kill X Axis
			STOP_X = 1;
			tot_overflow = 0;
		}
		if (!(bit_is_clear(PINK, PK1)))// X Right
		{
			TCCR3B = 0b11111000;// kill Y Axis 
			TCCR1B = 0b11111000;// kill X Axis
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("ERROR X");
			_delay_ms(50);
		}
		if (!(bit_is_clear(PINK, PK2)))// Y bottom
		{
			TCCR3B = 0b11111000;// kill Y Azis
			TCCR1B = 0b11111000;// kill X Axis 
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("ERROR Y");
			_delay_ms(50);
		}
		if (!(bit_is_clear(PINK, PK3)) && STOP_Y == 0)// Y top "Y Zero"
		{
			TCCR3B = 0b11111000;// kill Y Azis 
			STOP_Y = 1;
			tot_overflow = 0;
		}
		temp = 0;
		sei();
	}
}

void ChangeScreen(int CountScreen)
{
	//Screen(CountScreen);
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Select Weight:");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String(Screens[CountScreen]);//
}

void TST_Method()
{
	X_Axis_Step_Plus();
	_delay_ms(500);
	X_Axis_Step_Plus();
	_delay_ms(500);
	X_Axis_Step_Plus();
	_delay_ms(500);
	Y_Axis_Step_Plus();
	_delay_ms(500);
	Y_Axis_Step_Plus();
	_delay_ms(500);
	Y_Axis_Step_Plus();
	_delay_ms(500);
	Y_Axis_Step_Plus();
	_delay_ms(500);
	X_Axis_Step_Minus();
	_delay_ms(500);
	X_Axis_Step_Minus();
	_delay_ms(500);
	X_Axis_Step_Minus();
	_delay_ms(500);
	Y_Axis_Step_Minus();
	_delay_ms(500);
	Y_Axis_Step_Minus();
	_delay_ms(500);
	Y_Axis_Step_Minus();
	_delay_ms(500);
	Y_Axis_Step_Minus();
	_delay_ms(500);
	//X_Axis_Step_Plus_TMP();
	//
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("OUT");
	//_delay_ms(2000);
	while (1)
	{
		//X_Axis_Step_Plus_TMP();
	}
}

//ISR(TIMER1_OVF_vect)//overflow 
//{
	//cli();// Disable Interrupt
	//PORTF ^= 0x02;
	//TCNT1 = 0;
	//PORTG = 0xFF;
	//sei();// Enable Interrupt
//}

int main(void)
{
	//long temp = 0;
	_delay_ms(10);
	DDRG = 0xFF;//diode
	PORTG = 0x00;
	////InitTimer0();
	InitTimer1();
	InitTimer3();
	InitTimer4();
	Hardware_Init();
	USART_Init();
	Lcd4_Init();
	
	_delay_ms(100);
	PORTK |= (1 << PK6);//GREEN ON
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("miSRT Mk2");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Init...");
	_delay_ms(2000);
	
	Go_To_Zero_Init();// Set Zero Axis
	_delay_ms(50);
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("miSRT Mk2 Ready");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Select Weight:");
	
	while ((bit_is_clear(PINL, PL5)))  // Select
	{	
		//Lcd4_Set_Cursor(1,0);
		//Lcd4_Write_String("clear");			
	}
	
	_delay_ms(300);
	
	//TST_Method();
	
	SetWeight();
	_delay_ms(300);
	SetCapacity();
	_delay_ms(300);
	SetCups();
	
	itoa(Gram_Target, Screen_Buffer, 10);
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Selected Weight:");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String(Screen_Buffer);
	_delay_ms(2000);
	
	//ChangeScreen(CountScreen);
	PORTK = (1<<PK5);// Open AIR
	Lcd4_Clear();
	
	while(1)
	{
		while ((bit_is_clear(PINL, PL5)))
		{
			if (!(bit_is_clear(PINL, PL7)))//select new weight  
			{
				SetWeight();
				_delay_ms(300);
				SetCapacity();
				_delay_ms(300);
				SetCups();
			}
			itoa(Gram_Target, Screen_Buffer, 10);
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("New Weight Set:");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String(Screen_Buffer);
			_delay_ms(300);
		}
		
		PORTK &= ~(1 << PK6);// Green OFF
		PORTF |= (1 << PF7);//Red ON
					
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Fill:");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String(Screen_Buffer);
					
		//fill 
		Grams = Gram_Target - 2;
		//X_Axis = 3;// number of positions ???????????????????????????????????????
		//Y_Axis = 5;// never "0"
		Go_To (X_Axis, Y_Axis);
		//
		_delay_ms(10);
		Sum_L = 0;
		char Screen[16];
		for (int i = 0; i < 20; i++)//Left
		{
			itoa(errorFill_L[i], Screen, 10);
			if (errorFill_L[i] > 0)
			{
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("Fail Left On:");
				Lcd4_Set_Cursor(2,0);
				Lcd4_Write_String(Screen);
				while (bit_is_clear(PINL, PL5));
				errorFill_L[i] = 0;
			}
			//errorFill_L[i] = 0;
			_delay_ms(500);
		}
		countErrors_L = 0;
					
		Sum_R = 0;
		for (int i = 0; i < 20; i++)//Right
		{
			itoa(errorFill_R[i], Screen, 10);
			if (errorFill_R[i] > 0)
			{
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("Fail Right On:");
				Lcd4_Set_Cursor(2,0);
				Lcd4_Write_String(Screen);
				while (bit_is_clear(PINL, PL5));
				errorFill_R[i] = 0;
			}
			//errorFill_R[i] = 0;
			_delay_ms(500);
		}
		countErrors_R = 0;
		countFill = 1;
		//
		STOP_X = 0;
		STOP_Y = 0;
		_delay_ms(200);
		Go_To_Zero_Init();
					
		PORTF &= ~(1 << PF7);//Red OFF
		PORTK |= (1 << PK6);// Green ON
			
		//if (!(bit_is_clear(PINL, PL7)))
		//{
			//Lcd4_Clear();
			//Lcd4_Set_Cursor(1,0);
			//Lcd4_Write_String("OUT");
			//_delay_ms(2000);
		//}
	}
}