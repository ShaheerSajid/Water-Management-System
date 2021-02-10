/*
 * Serial.h
 *
 * Created: 4/19/2019 6:17:32 PM
 *  Author: Shahe
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_
#define SERIAL_TX_RING_SIZE 64
#define SERIAL_RX_RING_SIZE 64

typedef struct _t_buffer {
	volatile unsigned char ring[64];
	volatile unsigned char head;
	volatile unsigned char tail;
} t_buffer;

void serialBegin();
void serialPrint(char*);
unsigned char serialRead(unsigned char*);
unsigned char serial_sendc(unsigned char);
int serialAvailable();

#endif /* SERIAL_H_ */