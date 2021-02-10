/*
 * LCD.cpp
 *
 * Created: 3/16/2019 1:18:40 PM
 *  Author: Shahe
 */ 
#include "LCD.h"
#define	F_CPU	16000000UL
#include <util/delay.h>


void LCD::setPin(int pin, int state)
{
	if(pin >= 0 && pin <= 7)
		DDRB |= (state<<pin);
	if(pin >= 8 && pin <= 15)
		DDRD |= (state<<(pin-8));
	if(pin >= 16 && pin <= 23)
		DDRC |= (state<<(pin-16));
}
void LCD::port_clr()
{
	 clrPin(D0);
	 clrPin(D1);
	 clrPin(D2);
	 clrPin(D3);
	 //clrPin(rs);
	// clrPin(en);
}
void LCD::writePin(int pin, int state)
{
	if(pin >= 0 && pin <= 7)
	PORTB |= (state<<pin);
	if(pin >= 8 && pin <= 15)
	PORTD |= (state<<(pin-8));
	if(pin >= 16 && pin <= 23)
	PORTC |= (state<<(pin-16));
}
void LCD::clrPin(int pin)
{
	if(pin >= 0 && pin <= 7)
	PORTB &= ~(1<<pin);
	if(pin >= 8 && pin <= 15)
	PORTD &= ~(1<<(pin-8));
	if(pin >= 16 && pin <= 23)
	PORTC &= ~(1<<(pin-16));
}

void LCD::lcd_data_write(unsigned char data)
{
	
	 port_clr();
	 unsigned char d = data&0xf0;
	 writePin(D0, (d&0x10)==0x10);
	 writePin(D1, (d&0x20)==0x20);
	 writePin(D2, (d&0x40)==0x40);
	 writePin(D3, (d&0x80)==0x80);
	 writePin(rs, 1);
	 writePin(en, 1);
	 _delay_us(1);
	 clrPin(en);
	 _delay_us(100);
	 port_clr();
	 d = data<<4;
	 writePin(D0, (d&0x10)==0x10);
	 writePin(D1, (d&0x20)==0x20);
	 writePin(D2, (d&0x40)==0x40);
	 writePin(D3, (d&0x80)==0x80);
	 writePin(en, 1);
	 _delay_us(1);
	 clrPin(en);
	 _delay_us(100);
}
void LCD::lcd_cmd_write(unsigned char data)
{
	port_clr();
	unsigned char d;
	 d = data&0xf0;
	 writePin(D0, (d&0x10)==0x10);
	 writePin(D1, (d&0x20)==0x20);
	 writePin(D2, (d&0x40)==0x40);
	 writePin(D3, (d&0x80)==0x80);
	clrPin(rs);
	writePin(en, 1);
	_delay_us(1);
	clrPin(en);
	_delay_us(100);
	port_clr();
	 d = data<<4;
	 writePin(D0, (d&0x10)==0x10);
	 writePin(D1, (d&0x20)==0x20);
	 writePin(D2, (d&0x40)==0x40);
	 writePin(D3, (d&0x80)==0x80);
	 writePin(en, 1);
	 _delay_us(1);
	 clrPin(en);
	 _delay_us(100);
}

LCD::LCD(int Rs, int En, int d0, int d1, int d2, int d3)
{
	setPin(rs = Rs, 1);
	setPin(en = En, 1);
	setPin(D0 = d0, 1);
	setPin(D1 = d1, 1);
	setPin(D2 = d2, 1);
	setPin(D3 = d3, 1);
}

void LCD::print(char *data)
{
	//dat = data;
	x = 0;
	while (*data)
		{
			x++;
			lcd_data_write(*data++);
		}
}

void LCD::writec(char c)
{
	lcd_data_write(c);
}
void LCD::print2(char *data)
{
	while (*data)
	{
		lcd_data_write(*data++);
	}
}

void LCD::autoScroll(int row)
{
	
	 /*while(1)
  {
	 int x = sizeof(d1)/sizeof(d1[0])-1;
	 for(int j = 0; j <= x; j++)
	 {
		 for(int i = 0; i < 16; i++)
		 {
			 if(i+j >= x)
			 buf[i] = d1[i+j -x];
			 else
			 buf[i] = d1[i+j];
		 }
		 lcd.setCursor(1,2);
		 lcd.print(buf);
		 _delay_ms(2000);
	 }
  }*/
	 
	for(int j = 0; j <= x; j++)
	{
		for(int i = 0; i < 16; i++)
		{
			if(i+j >= x)
			buf[i] = dat[i+j -x];
			else
			buf[i] = dat[i+j];
		}
		setCursor(1,row);
		print2(buf);
		_delay_ms(300);
	}
}




void LCD::write(unsigned char d)
{
   lcd_data_write(d);
}
void LCD::createChar(unsigned char addr, unsigned char *t)
{
	 lcd_cmd_write(0x40 + (addr*8));   // Set CGRAM Address
	 for (unsigned char i = 0; i <= 7 ; i++)
		  lcd_data_write(t[i]);
	 lcd_cmd_write(0x80);      // Return to Home
}



void LCD::init()
{
	clrPin(en);
	_delay_ms(2);	
	lcd_cmd_write(0x33);
	lcd_cmd_write(0x32);
	lcd_cmd_write(0x28);
	lcd_cmd_write(0x0C);
	lcd_cmd_write(0x01);
	_delay_ms(2);
	lcd_cmd_write(0x06);
}
void LCD::setCursor(int x, int y)
{
	unsigned char adr[] = {0x80, 0xc0};
	lcd_cmd_write(adr[y-1] + x -1);
	_delay_us(100);
}
void LCD::scroll(bool dir)
{
	if(dir)
	lcd_cmd_write(0x18); //left
	else
	lcd_cmd_write(0x1C); //right
}
void LCD::clear()
{
	
	lcd_cmd_write(0x01); 
	lcd_cmd_write(0x02);
}