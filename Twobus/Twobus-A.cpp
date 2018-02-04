#ifndef _TWOBUS_A_CPP
#define _TWOBUS_A_CPP
#define SIMULATE_ON_PROTEUS //Comment this line if testing on real hardware. Proteus contains a bug with AVR interrupts
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Twobus-A.h"

//Definitions
#define TWOB_A_CPORT PORTD //PORTx of Twobus CLK
#define TWOB_A_DPORT PORTD //PORTx of Twobus DAT
#define TWOB_A_CDDR DDRD   //DDRx of Twobus CLK
#define TWOB_A_DDDR DDRD   //DDRx of Twobus DAT
#define TWOB_A_CPIN PIND   //PINx of Twobus CLK
#define TWOB_A_DPIN PIND   //PINx of Twobus DAT
#define TWOB_A_CN PD2      //Pxx of Twobus CLK
#define TWOB_A_DN PD3      //Pxx of Twobus DAT
#define TWOB_A_CINT INT0   //INTx of Twobus CLK
#define TWOB_A_CINC0 ISC01 //ISCxx of Twobus CLK
#define TWOB_A_CINC1 ISC00 //ISCxx of Twobus CLK
#define TWOB_A_CVECT INT0_vect //INTx_vect of Twobus CLK
#define TWOB_A_CSPEED 19200 //Bus clock speed (Hz)
#define TWOB_A_BUFSZ 64    //Receive buffer size
#define TWOB_A_DLY (500000UL / TWOB_A_CSPEED) //Bus clock delay (us)

volatile uint8_t recv_state = 0; //Receiving state
//0 - Waiting for start
//1 - ???
//2 - Receiving receiver address
//3 - Receiving sender address
//4 - Receiving message length low
//5 - Receiving message length high
//6 - Receiving message
volatile bool lineBusy = false;              //Line busy indicator
volatile bool needReceive = false;           //Address matched indicator
volatile uint8_t Twobus_A_sender_address = 0; //Sender address
volatile uint8_t receivedBit = 0;            //Number of received bit
volatile uint8_t receivedByte = 0;           //Received byte
volatile uint16_t receiveLen = 0;            //Bytes of message received
volatile uint8_t Twobus_A_address = 1;       //This device address
volatile uint8_t Twobus_A_message[TWOB_A_BUFSZ]; //Receive buffer
volatile uint16_t Twobus_A_message_len = 1;  //Message length
volatile bool Twobus_A_message_encrypted = false; //Message encrypted indicator
volatile bool __proteus_ignore = false;      //Proteus bugs correction

ISR(TWOB_A_CVECT){
	#ifdef SIMULATE_ON_PROTEUS
	if(!__proteus_ignore){
		__proteus_ignore = true;
		PORTB = 0xFF;
		return;
	}
	#endif
	bool bit = TWOB_A_DPIN & (1 << TWOB_A_DN);
	receivedByte |= (uint8_t)bit << receivedBit;
	receivedBit++;
	if(receivedBit >= 8){ //Byte received
		PORTC = recv_state;
		if(recv_state == 0){ //Start
			lineBusy = true;
			Twobus_A_message_encrypted = (bool)(receivedByte & (1));
			recv_state = 2;
		}
		else if(recv_state == 2){ //Read receiver
			if(receivedByte == Twobus_A_address || receivedByte == 0){
				needReceive = true;
			} else {
				needReceive = false;
			}
			recv_state = 3;
		}
		else if(recv_state == 3){ //Read sender
			Twobus_A_sender_address = receivedByte;
			recv_state = 4;
		}
		else if(recv_state == 4){ //Read length low
			Twobus_A_message_len |= (uint16_t)receivedByte;
			recv_state = 5;
		}
		else if(recv_state == 5){ //Read length high
			Twobus_A_message_len |= ((uint16_t)receivedByte << 8);
			recv_state = 6;
		}
		else if(recv_state == 6){ //Read message
			Twobus_A_message[receiveLen++] = receivedByte;
			if(needReceive)
				PORTB = receivedByte;
			if(receiveLen >= Twobus_A_message_len){
				lineBusy = false;
				recv_state = 0;
				Twobus_A_message_len = 0;
				receiveLen = 0;
			}
		}
		receivedByte = 0;
		receivedBit = 0;
	}
}

void __twobus_a_sendbit(bool bit){ //Send bit to the bus
	if(bit){
		TWOB_A_DPORT |= (1 << TWOB_A_DN);
	}
	else
	{
		TWOB_A_DPORT &= ~(1 << TWOB_A_DN);
	}
	TWOB_A_CPORT |= (1 << TWOB_A_CN);
	_delay_us(TWOB_A_DLY);
	TWOB_A_CPORT &= ~(1 << TWOB_A_CN);
	TWOB_A_DPORT &= ~(1 << TWOB_A_DN);
	_delay_us(TWOB_A_DLY);
}

void __twobus_a_sendbyte(uint8_t data){ //Send byte to the bus
	for(uint8_t i = 0; i < 8; i++){
		__twobus_a_sendbit((bool)(data & (1 << i)));
	}
}

void __twobus_a_broadcast(){ //Begin to broadcast to the bus
	TWOB_A_CDDR |= (1 << TWOB_A_CN);
	TWOB_A_DDDR |= (1 << TWOB_A_DN);
	cli();
	GICR &= ~(1 << TWOB_A_CINT);
	MCUCR &= ~(1 << TWOB_A_CINC0);
	MCUCR &= ~(1 << TWOB_A_CINC1);
	sei();
}

void __twobus_a_listen(){ //Begin to listen to the bus
	TWOB_A_CDDR &= ~(1 << TWOB_A_CN);
	TWOB_A_DDDR &= ~(1 << TWOB_A_DN);
	cli();
	GICR |= (1 << TWOB_A_CINT);
	MCUCR |= (1 << TWOB_A_CINC0);
	MCUCR |= (1 << TWOB_A_CINC1);
	sei();
	receivedBit = 0;
}

void __twobus_a_listen_noint(){ //Begin to listen to the bus without interrupts
	TWOB_A_CDDR &= ~(1 << TWOB_A_CN);
	TWOB_A_DDDR &= ~(1 << TWOB_A_DN);
}

uint8_t Twobus_A_SendPacket(uint8_t destAddr, uint8_t encryption_key, uint16_t timeout, uint8_t data[], uint16_t dlen, bool encrypted){ //Send packet to the bus. Returns:
	//0 - OK
	//1 - Begin timeout error
	while(lineBusy){_delay_ms(1); if(timeout-- == 0){return 1;}};
	__twobus_a_broadcast();
	__twobus_a_sendbyte(128 | (uint8_t)encrypted);
	__twobus_a_sendbyte(destAddr);
	__twobus_a_sendbyte(Twobus_A_address);
	__twobus_a_sendbyte((uint8_t)(dlen & 0x00FF));
	__twobus_a_sendbyte((uint8_t)(dlen >> 8));
	for(uint16_t i = 0; i < dlen; i++){
		if(Twobus_A_message_encrypted){
			__twobus_a_sendbyte(data[i] ^ encryption_key);
			} else {
			__twobus_a_sendbyte(data[i]);
		}
	}
	__twobus_a_listen();
	return 0;
}

void Twobus_A_init(uint8_t addr){ //Initialize the library
	Twobus_A_address = addr;
	__twobus_a_listen();
}
#endif