/*
 * Serial.cpp
 *
 * Created: 4/19/2019 6:17:19 PM
 *  Author: Shahe
 */ 

#include "Serial.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#define	F_CPU 16000000UL
#define BAUD 9600
#include <util/setbaud.h>

static volatile t_buffer g_rx_buff;
static volatile t_buffer g_tx_buff;


void serialBegin()
{
	UCSR0B = (1 << RXEN0) | (1 << TXEN0)|(1 << RXCIE0)|(1<<UDRIE0);
	UCSR0C = (3<<UCSZ00);
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	g_rx_buff.head = 0;
	g_rx_buff.tail = 0;
	g_tx_buff.head = 0;
	g_tx_buff.tail = 0;
}


ISR(USART_RX_vect)
{
	if (bit_is_clear(UCSR0A, FE0)) {
		volatile unsigned char data = UDR0;
		volatile unsigned char next = ((g_rx_buff.head + 1) % SERIAL_RX_RING_SIZE);
		if (next != g_rx_buff.tail) {
			g_rx_buff.ring[g_rx_buff.head] = data;
			g_rx_buff.head = next;
		}
	}
	else {
		volatile unsigned char data __attribute__((unused)) = UDR0;
	}
}

ISR(USART_UDRE_vect) {
	
	if (g_tx_buff.head != g_tx_buff.tail) {
		UDR0 = g_tx_buff.ring[g_tx_buff.tail];
		g_tx_buff.tail = (g_tx_buff.tail + 1) % SERIAL_TX_RING_SIZE;
	}
	else {
		// mask the interrupt everything has been send
		UCSR0B &= ~(1<<UDRIE0);
	}
}



unsigned char serial_sendc(unsigned char a_data) {

	uint8_t n = 0x00;
	uint8_t next = ((g_tx_buff.head + 1) % SERIAL_TX_RING_SIZE);
	if (next != g_tx_buff.tail) {
		g_tx_buff.ring[g_tx_buff.head] = a_data;
		g_tx_buff.head = next;
		n = 1;
		UCSR0B |= (1<<UDRIE0);
	}

	return n;
}


int serialAvailable()
{
	return ((SERIAL_RX_RING_SIZE + g_tx_buff.tail - g_rx_buff.tail) & (SERIAL_TX_RING_SIZE-1));
}


unsigned char serialRead(unsigned char *a_data) {
	
	if (g_rx_buff.head == g_rx_buff.tail)
	return 0;
	*a_data = g_rx_buff.ring[g_rx_buff.tail];
	g_rx_buff.tail = (g_rx_buff.tail + 1) % SERIAL_RX_RING_SIZE;
	return 1;
}


void serialPrint(char* msg)
{
	while(*msg != 0x00){
		
		serial_sendc(*msg);	
		msg++;
	}
}
