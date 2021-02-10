#include <avr/io.h>
#define	F_CPU	16000000UL
#define UART_BAUD_RATE 9600
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "LCD.h"
#include "spi.h"
#include "mfrc522.h"
#include "Serial.h"


LCD lcd(0,15,14,13,12,11); //rs,en,d4,d5,d6,d7
unsigned char disconnect[] = {0x00,0x12,0x0A,0x06,0x0E,0x0F,0x1E,0x00};
unsigned char connect[] = {0x00,0x02,0x02,0x06,0x0E,0x0E,0x1E,0x00};
unsigned char fl[] = {0x09,0x09,0x12,0x12,0x09,0x09,0x12,0x12};

uint8_t byte;
uint8_t str[MAX_LEN];
uint8_t c = 0x00;
int p = 0;
uint64_t cnt = 0;
double lit = 0;
int units = 0;
int decimal = 0;
double prev = 0;
bool session = false;
bool flow = false;
bool ti = false;
bool ser = false;
bool connected = false;
uint64_t _millis = 0;
uint16_t _1000us = 0;
uint64_t t1 = 0;
uint64_t t2 = 0;
uint64_t t3 = 0;
uint64_t t4 = 0;

ISR(TIMER0_OVF_vect) {
	_1000us += 128;
	while (_1000us > 1000) {
		_millis++;
		_1000us -= 1000;
	}
}
uint64_t millis() {
	uint64_t m;
	TIMSK0  &= ~(1<<TOIE0);
	m = _millis;
	TIMSK0  |= 1<<TOIE0;
	return m;
}





int main(void)
{
	_delay_ms(1000);
	
	DDRC |= (1 << 5)|(1<<4); // Buzzer, valve
	
	
	DDRD &= ~(1 << DDD2);
	PORTD |= (1 << PORTD2);
	EICRA |= (1 << ISC01);
	EIMSK &= ~(1 << INT0);

	TCCR0B |= (1<<CS01);
	TIMSK0  |= 1<<TOIE0;

	
	spi_init();
	mfrc522_init();
	lcd.init();
	lcd.createChar(0,connect);
	lcd.createChar(1,disconnect);
	lcd.createChar(2,fl);
	lcd.setCursor(1,1);
	lcd.print("Water Management");
	lcd.setCursor(1,2);
	lcd.print("System");
	serialBegin();
	sei();
	lcd.clear();
	_delay_ms(1000);
	lcd.setCursor(1,1);
	lcd.print("Place Card");

	
	while(1){
		
		
		if(serialRead(&c))
		{
			p = 0;
			uint8_t buf[20] = {0x00}; //wait for reply
			_delay_ms(100);
			buf[p] = c;
			p++;
			while(serialRead(&c))
			{
				buf[p] = c;
				p++;
			}
			if(buf[0] == 'N'){
				lcd.setCursor(16,2);
				lcd.write(1);
				connected = false;
			}
			else{
				lcd.setCursor(16,2);
				lcd.write(0);
				connected = true;
			}
		}
		if(connected){
			byte = mfrc522_request(PICC_REQALL,str);
			uint8_t str1[MAX_LEN] = {0x00};
			if(byte == CARD_FOUND)
			{
				byte = mfrc522_get_card_serial(str1);
				if(byte == CARD_FOUND)
				{
					
					PORTC |= 1<<5;     //Buzzer
					_delay_ms(100);
					PORTC &= ~(1<<5);
					
					serialPrint("?");
					serialPrint((char*)str1); //Send UID
					_delay_ms(2000);
					
					p = 0;
					uint8_t buf[20] = {0x00}; //wait for reply
					while(!serialRead(&c));
					buf[p] = c;
					p++;
					_delay_ms(100);
					while(serialRead(&c))
					{
						buf[p] = c;
						p++;
					}
					
					
					if(strcmp((char*)buf, "Unidentified") == 0){
						//Print user not found
					}
					else
					{
						//print user details if found
						
						char f = NULL;
						int start = 0;
						int middle = 0;
						int stop = 0;
						unsigned char w[20] = {0x00};
						unsigned char d[5] = {0x00};
						unsigned char *e = NULL;
						while(f != ' ')
						{
							f = buf[start];
							start++;
						}
						f = NULL;
						if(strchr((char*)buf, '.') != NULL)
						{
							while(f != '.')
							{
								f = buf[middle];
								middle++;
							}
						}
						else
						middle = strlen((char*)buf)+1;
						
						
						f = NULL;
						for(int v = 0; v < start-1;v++)
						w[v] = buf[v];
						for(int v = 0; v < (middle-start-1);v++)
						d[v] = buf[v+start];
						if(strchr((char*)buf, '.') != NULL)
						e = &buf[middle];
						else
						e = (unsigned char*)"0";

						
						units = atoi((char*)d);
						decimal = atoi((char*)e);
						prev = units+(float)decimal/100;
						
						lcd.setCursor(1,1);
						lcd.print((char*)w);
						lcd.writec(' ');
						lcd.print((char*)d);
						lcd.writec('.');
						lcd.print((char*)e);
						lcd.writec('L');
						
						session = false;
						if(units > 0)
						{
							EIMSK |= (1 << INT0);
							PORTC |= (1 << 4);
							session = true;
						}
						else
						{
							PORTC &= ~(1 << 4);
							cnt = 0;
							flow = false;
							ser = false;
							EIMSK &= ~(1 << INT0);
							lcd.setCursor(1,1);
							lcd.print("Neg Balance");
							lcd.setCursor(1,2);
							lcd.print("Top up Required");
							_delay_ms(1000);
							lcd.clear();
							_delay_ms(1000);
							lcd.setCursor(1,1);
							lcd.print("Place Card");
							lcd.setCursor(16,2);
							lcd.write(0);
						}
						
						t2 = 0;
						lit = 0;
						while(1 && session)
						{
							uint64_t t5 = millis();
							
							if(ser)
							{
								t3 = millis();
								lcd.setCursor(15,2); // flow start symbol
								lcd.write(2);
								serialPrint("FStart");
								ser = false;
							}
							if(session && flow)
							{
								double val = lit+(((float)(millis()-t3)/1000)*((float)cnt/4500));
								double rt = prev-val;
								float dec = rt - (int)rt;
								dec *= 100;
								if(dec < 0){
									PORTC &= ~(1 << 4);// relay off
									cnt = 0;
									flow = false;
									ser = false;
									EIMSK &= ~(1 << INT0);
									lcd.setCursor(1,1);
									lcd.print("Tank Empty");
									_delay_ms(1000);
									lcd.clear();
									_delay_ms(1000);
									lcd.setCursor(1,1);
									lcd.print("Place Card");
									lcd.setCursor(16,2);
									lcd.write(0);
									session = false;
									break;
								}
								char buffer1[10];
								char buffer2[10];
								itoa((int)rt, buffer1, 10);
								itoa((int)dec, buffer2, 10);
								lcd.setCursor(1,2);
								lcd.print(buffer1);
								lcd.print(".");
								lcd.print(buffer2);
								lcd.print(" L   ");
							}
							if(flow && abs(t5-t1) >= 200)
							{
								EIMSK &= ~(1 << INT0);
								session = false;
								flow = false;
								t4 = millis()-t3-200;
								lit += (((float)t4/1000)*((float)cnt/4500));
								float qw = prev-lit;
								float dec = qw - (int)qw;
								dec *= 100;
								char buffer1[10];
								char buffer2[10];
								itoa((int)qw, buffer1, 10);
								itoa((int)dec, buffer2, 10);
								serialPrint("FStop");
								_delay_ms(10);
								serialPrint(buffer1);
								serialPrint(".");
								serialPrint(buffer2);
								/*p = 0;
								uint8_t buf[20] = {0x00}; //wait for reply
								while(!serialRead(&c));
								buf[p] = c;
								p++;
								_delay_ms(100);
								while(serialRead(&c))
								{
								buf[p] = c;
								p++;
								}*/
								cnt = 0;
								lcd.setCursor(1,2);
								lcd.print(buffer1);
								lcd.print(".");
								lcd.print(buffer2);
								lcd.print(" L   ");
								lcd.setCursor(15,2);
								lcd.writec(' ');
								EIMSK |= (1 << INT0);
								session = true;
								
							}
							if(serialRead(&c))
							{
								p = 0;
								buf[20] = {0x00}; //wait for reply
								_delay_ms(100);
								buf[p] = c;
								p++;
								while(serialRead(&c))
								{
									buf[p] = c;
									p++;
								}
								if(buf[0] == 'O' || buf[0] == 'N'){
									PORTC &= ~(1 << 4);
									cnt = 0;
									flow = false;
									ser = false;
									EIMSK &= ~(1 << INT0);
									if(buf[0] == 'N')
									{
										lcd.setCursor(1,1);
										lcd.print("Disconnected    ");
										lcd.setCursor(16,2);
										lcd.write(1);
										_delay_ms(1000);
										lcd.clear();
										connected = false;
										session = false;
									}
									else{
										lcd.setCursor(1,1);
										lcd.print("Session Timeout");
										_delay_ms(1000);
										lcd.clear();
										_delay_ms(1000);
										lcd.setCursor(1,1);
										lcd.print("Place Card");
										lcd.setCursor(16,2);
										lcd.write(0);
										session = false;
									}
									break;
								}
							}
							
						}
					}
				}
			}
		}
		//_delay_ms(100);
		
	}
}


ISR (INT0_vect)          //External interrupt_zero ISR
{
	if(session){
		if(flow == false)
		{
			flow = true;
			ser = true;
		}
		TIMSK0  &= ~(1<<TOIE0);
		t1 = _millis;
		TIMSK0  |= 1<<TOIE0;
		cnt++;
	}
}