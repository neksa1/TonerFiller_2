#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

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
#include <stdint.h>
#include "globals.h"
#include <avr/interrupt.h>

//#define HX711_SCK  PORTB  //PB1 instructions 
//#define SCKPIN    1<<PB0
////#define HX711_DT PINB.6
//#define HX711_DT  PORTB
//#define DTPIN    1<<PB4
//#define HIGH 1
//#define LOW 0
//
//long HX711_Buffer = 0;
//long Weight_Maopi = 0,Weight_Shiwu = 0;
//int Weight = 0;
//char MyWeight[16];

void Hardware_Init()
{
	//1 output, 0 input, 7,6,5,4,3,2,1,0
	DDRC = 0xFF;//display data 
	DDRJ = 0xFF;//display control
	
	//Tasters 
	DDRL = 0b11100001;//0xE0  L4 AIR PRESURE 
	PORTL = 0b00000001;// on 7, 6, 5
	
	//Limits										            Valve_L     AIR       Green     Valve_R
	DDRK = 0xFF;//(1<<PK0) | (1<<PK1) | (1<<PK2) | (1<<PK3) | (1<<PK4) | (1<<PK5) | (1<<PK6) | (1<<PK7);// 0x0F; //K6 zeleno//PF7 red
	PORTK =0x00;//(0<<PK0) | (0<<PK1) | (0<<PK2) | (0<<PK3) | (0<<PK4) | (0<<PK5) | (0<<PK6) | (0<<PK7);// 0x00;
	
	// 3, 2, 1, 0 motor 7, 6, 5, 4 Mosfet
	// F0 = Motor X Direction
	// F1 = Motor X PWM
	// F2 = Motor Y Direction
	// F3 = Motor Y PWM
	// F7 = Led Red
	DDRF = (1<<PF0) | (1<<PF1) | (1<<PF2) | (1<<PF3) | (1<<PF4) | (1<<PF5) | (1<<PF6) | (1<<PF7);//0F
	PORTF = (0<<PF0) | (0<<PF1) | (0<<PF2) | (0<<PF3) | (0<<PF4) | (0<<PF5) | (0<<PF6) | (0<<PF7);
	
	//Load Cells
	        //CLK        DT
	DDRB = (1<<PB0) | (0<<PB4);// LEVA  Right
	PORTB =(1<<PB0) | (0<<PB4);
	//DDRE = (1<<PE2) | (0<<PE3);// Right
	//PORTE =(1<<PE2) | (0<<PE3);
	DDRE = (1<<PE0) | (0<<PE1);// DESNA Left
	PORTE =(1<<PE0) | (0<<PE1);
	//DDRE = (1<<PE0) | (0<<PE1) | (1<<PE2) | (0<<PE3);
	//PORTE =(1<<PE0) | (0<<PE1) | (1<<PE2) | (0<<PE3);
	
	//Valves 
	        //Air     Valve_L    Valve_R
	//DDRK = (1<<PK5) | (1<<PK4) | (1<<PK7);
	//PORTK =(0<<PK5) | (0<<PK4) | (0<<PK7);
	
	
	//DDRD = 0xFF;
	//PORTD = 0x00;
	
	//// Initial TIMER0 Phase Correct PWM
	//// Fast PWM Frequency = fclk / (N * 510), Where N is the Prescaler
	//// f_PWM = 11059200 / (64 * 510) = 338.82 Hz
	//TCCR0A = 0b10100001; // Phase Correct PWM 8 Bit, Clear OCA0/OCB0 on Compare Match, Set on TOP
	////TCCR0A = 0b10100011; // Fast PWM 8 Bit, Clear OCA0/OCB0 on Compare Match, Set on TOP
	//TCCR0B = 0b00000010; // Used 64 Prescaler// 15686Hz
	//TCNT0 = 0;           // Reset TCNT0
	//OCR0A = 0;           // Initial the Output Compare register A & B
	//OCR0B = 0;
}

void USART_Init()
{
	CLKPR=0x80;//prescale 
	CLKPR=0x00;//??????????????????????????????
	
	//UCSR0A=0x00;
	////UCSR0B=0xD8;//?????????????????????????????????
	//UCSR0C=0x06;
	//UBRR0H=0x00;
	//UBRR0L=0x33;
}
void USART_Transmit( unsigned char data )
{
	while ( !( UCSR0A & (1<<UDRE0)) );       // Wait for empty transmit buffer
	UDR0 = data;                             // Put data into buffer, sends the data
}
unsigned char USART_Receive( void )
{
	while ( !(UCSR0A & (1<<RXC0)) );         // Wait for data to be received
	return UDR0;                             // Get and return received data from buffer
}

void InitTimer1()// MOTOR X
{
	//sei();
	//cli();//clear, off interrupt
	
	TCCR1A = 0x80;//8k
	TCCR1B = 0x09;
	OCR1AH = 0x03;
	OCR1AL = 0xE7;
	
	//OCR1A = 65000;
	//OCR1AH = 0x01;
	//OCR1AL = 0x8F;
	//OCR1AH = 0x00;
	//OCR1AL = 0x00;
	TIMSK1 |= (1 << OCIE1A);//set
	//TCCR1B |= (1 << CS10);// set up timer with prescaler = 0
	TCNT1 = 0;// initialize counter
	//TIMSK1 |= (1 << TOIE1);// enable overflow interrupt
}
void InitTimer3()// MOTOR Y
{
	TCCR3A = 0x80;//400hz
	TCCR3B = 0x0C;
	OCR3AH = 0x00;
	OCR3AL = 0x4D;
	TIMSK3 |= (1 << OCIE3A);
	TCNT3 = 0;// initialize counter
}
void InitTimer4 ()
{
	TCCR4A = 0x80;
	TCCR4B = 0x0D;
	OCR4AH = 0x00;
	OCR4AL = 0x9B;
	TIMSK4 |= (1 << OCIE4A);//set
	TCNT4 = 0;
}

//unsigned long HX711_Read(void)
//{
	//unsigned long count;
	//unsigned char i;
	////unsigned int Flag = 0;
	////|= (1 << PF0); // set direction 1
	////&= ~(1 << PF0); // set direction 0
	//
	//HX711_DT |= (1 << PB4); //|= (1 << PB4);//
	//
	//_delay_us(1);
	//
	//HX711_SCK &= ~(1 << PB0);
	//_delay_us(1);
	//
	//count=0;
	//while(HX711_DT == 1);
	//for(i=0;i<24;i++)
	//{
		//PORTG = 0xFF;//LED ON
		//HX711_SCK |= (1 << PB0);
		//_delay_us(1);
		//count=count<<1;
		//HX711_SCK &= ~(1 << PB0);
		//_delay_us(1);
		//if(!(bit_is_clear(PINB, PB4)))//HX711_DT == 1 
		//{
			//PORTG = 0x00;
			////_delay_ms(10000);
			//count++;
		//}
	//}
	//HX711_SCK |= (1 << PB0);
	//
	//if (count > 0)
	//{
		//Lcd4_Clear();
		//Lcd4_Set_Cursor(1,0);
		//Lcd4_Write_String("Count > 0");
		//_delay_ms(40);//4000
	//}
	//
	////char MyBuffer[16];
	////unsigned long mycount = count;
	////itoa(mycount, MyBuffer, 10);
	////Lcd4_Clear();
	////Lcd4_Set_Cursor(1,0);
	////Lcd4_Write_String(MyBuffer);
	////Lcd4_Set_Cursor(2,0);
	////Lcd4_Write_String("GetMaopi");
	////_delay_ms(4000);
	//
	//count ^= 0x800000;
	//_delay_us(1);
	//HX711_SCK &= ~(1 << PB0);
	//_delay_us(1);
	//
	////char MyBuffer[16];
	////unsigned long mycount = count;
	////itoa(count, MyBuffer, 10);
	////Lcd4_Clear();
	////Lcd4_Set_Cursor(1,0);
	////Lcd4_Write_String(MyBuffer);
	////Lcd4_Set_Cursor(2,0);
	////Lcd4_Write_String("GetCount");
	////_delay_ms(4000);
	//
	//return(count);
//}
//void Get_Maopi()
//{
	//char MyBuffer[16];
	//
	//HX711_Buffer = HX711_Read();
	//Weight_Maopi = HX711_Buffer/100;//100
	//
	////if (Weight_Maopi > 10000)
	////{
		////Lcd4_Clear();
		////Lcd4_Set_Cursor(1,0);
		////Lcd4_Write_String("Buffer > 0");
		////_delay_ms(4000);
	////}
	////
	//itoa(Weight_Maopi, MyBuffer, 10);
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String(MyBuffer);
	//Lcd4_Set_Cursor(2,0);
	//Lcd4_Write_String("GetMaopi");
	//_delay_ms(40);//4000
//}
//unsigned int Get_Weight()
//{
	//char MyBuffer[16];
	//
	//HX711_Buffer = HX711_Read();
	//HX711_Buffer = HX711_Buffer/100;// /100
	//
	//itoa(HX711_Buffer, MyBuffer, 10);
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String(MyBuffer);
	//Lcd4_Set_Cursor(2,0);
	//Lcd4_Write_String("GetWeight");
	//_delay_ms(40);//4000
	//
	//Weight_Shiwu = HX711_Buffer;
	//Weight_Shiwu = (Weight_Shiwu - Weight_Maopi)*(-1.680);// Parametar za prebacivanje iz njegovog broja u grame
	//Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/7.35+0.05);
	//
	//return Weight_Shiwu;
//}
