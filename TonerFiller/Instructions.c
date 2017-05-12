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
#include <avr/interrupt.h>
#include "globals.h"

#define HX711_SCK_Right  PORTE  //Pins Right Cell
//#define SCKPIN           1<<PB0
#define HX711_DT_Right   PORTE
//#define DTPIN            1<<PB4

#define HX711_SCK_Left  PORTB//E  //Pins Left Cell
//#define SCKPIN           1<<PB0
#define HX711_DT_Left   PORTB//E
//#define DTPIN            1<<PB4

long HX711_Buffer_Right = 0;    //variables Right Cell
long Weight_Zero_Right = 0,Weight_Right = 0;

long HX711_Buffer_Left = 0;    //variables Left Cell
long Weight_Zero_Left = 0,Weight_Left = 0;
countFill = 1;
//errorFill[5] = {0,0,0,0,0};
countErrors_L = 0;
countErrors_R = 0;
Gram_Target = 0;
No_Cups = 20;
Capacity = 1;
char MyWeight[8];
char MyWeight2[8];

//volatile int Init_Grams_Left = 0;
int X = 0;
int Y = 0;

void X_Axis_Step_Plus_TMP()
{
	TCCR1B = 0x09; //set timer1
	TCCR3B = 0b11111000;//kill timer3
	
	sei(); //start PWM
	
	PORTF |= (1 << PF0); // set direction 1
	_delay_ms(55000); // delay ??????????????????????//////____900__//////////////////////
	cli();// stop global interrupts
	return;
}

////Init step
void X_Axis_Step_Plus_Init()
{
	TCCR1B = 0x09; //set timer1
	TCCR3B = 0b11111000;//kill timer3
	
	sei(); //start PWM
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("X+ Init");
	PORTF |= (1 << PF0); // set direction 1
	_delay_ms(1600); // delay ??????????????????????
	cli();// stop global interrupts
	return;
}
void Y_Axis_Step_Plus_Init()
{
	TCCR3B = 0x0A;
	TCCR1B = 0b11111000;
	
	sei();
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("Y+ Init");
	PORTF |= (1 << PF2); // set direction 1
	_delay_ms(250); // delay ??????????????????????
	cli();
	return;
}
void Go_To_Zero_Init()
{
	while (STOP_X == 0)
	{
		X_Axis_Step_Minus();
	}
	X_Axis_Step_Plus_Init();
	STOP_X = 0;
	
	while (STOP_Y == 0)
	{
		Y_Axis_Step_Minus();
	}
	Y_Axis_Step_Plus_Init();
	STOP_Y = 0;
	
	return;
}
////Step
void X_Axis_Step_Plus()
{
	TCCR1B = 0x09; //set timer1
	TCCR3B = 0b11111000;//kill timer3
	
	sei(); //start PWM
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("X+");
	PORTF |= (1 << PF0); // set direction 1
	_delay_ms(1550); // delay ??????????????????????
	X++;
	cli();// stop global interrupts 
	return;
}
void X_Axis_Step_Minus()
{
	TCCR1B = 0x09;
	TCCR3B = 0b11111000;
	
	sei();
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("X-");
	PORTF &= ~(1 << PF0); // set direction 0
	_delay_ms(1550); // delay ??????????????????????
	X--;
	cli();
	return;
}
void Y_Axis_Step_Plus()
{
	TCCR3B = 0x0A;
	TCCR1B = 0b11111000;
	
	sei();
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("Y+");
	PORTF |= (1 << PF2); // set direction 1
	_delay_ms(950); // delay ??????????????????????
	Y++;
	cli();
	return;
}
void Y_Axis_Step_Minus()
{
	TCCR3B = 0x0A;
	TCCR1B = 0b11111000;
	
	sei();
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("Y-");
	PORTF &= ~(1 << PF2); // set direction 0
	_delay_ms(950); // delay ??????????????????????
	Y--;
	cli();
	return;
}
////Big Step
void X_Axis_Big_Step_Plus()
{
	TCCR1B = 0x09; //set timer1
	TCCR3B = 0b11111000;//kill timer3
	
	sei(); //start PWM
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("X+");
	PORTF |= (1 << PF0); // set direction 1
	_delay_ms(1400); // delay ??????????????????????????????????????????????????????
	X++;
	cli();// stop global interrupts
	return;
}
void X_Axis_Big_Step_Minus()
{
	TCCR1B = 0x09;
	TCCR3B = 0b11111000;
	
	sei();
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("X-");
	PORTF &= ~(1 << PF0); // set direction 0
	_delay_ms(1400); // delay ??????????????????????????????????????????????????????????
	X--;
	cli();
	return;
}
void Y_Axis_Big_Step_Plus()
{
	TCCR3B = 0x0A;
	TCCR1B = 0b11111000;
	
	sei();
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("Y+");
	PORTF |= (1 << PF2); // set direction 1
	_delay_ms(1500); // delay ????????????????????????????????????????????????????????????
	Y++;
	cli();
	return;
}
void Y_Axis_Big_Step_Minus()
{
	TCCR3B = 0x0A;
	TCCR1B = 0b11111000;
	
	sei();
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("Y-");
	PORTF &= ~(1 << PF2); // set direction 0
	_delay_ms(1500); // delay ?????????????????????????????????????????????????????????
	Y--;
	cli();
	return;
}
////
void Go_To_Zero()
{
	if (X == 0 && Y == 0)
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Zero Set Zero");
		_delay_ms(1000);
	}
	else
	{
		for (; X > 0; X--)//(int j = 5; j > 0; j--)
		{
			X_Axis_Step_Minus();
			//_delay_ms(500);
		}
		for (; Y > 0; Y--)
		{
			Y_Axis_Step_Minus();
			//_delay_ms(500);
		}
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Zero Set");
	}
} 
int Go_To (int X_Axis, int Y_Axis) //int STOP_X, int STOP_Y
{
	int flagdir = 0;
	char Count = 0;
	char Xtemp;
	char Ytemp;
	for (Ytemp = 0; Ytemp < Y_Axis; Ytemp++)
	{
		if (STOP_Y == 0)// switch check 
		{
			if (Ytemp == 0)
			{
				//nada 
			}
			else
			{
				if (Capacity == 1)
				{
					if (Count == No_Cups - 1)
					{
						return;
					}
					Fill();
					//Fill_TEST();
					Count++;
				}
				else
				{
					if (Count == No_Cups - 1)
					{
						return;
					}
					FillHalf();
					Count++;
				}
				if (Y_Axis < 5)//X_Axis < 3
				{
					Y_Axis_Big_Step_Plus();
				}
				else
				{
					Y_Axis_Step_Plus();
				}
				_delay_ms(500);
				flagdir++;
			}
		}
		else
		{
			//STOP_Y = 0;
			return;
		} 
		
		if (flagdir % 2)// switch direction
		{
			for (Xtemp = 0; Xtemp < X_Axis; Xtemp++)
			{
				if (STOP_X == 0)
				{
					if (Capacity == 1)
					{
						if (Count == No_Cups - 1)
						{
							return;
						}
						Fill();
						//Fill_TEST();
						Count++;
					}
					else
					{
						if (Count == No_Cups - 1)
						{
							return;
						}
						FillHalf();
						Count++;
					}
					if (Y_Axis < 5)
					{
						X_Axis_Big_Step_Minus();
					}
					else
					{
						X_Axis_Step_Minus();
					}
					_delay_ms(500);
				}
				else
				{
					//STOP_X = 0;
					return;
				}
			}
			if (Xtemp == X_Axis && Ytemp == Y_Axis - 1)// fill last position
			{
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("IN Axis Minus");
				_delay_ms(2000);
				if (Capacity == 1)
				{
					//if (Count == No_Cups - 1)
					//{
					//	return;
					//}
					Fill();
					//Fill_TEST();
					Count++;
				}
				else
				{
					//if (Count == No_Cups - 1)
					//{
					//	return;
					//}
					FillHalf();
					Count++;
				}
			}
		}
		else
		{
			for (Xtemp = 0; Xtemp < X_Axis; Xtemp++)
			{
				if (STOP_X == 0)
				{
					if (Capacity == 1)
					{
						if (Count == No_Cups - 1)
						{
							return;
						}
						Fill();
						//Fill_TEST();
						Count++;
					}
					else
					{
						if (Count == No_Cups - 1)
						{
							return;
						}
						FillHalf();
						Count++;
					}
					if (Y_Axis < 5)
					{
						X_Axis_Big_Step_Plus();
					}
					else
					{
						X_Axis_Step_Plus();
					}
					_delay_ms(500);
				}
				else
				{
					//STOP_X = 0;
					return;
				}
			}
			if (Xtemp == X_Axis && Ytemp == Y_Axis - 1)// fill last position
			{
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("IN Axis Plus");
				_delay_ms(2000);
				if (Capacity == 1)
				{
					//if (Count == No_Cups - 1)
					//{
					//	return;
					//}
					Fill();
					//Fill_TEST();
					Count++;
				}
				else
				{
					//if (Count == No_Cups - 1)
					//{
					//	return;
					//}
					FillHalf();
					Count++;
				}
			}
		}
	}
	return;
	
}

unsigned long Read_HX711_Right(void)
{
	unsigned long count;
	unsigned char i;
	
	HX711_DT_Right |= (1 << PE1);
	_delay_us(1);
	HX711_SCK_Right &= ~(1 << PE0);
	_delay_us(1);
	
	count=0;
	while(HX711_DT_Right == 1);
	for(i=0;i<24;i++)
	{
		//PORTG = 0xFF;
		HX711_SCK_Right |= (1 << PE0);
		_delay_us(1);
		count=count<<1;
		HX711_SCK_Right &= ~(1 << PE0);
		_delay_us(1);
		if(!(bit_is_clear(PINE, PE1)))
		{
			//PORTG = 0x00;
			//_delay_ms(500);
			count++;
		}
	}
	HX711_SCK_Right |= (1 << PE0);
	count ^= 0x800000;
	_delay_us(1);
	HX711_SCK_Right &= ~(1 << PE0);
	_delay_us(1);

	return(count);
}
void Get_Zero_Weight_Right()
{
	HX711_Buffer_Right = Read_HX711_Right();
	Weight_Zero_Right = HX711_Buffer_Right/100;
}
unsigned int Get_Weight_Right()
{
	HX711_Buffer_Right = Read_HX711_Right();
	HX711_Buffer_Right = HX711_Buffer_Right/100;

	Weight_Right = HX711_Buffer_Right;
	Weight_Right = (Weight_Right - Weight_Zero_Right);// -1.6764 Rade's constant (variable to grams)
	Weight_Right = (unsigned int)(((float)Weight_Right/7.35+0.05)*(-1.3641));//1.71//.7087///7126//.7002//(-1.6764));
	//1083 bez opterecenja
	return Weight_Right;
}

unsigned long Read_HX711_Left(void)
{
	unsigned long count;
	unsigned char i;
	
	HX711_DT_Left |= (1 << PB4); //HX711_DT_Left |= (1 << PL1);
	_delay_us(1);
	HX711_SCK_Left &= ~(1 << PB0); //HX711_SCK_Left &= ~(1 << PL0);
	_delay_us(1);
	
	count=0;
	while(HX711_DT_Left == 1); //PORTL HX711_DT_Left == 1
	for(i=0;i<24;i++)
	{
		//PORTG = 0xFF;
		HX711_SCK_Left |= (1 << PB0); //HX711_SCK_Left |= (1 << PL0);
		_delay_us(1);
		count=count<<1;
		HX711_SCK_Left &= ~(1 << PB0); //HX711_SCK_Left &= ~(1 << PL0);
		_delay_us(1);
		if(!(bit_is_clear(PINB, PB4)))
		{
			count++;
		}
	}
	HX711_SCK_Left |= (1 << PB0); //HX711_SCK_Left |= (1 << PL0);
	count ^= 0x800000;
	_delay_us(1);
	HX711_SCK_Left &= ~(1 << PB0); //HX711_SCK_Left &= ~(1 << PL0);
	_delay_us(1);

	return(count);
}
void Get_Zero_Weight_Left()
{
	HX711_Buffer_Left = Read_HX711_Left();
	Weight_Zero_Left = HX711_Buffer_Left/100;
	//Weight_Zero_Left = (unsigned int)(((float)Weight_Zero_Left/7.35+0.05)*(-1.6168));
}
unsigned int Get_Weight_Left()
{
	unsigned  MyWeight [16];
	HX711_Buffer_Left = Read_HX711_Left();
	HX711_Buffer_Left = HX711_Buffer_Left/100;

	Weight_Left = HX711_Buffer_Left;
	Weight_Left = (Weight_Left - Weight_Zero_Left);// -1.6168 Rade's constant (variable to grams)
	Weight_Left = (unsigned int)(((float)Weight_Left/7.35+0.05)*(-1.7018));//(-1.6168));
	
	return Weight_Left;
}  

void FillHalf()
{
	///MOZDA NIJE DOBRO treba srediti ovu funkciju
	PORTK = (1<<PK5);// Open AIR
	
	Get_Zero_Weight_Left();
	//_delay_ms(10);
	//Get_Zero_Weight_Right();
	_delay_ms(300);
	
	//////OVO MORA
	_delay_ms(600);//?????????????????????????????????????????????????????????????????????
	for(int i=0;i<2;i++)
	{
		Global_Weight_Left = Get_Weight_Left();
		_delay_ms(150);
	}
	_delay_ms(10);
	//for(int i=0;i<2;i++)
	//{
	//	Global_Weight_Right = Get_Weight_Right();
	//	_delay_ms(150);
	//}
	int Init_Grams_Left  = Global_Weight_Left;
	//int Init_Grams_Right  = Global_Weight_Right;
	
	char tempValve = 0;
	//PORTK = (1<<PK4) | (1<<PK5);// Open Valve
	while (abs(Global_Weight_Left-Init_Grams_Left) < Grams)//Grams
	{
		if (tempValve == 0)
		{
			PORTK = (1<<PK4) | (1<<PK5) | (0<<PK7);// Open Valve
		}
		if ((Global_Weight_Left-Init_Grams_Left) >= Grams)
		{
			PORTK = (0<<PK4) | (1<<PK5) | (0<<PK7);
		}
		//if ((Global_Weight_Right-Init_Grams_Right) >= Grams)
		//{
		//	PORTK = (1<<PK4) | (0<<PK7) | (1<<PK5);
		//}
		tempValve = 1;
			
		Global_Weight_Left = Get_Weight_Left();
		//_delay_ms(10);
		//Global_Weight_Right = Get_Weight_Right();
		_delay_ms(110);
	}
	PORTK = (0<<PK4) | (1<<PK5) | (0<<PK7);// Close Valve
	
	int oldSum_L = Sum_L;
	//int oldSum_R = Sum_R;
	
	tempValve = 0;
	
	Sum_L = Sum_L + abs(Global_Weight_Left-Init_Grams_Left);
	//Sum_R = Sum_R + abs(Global_Weight_Right-Init_Grams_Right);
	
	if (Sum_L - oldSum_L > Grams + 50 || Global_Weight_Left - Init_Grams_Left > Grams + 2//4
									  || Global_Weight_Left - Init_Grams_Left < Grams - 2)//4
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Sum overflow L");
		_delay_ms(2000);
		errorFill_L[countErrors_L] = countFill;
		countErrors_L++;
	}
	_delay_ms(10);
	//if (Sum_R - oldSum_R > Grams + 50 || Global_Weight_Right - Init_Grams_Right > Grams + 2//4
	//								  || Global_Weight_Right - Init_Grams_Right < Grams - 2)//4
	//{
	//	Lcd4_Clear();
	//	Lcd4_Set_Cursor(1,0);
	//	Lcd4_Write_String("Sum overflow R");
	//	_delay_ms(2000);
	//	errorFill_R[countErrors_R] = countFill;
	//	countErrors_R++;
	//}
	countFill++;
	_delay_ms(10);
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Fill Complete");
	_delay_ms(1000);
	PORTK = (0<<PK5) | (0<<PK4) | (0<<PK7);// Kill AIR
}
void Fill()
{ 
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Filling...");
	// |= (1 << PB4);
	// &= ~(1 << PB0);
	//PORTK = (1<<PK5);// Open AIR
	PORTK |= (1<<PK5);
	
	Get_Zero_Weight_Left();
	_delay_ms(10);
	Get_Zero_Weight_Right();
	_delay_ms(10);//300
	
	//Init_Grams_Left = Get_Weight_Left();
	//Global_Weight_Left = Get_Weight_Left();
	//Global_Weight_Right = Get_Weight_Right();
	
	//Lcd4_Clear();
	
	//////OVO MORA 
	_delay_ms(100);//600
	for(int i=0;i<4;i++)
	{
	  Global_Weight_Left = Get_Weight_Left();
	  _delay_ms(300);//300
	}
	_delay_ms(10);
	for(int i=0;i<4;i++)
	{
		Global_Weight_Right = Get_Weight_Right();
		_delay_ms(300);
	}  
	int Init_Grams_Left  = Global_Weight_Left;  
	int Init_Grams_Right  = Global_Weight_Right;
	
	//itoa(Init_Grams_Left, MyWeight, 10);
	//_delay_ms(10);
	//itoa(Init_Grams_Right, MyWeight2, 10);
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String(MyWeight);
	//Lcd4_Set_Cursor(2,0);
	//Lcd4_Write_String(MyWeight2);
	//_delay_ms(2000);
	
	char tempValve = 0;
	int TEMP_Grams_Left = 0;
	int TEMP_Grams_Right = 0;
	//PORTK = (1<<PK4) | (1<<PK5);// Open Valve
	while ((abs(Global_Weight_Left-Init_Grams_Left) < Grams) || (abs(Global_Weight_Right-Init_Grams_Right) < Grams))//Grams 
	{
		if (tempValve == 0)
		{         
			//PORTK = (1<<PK4) | (1<<PK5) | (1<<PK7);// Open Valve
			PORTK |= (1<<PK4);
			PORTK |= (1<<PK7);
		}
		if ((Global_Weight_Left-Init_Grams_Left) >= Grams)
		{
			//PORTK = (0<<PK4) | (1<<PK5) | (1<<PK7);
			PORTK &= ~(1<<PK4);//0 11111111111111111111111111111111111111111
			PORTK |= (1<<PK7);//1 111111111111111111111111111111111111111111
		}
		if ((Global_Weight_Right-Init_Grams_Right) >= Grams)
		{
			//PORTK = (1<<PK4) | (0<<PK7) | (1<<PK5);
			PORTK |= (1<<PK4);//1
			PORTK &= ~(1<<PK7);//0
		}
		tempValve = 1;
		
		////_delay_ms(200);//400//700
		//itoa((Global_Weight_Left-Init_Grams_Left), MyWeight, 10);
		//_delay_ms(10);
		//itoa((Global_Weight_Right-Init_Grams_Right), MyWeight2, 10);
		//Lcd4_Clear();
		//Lcd4_Set_Cursor(1,0);
		//Lcd4_Write_String(MyWeight);
		//Lcd4_Set_Cursor(2,0);
		//Lcd4_Write_String(MyWeight2);
		////_delay_ms(10);//700
		////_delay_ms(150);//200ok
		
		Global_Weight_Left = Get_Weight_Left();
		if (abs(Global_Weight_Left-Init_Grams_Left) > (Grams * 5))
		{
			Global_Weight_Left = TEMP_Grams_Left;
		}
		TEMP_Grams_Left = Global_Weight_Left;
		//_delay_ms(150);
		_delay_ms(10);
		Global_Weight_Right = Get_Weight_Right();
		if (abs(Global_Weight_Right-Init_Grams_Right) > (Grams * 5))
		{
			Global_Weight_Right = TEMP_Grams_Right;
		}
		TEMP_Grams_Right = Global_Weight_Right;
		_delay_ms(110);
	}
	//PORTK = (0<<PK4) | (1<<PK5) | (0<<PK7);// Close Valve
	PORTK &= ~(1<<PK4);
	PORTK &= ~(1<<PK7);
	
	int oldSum_L = Sum_L;
	int oldSum_R = Sum_R;
	
	tempValve = 0;
	
	Sum_L = Sum_L + abs(Global_Weight_Left-Init_Grams_Left);
	Sum_R = Sum_R + abs(Global_Weight_Right-Init_Grams_Right);
	
	if ((Sum_L - oldSum_L) > (Gram_Target + 50) || (Global_Weight_Left - Init_Grams_Left) > (Gram_Target + 2)//3//4 
								  || (Global_Weight_Left - Init_Grams_Left) < (Gram_Target - 2))//3//4
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Sum overflow L");
		_delay_ms(2000);
		errorFill_L[countErrors_L] = countFill;
		countErrors_L++;
	}
	_delay_ms(10);
	if ((Sum_R - oldSum_R) > (Gram_Target + 50) || (Global_Weight_Right - Init_Grams_Right) > (Gram_Target + 2)//3//4
								  || (Global_Weight_Right - Init_Grams_Right) < (Gram_Target - 2))//3//4
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Sum overflow R");
		_delay_ms(2000);
		errorFill_R[countErrors_R] = countFill;
		countErrors_R++;
	}
	countFill++;
	
	//itoa(Sum_L, MyWeight, 10);
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("SUM_L");
	//Lcd4_Set_Cursor(2,0);
	//Lcd4_Write_String(MyWeight);
	//_delay_ms(2000);
	
	_delay_ms(10);
	//int Real_Grams_Left = 0;
	//for(int i=0;i<2;i++)
	//{
	//	Real_Grams_Left = Get_Weight_Left();
	//}
	//if (Real_Grams_Left - Global_Weight_Left > 5)
	//{
	//	itoa((Real_Grams_Left - Global_Weight_Left), MyWeight, 10);
	//	Lcd4_Clear();
	//	Lcd4_Set_Cursor(1,0);
	//	Lcd4_Write_String("Target fail");
	//	Lcd4_Set_Cursor(2,0);
	//	Lcd4_Write_String(MyWeight);
	//	_delay_ms(4000);
	//}
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Fill Complete");
	_delay_ms(1000);
	//PORTK = (1<<PK5) | (0<<PK4) | (0<<PK7);// Kill AIR
}

void SetWeight(void)
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Weight:");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("0");
	while(1)
	{
		if (!(bit_is_clear(PINL, PL5)))
		{
			if (Gram_Target >= 150)
			{
				X_Axis = 3;//velike case
				Y_Axis = 3;//3 bilo
			}
			else 
			{
				X_Axis = 3;//male case
				Y_Axis = 5;
			}
			return;
		}
		else if (!(bit_is_clear(PINL, PL6)))//UP
		{
			Gram_Target = Gram_Target + 5;
			itoa(Gram_Target, MyWeight, 10);
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Weight:");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String(MyWeight);
			_delay_ms(100);//700 
		}
		else if (!(bit_is_clear(PINL, PL7)))//DOWN
		{
			Gram_Target = Gram_Target - 5;
			itoa(Gram_Target, MyWeight, 10);
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Weight:");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String(MyWeight);
			_delay_ms(100);//700
		}
	}
}
void SetCups(void)
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Cups:");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("20");
	if (Y_Axis < 5)
	{
		No_Cups = 12;
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Cups:");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("12");
	}
	
	while(1)
	{
		if (!(bit_is_clear(PINL, PL5)))
		{
			return;
		}
		else if (!(bit_is_clear(PINL, PL6)))//UP
		{
			No_Cups = No_Cups + 1;
			itoa(No_Cups, MyWeight, 10);
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Cups:");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String(MyWeight);
			_delay_ms(300);//700
		}
		else if (!(bit_is_clear(PINL, PL7)))//DOWN
		{
			No_Cups = No_Cups - 1;
			itoa(No_Cups, MyWeight, 10);
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Cups:");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String(MyWeight);
			_delay_ms(300);//700
		}
	}
}
void SetCapacity(void)
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Capacity:");
	Lcd4_Set_Cursor(2,0);
	Lcd4_Write_String("Full");
	while(1)
	{
		if (!(bit_is_clear(PINL, PL5)))
		{
			return;
		}
		else if (!(bit_is_clear(PINL, PL6)))//UP
		{
			Capacity = 1;
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Capacity:");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String("Full");
			_delay_ms(300);//700
		}
		else if (!(bit_is_clear(PINL, PL7)))//DOWN
		{
			Capacity = 0;
			Lcd4_Clear();
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Capacity:");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String("Half");
			_delay_ms(300);//700
		}
	}
}

void Fill_TEST()
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Filling...");
	// |= (1 << PB4);
	// &= ~(1 << PB0);
	//PORTK = (1<<PK5);// Open AIR
	PORTK |= (1<<PK5);
	
	Get_Zero_Weight_Left();
	_delay_ms(10);
	
	
	//Init_Grams_Left = Get_Weight_Left();
	//Global_Weight_Left = Get_Weight_Left();
	//Global_Weight_Right = Get_Weight_Right();
	
	//Lcd4_Clear();
	
	//////OVO MORA
	_delay_ms(100);//600
	for(int i=0;i<4;i++)
	{
		Global_Weight_Left = Get_Weight_Left();
		_delay_ms(300);//300
	}
	_delay_ms(10);
	
	int Init_Grams_Left  = Global_Weight_Left;
	
	
	//itoa(Init_Grams_Left, MyWeight, 10);
	//_delay_ms(10);
	//itoa(Init_Grams_Right, MyWeight2, 10);
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String(MyWeight);
	//Lcd4_Set_Cursor(2,0);
	//Lcd4_Write_String(MyWeight2);
	//_delay_ms(2000);
	
	char tempValve = 0;
	int TEMP_Grams_Left = 0;
	
	//PORTK = (1<<PK4) | (1<<PK5);// Open Valve
	while (abs(Global_Weight_Left-Init_Grams_Left) < Grams)// || (abs(Global_Weight_Right-Init_Grams_Right) < Grams))//Grams
	{
		if (tempValve == 0)
		{
			//PORTK = (1<<PK4) | (1<<PK5) | (1<<PK7);// Open Valve
			PORTK |= (1<<PK4);
			PORTK |= (1<<PK7);
		}
		if ((Global_Weight_Left-Init_Grams_Left) >= Grams)
		{
		//PORTK = (0<<PK4) | (1<<PK5) | (1<<PK7);
			PORTK &= ~(1<<PK4);//0 
			PORTK |= (1<<PK7);//1 
		}
		
		tempValve = 1;
		
		////_delay_ms(200);//400//700
		//itoa((Global_Weight_Left-Init_Grams_Left), MyWeight, 10);
		//_delay_ms(10);
		//itoa((Global_Weight_Right-Init_Grams_Right), MyWeight2, 10);
		//Lcd4_Clear();
		//Lcd4_Set_Cursor(1,0);
		//Lcd4_Write_String(MyWeight);
		//Lcd4_Set_Cursor(2,0);
		//Lcd4_Write_String(MyWeight2);
		////_delay_ms(10);//700
		////_delay_ms(150);//200ok
		
		Global_Weight_Left = Get_Weight_Left();
		if (abs(Global_Weight_Left-Init_Grams_Left) > (Grams * 5))
		{
			Global_Weight_Left = TEMP_Grams_Left;
		}
		TEMP_Grams_Left = Global_Weight_Left;
		//_delay_ms(150);
		
		_delay_ms(110);
	}
	//PORTK = (0<<PK4) | (1<<PK5) | (0<<PK7);// Close Valve
	PORTK &= ~(1<<PK4);
	PORTK &= ~(1<<PK7);
	
	int oldSum_L = Sum_L;
	
	tempValve = 0;
	
	Sum_L = Sum_L + abs(Global_Weight_Left-Init_Grams_Left);
	
	
	if ((Sum_L - oldSum_L) > (Gram_Target + 50) || (Global_Weight_Left - Init_Grams_Left) > (Gram_Target + 2)//3//4
	|| (Global_Weight_Left - Init_Grams_Left) < (Gram_Target - 2))//3//4
	{//MOZA = POMOGNE
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Sum overflow L");
		_delay_ms(2000);
		errorFill_L[countErrors_L] = countFill;
		countErrors_L++;
	}
	_delay_ms(10);
	
	countFill++;
	
	//itoa(Sum_L, MyWeight, 10);
	//Lcd4_Clear();
	//Lcd4_Set_Cursor(1,0);
	//Lcd4_Write_String("SUM_L");
	//Lcd4_Set_Cursor(2,0);
	//Lcd4_Write_String(MyWeight);
	//_delay_ms(2000);
	
	_delay_ms(10);
	//int Real_Grams_Left = 0;
	//for(int i=0;i<2;i++)
	//{
	//	Real_Grams_Left = Get_Weight_Left();
	//}
	//if (Real_Grams_Left - Global_Weight_Left > 5)
	//{
	//	itoa((Real_Grams_Left - Global_Weight_Left), MyWeight, 10);
	//	Lcd4_Clear();
	//	Lcd4_Set_Cursor(1,0);
	//	Lcd4_Write_String("Target fail");
	//	Lcd4_Set_Cursor(2,0);
	//	Lcd4_Write_String(MyWeight);
	//	_delay_ms(4000);
	//}
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Fill Complete");
	_delay_ms(1000);
	//PORTK = (1<<PK5) | (0<<PK4) | (0<<PK7);// Kill AIR
}

/*
unsigned long HX711_Read(void)
{
	unsigned long count;
	unsigned char i;
	//unsigned int Flag = 0;
	//|= (1 << PF0); // set direction 1
	//&= ~(1 << PF0); // set direction 0

	HX711_DT |= (1 << PE1); //|= (1 << PB4);//

	_delay_us(1);
	HX711_SCK &= ~(1 << PE0);
	_delay_us(1);

	count=0;
	while(HX711_DT == 1);
	for(i=0;i<24;i++)
	{
		//PORTG = 0xFF;//LED ON
		HX711_SCK |= (1 << PE0);
		_delay_us(1);
		count=count<<1;
		HX711_SCK &= ~(1 << PE0);
		_delay_us(1);
	
		if(!(bit_is_clear(PINE, PE1)))//HX711_DT == 1
		{
			//PORTG = 0x00;
			//_delay_ms(1000);
			count++;
		}
	}
	HX711_SCK |= (1 << PE0);

	count ^= 0x800000;
	_delay_us(1);
	HX711_SCK &= ~(1 << PE0);
	_delay_us(1);

	return(count);
}
void Get_Maopi()
{
	char MyBuffer[16];

	HX711_Buffer = HX711_Read();
	Weight_Maopi = HX711_Buffer/100;//100
}
unsigned int Get_Weight()
{
	char MyBuffer[16];

	HX711_Buffer = HX711_Read();
	HX711_Buffer = HX711_Buffer/100;// /100

	Weight_Shiwu = HX711_Buffer;
	Weight_Shiwu = (Weight_Shiwu - Weight_Maopi)*(-1.680);// Parametar za prebacivanje iz njegovog broja u grame
	Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/7.35+0.05);

	return Weight_Shiwu;
}
*/
