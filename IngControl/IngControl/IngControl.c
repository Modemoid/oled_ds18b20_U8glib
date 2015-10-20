/*
 * IngControl.c
 *
 * Created: 20.10.2015 9:10:36
 *  Author: kartsev_pv
 */ 

#define F_CPU 16000000UL //16mhz

#define Ing0DDR DDRB
#define Ing0PORT PORTB
#define Ing0PIN PINB
#define Ing0PinNum 3

#define Ing1DDR DDRB
#define Ing1PORT PORTB
#define Ing1PIN PINB
#define Ing1PinNum 5 //Arduino led attached


#include <avr/io.h>

int main(void)
{
	
	//DDRB = 0xff //all B is out
	//DDRB = 0x00 //all B is in
	
	Ing0DDR  = _BV(Ing0PinNum);
	Ing0DDR  = _BV(Ing1PinNum);
	
	//PORTD |= _BV(PD3);      // ���������� "1" �� ����� 3 ����� D
	//PORTD &= ~_BV(PD4);      // ���������� "0" �� ����� 4 ����� D
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}