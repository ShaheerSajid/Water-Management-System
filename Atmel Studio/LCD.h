/*
 * LCD.h
 *
 * Created: 3/16/2019 12:56:15 PM
 *  Author: Shahe
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef LCD_H_
#define LCD_H_

class LCD
{
	private:
	int rs;
	int en;
	int D0;
	int D1;
	int D2;
	int D3;
	char *dat;
	int x;
	char buf[16];
	void setPin(int, int);
	void writePin(int ,int);
	void clrPin(int);
	void lcd_data_write(unsigned char);
	void lcd_cmd_write(unsigned char);
	void port_clr();
	void print2(char*);
	
	public:
	LCD(int, int,int ,int ,int , int);
	void print(char*);
	void writec(char);
	void init();
	void write(unsigned char);
	void createChar(unsigned char , unsigned char*);
	void setCursor(int, int);
	void scroll(bool);
	void clear();
	void autoScroll(int);
};
#endif /* LCD_H_ */