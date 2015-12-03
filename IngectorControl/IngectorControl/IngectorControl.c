/*
 * IngectorControl.c
 *
 * Created: 11.11.2015 12:07:02
 *  Author: kartsev_pv
 * Project for Atmega8(A/L) CPU 
 * U can use arduino nano with special shield 
 */ 


#define F_CPU 16000000UL //16mhz
#define Bdate __TIMESTAMP__

#define Ing0DDR DDRB
#define Ing0PORT PORTB
#define Ing0PIN PINB
#define Ing0PinNum 3 //Ingector attcahed

#define Ing1DDR DDRB
#define Ing1PORT PORTB
#define Ing1PIN PINB
#define Ing1PinNum 5 //Arduino nano led attached  
//this section added for arduino shield. @ PCB may be not used. 


//-----------------------------------------------------------Drocel
//* here you must select drocel type. 
//* now supported only mechanical drocel with Resistor feedback, attached to ADC

#define DrocelM  //if Drocel mechanics and use ADC
//#define DrocelE  //if drocel use modeling servo.

#ifndef DrocelM
#ifndef DrocelE
#error Bad definer Drocel type No type defined
#endif
#endif


#ifdef DrocelM 
#ifdef DrocelE
#error Bad definer Drocel type You must select one drocel type
#endif
#endif

#ifdef DrocelM
#define PotDrocelDDR DDRC 
#define PotDrocelPORT PORTC
#define PotDrocelPIN PINC
#define PotDrocelPinNum 0 //Drocel pot attached
#endif

#ifdef DrocelE
#error Bad definer Drocel You must wright this code
#endif
//end -----------------------------------------------------------Drocel



#define Pot0DDR DDRC
#define Pot0PORT PORTC
#define Pot0PIN PINC
#define Pot0PinNum 1


#define IgiDDR DDRD
#define IgiPORT PORTD
#define IgiPIN PIND
#define IgiPinNum 2 //int0

#include <avr/io.h>
#include <avr/interrupt.h>


/////////////////////////global variable section

char ADch0 = 0xFF,ADch1 = 0xFF; //used for ADC result. 

/////////////////////////end global variable section



ISR (TIMER2_OVF_vect)
{
	
}
ISR (ADC_vect)
{
	if (ADMUX == 0b01100000)
	{
		//TODO: GET ADC DATA
ADch0 = ADCH;
		ADMUX = 0b01100001;    //(aref, only adch byte used, ch1 used)
	}
	else
	{
		//TODO: GET ADC DATA
		ADch1 = ADCH;
		ADMUX = 0b01100000;   //(aref, only adch byte used, ch0 used)
	}
	//ADMUX = 0x00100001;    //(aref, only adch byte used, ch1 used)

	//value = value + (ADC*11/4); // Суммируем старое и новое значения АЦП, преобразуем
	//adc_counter++; // Увеличиваем счетчик измерений
	//перезапускаем АДЦ
	ADCSRA |=(1<<ADSC); //silgle ADC convert start,
}




int main(void)
{

	//DDRB = 0xff //all B is out
	//DDRB = 0x00 //all B is in
	//PORTD |= _BV(PD3);      // установить "1" на линии 3 порта D
	//PORTD &= ~_BV(PD4);      // установить "0" на линии 4 порта D
	
	Ing0DDR  = _BV(Ing0PinNum);//ingector //out
	Ing0DDR  = _BV(Ing1PinNum);// arduino led //out 
	
	IgiDDR = ~_BV(IgiPinNum); //igition sensor //in
	PotDrocelDDR = ~_BV(PotDrocelPinNum); //Drocel pot attached //in
	Pot0DDR = ~_BV(Pot0PinNum); //Drocel pot attached //in
	
ADCSetup();
sei();

//StartSetup();	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
void ADCSetup(void)
{
	ADMUX = 0b01100000;    //(aref, only adch byte used, ch0 used) 
	//ADMUX = 0b00100001;    //(aref, only adch byte used, ch1 used) 
	
	ADCSRA = 0b10001110; //(ADC EN,
						//silgle ADC convert NOT start,
						//ADC free run OFF,
						//ADC in flag(HW set)
						//ADC int enabled,
						//speed /64(250khz) adps2:0 110 )
						
//SREG |= 						
	  ADCSRA |=(1<<ADSC); //silgle ADC convert start,
}
