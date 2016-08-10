/*
 pins_arduino.h - Pin definition functions for Arduino ATmega128
 Part of Arduino - http://blog.csdn.net/canyue102/article/details/9451771
 
Copyright (c) 2013 Dongyu_canyue102
 
This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
*/
 
#ifndef Pins_Arduino_h
#define Pins_Arduino_h
 
#include <avr/pgmspace.h>
 
#define NUM_DIGITAL_PINS 53
#define NUM_ANALOG_INPUTS 8
#define analogInputToDigitalPin(p) ((p < 8) ? (p) + 45 : -1)
#define digitalPinHasPWM(p) (((p) >= 12 && (p) <= 15) || ((p) >= 35 && (p)<= 37))
 
static const uint8_t SS = 8;
static const uint8_t MOSI = 10;
static const uint8_t MISO = 11;
static const uint8_t SCK = 9;
 
static const uint8_t SDA = 25;
static const uint8_t SCL = 24;
static const uint8_t LED_BUILTIN = 12;
 
static const uint8_t A0 = 40;
static const uint8_t A1 = 41;
static const uint8_t A2 = 42;
static const uint8_t A3 = 43;
static const uint8_t A4 = 44;
static const uint8_t A5 = 45;
static const uint8_t A6 = 46;
static const uint8_t A7 = 47;
 
// A majority of the pins are NOT PCINTs, SO BE WARNED (i.e. you cannot use them as receive pins)
// Only pins available for RECEIVE (TRANSMIT can be on any pin):
// (I've deliberately left out pin mapping to the Hardware USARTs - seems senseless to me)
// Pins: 0-53
 
#define digitalPinToPCICR(p) (((p) >= 0 && (p) <= 53) ? (&PCICR) : ((uint8_t *)0))
 
#define digitalPinToPCICRbit(p) ( (((p) >= 0) && ((p) <= 7)) || (((p) >= 8) && ((p) <=47 )) ? 0 : \
 ( (((p) >= 48) && ((p) <= 52)) ? 2 : \
 0 ) )
 
#define digitalPinToPCMSK(p) ( (((p) >= 0) && ((p) <= 7)) || (((p) >= 8) && ((p) <= 47)) ? (&PCMSK0) : \
 ( (((p) >= 48) && ((p) <= 52)) ? (&PCMSK2) : \
 ((uint8_t *)0) ) )
 
#define digitalPinToPCMSKbit(p) ( (((p) >= 0) && ((p) <= 7)) ? ((p) - 6) : \
 ( ((p) == 8) ? 3 : \
 ( ((p) == 9) ? 2 : \
 ( ((p) == 10) ? 1 : \
 ( ((p) == 11) ? 0 : \
 ( (((p) >= 12) && ((p) <= 52)) ? ((p) - 12) : \
 0 ) ) ) ) ) )
 
#ifdef ARDUINO_MAIN
 
const uint16_t PROGMEM port_to_mode_PGM[] = {
 NOT_A_PORT,
 (uint16_t) &DDRA,
 (uint16_t) &DDRB,
 (uint16_t) &DDRC,
 (uint16_t) &DDRD,
 (uint16_t) &DDRE,
 (uint16_t) &DDRF,
 (uint16_t) &DDRG,
 
};
 
const uint16_t PROGMEM port_to_output_PGM[] = {
 NOT_A_PORT,
 (uint16_t) &PORTA,
 (uint16_t) &PORTB,
 (uint16_t) &PORTC,
 (uint16_t) &PORTD,
 (uint16_t) &PORTE,
 (uint16_t) &PORTF,
 (uint16_t) &PORTG,
 
};
 
const uint16_t PROGMEM port_to_input_PGM[] = {
 NOT_A_PIN,
 (uint16_t) &PINA,
 (uint16_t) &PINB,
 (uint16_t) &PINC,
 (uint16_t) &PIND,
 (uint16_t) &PINE,
 (uint16_t) &PINF,
 (uint16_t) &PING,
 
};
 
const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
 // PORTLIST
 // -------------------------------------------
 PD , // PA 0 ** 0 ** D22
 PD , // PA 1 ** 1 ** D23
 PD , // PA 2 ** 2 ** D24
 PD , // PA 3 ** 3 ** D25
 PD , // PA 4 ** 4 ** D26
 PD , // PA 5 ** 5 ** D27
 PD , // PA 6 ** 6 ** D28
 PD , // PA 7 ** 7 ** D29
 PC , // PB 0 ** 8 ** SPI_MISO
 PC , // PB 1 ** 9 ** SPI_MOSI
 PC , // PB 2 ** 10 ** SPI_SCK
 PC , // PB 3 ** 11 ** SPI_SS
 PC , // PB 4 ** 12 ** PWM10
 PC , // PB 5 ** 13 ** PWM11
 PC , // PB 6 ** 14 ** PWM12
 PC , // PB 7 ** 15 ** PWM13
 PA , // PC 0 ** 16 ** D30
 PA , // PC 1 ** 17 ** D31
 PA , // PC 2 ** 18 ** D32
 PA , // PC 3 ** 19 ** D33
 PA , // PC 4 ** 20 ** D34
 PA , // PC 5 ** 21 ** D35
 PA , // PC 6 ** 22 ** D36
 PA , // PC 7 ** 23 ** D37
 PB , // PD 0 ** 24 ** USART1_TX
 PB , // PD 1 ** 25 ** USART1_RX
 PB , // PD 2 ** 26 ** I2C_SDA
 PB , // PD 3 ** 27 ** I2C_SCL
 PB , // PD 4 ** 28 ** D38
 PB , // PD 5 ** 29 ** I2C_SDA
 PB , // PD 6 ** 30 ** I2C_SCL
 PB , // PD 7 ** 31 ** D38
 PE , // PE 0 ** 32 ** USART0_RX
 PE , // PE 1 ** 33 ** USART0_TX
 PE , // PE 2 ** 34 ** PWM2
 PE , // PE 3 ** 35 ** PWM3
 PE , // PE 4 ** 36 ** PWM5
 PE , // PE 5 ** 37 ** PWM2
 PE , // PE 6 ** 38 ** PWM3
 PE , // PE 7 ** 39 ** PWM5
 PF , // PF 0 ** 40 ** A0
 PF , // PF 1 ** 41 ** A1
 PF , // PF 2 ** 42 ** A2
 PF , // PF 3 ** 43 ** A3
 PF , // PF 4 ** 44 ** A4
 PF , // PF 5 ** 45 ** A5
 PF , // PF 6 ** 46 ** A6
 PF , // PF 7 ** 47 ** A7
 PG , // PG 0 ** 48 ** D39
 PG , // PG 1 ** 49 ** D40
 PG , // PG 2 ** 50 ** D41
 PG , // PG 3 ** 51 ** D41
 PG , // PG 4 ** 52 ** D41
};
 
const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
 // PIN IN PORT
 // -------------------------------------------
 _BV( 0 ) , // PA 0 ** 22 ** D22
 _BV( 1 ) , // PA 1 ** 23 ** D23
 _BV( 2 ) , // PA 2 ** 24 ** D24
 _BV( 3 ) , // PA 3 ** 25 ** D25
 _BV( 4 ) , // PA 4 ** 26 ** D26
 _BV( 5 ) , // PA 5 ** 27 ** D27
 _BV( 6 ) , // PA 6 ** 28 ** D28
 _BV( 7 ) , // PA 7 ** 29 ** D29
 _BV( 0 ) , // PB 3 ** 50 ** SPI_MISO
 _BV( 1 ) , // PB 2 ** 51 ** SPI_MOSI
 _BV( 2 ) , // PB 1 ** 52 ** SPI_SCK
 _BV( 3 ) , // PB 0 ** 53 ** SPI_SS
 _BV( 4 ) , // PB 4 ** 10 ** PWM10
 _BV( 5 ) , // PB 5 ** 11 ** PWM11
 _BV( 6 ) , // PB 6 ** 12 ** PWM12
 _BV( 7 ) , // PB 7 ** 13 ** PWM13
 _BV( 0 ) , // PC 7 ** 30 ** D30
 _BV( 1 ) , // PC 6 ** 31 ** D31
 _BV( 2 ) , // PC 5 ** 32 ** D32
 _BV( 3 ) , // PC 4 ** 33 ** D33
 _BV( 4 ) , // PC 3 ** 34 ** D34
 _BV( 5 ) , // PC 2 ** 35 ** D35
 _BV( 6 ) , // PC 1 ** 36 ** D36
 _BV( 7 ) , // PC 0 ** 37 ** D37
 _BV( 0 ) , // PD 3 ** 18 ** USART1_TX
 _BV( 1 ) , // PD 2 ** 19 ** USART1_RX
 _BV( 2 ) , // PD 1 ** 20 ** I2C_SDA
 _BV( 3 ) , // PD 0 ** 21 ** I2C_SCL
 _BV( 4 ) , // PD 3 ** 18 ** USART1_TX
 _BV( 5 ) , // PD 2 ** 19 ** USART1_RX
 _BV( 6 ) , // PD 1 ** 20 ** I2C_SDA
 _BV( 7 ) , // PD 0 ** 21 ** I2C_SCL
 _BV( 0 ) , // PE 0 ** 0 ** USART0_RX
 _BV( 1 ) , // PE 1 ** 1 ** USART0_TX
 _BV( 2 ) , // PE 4 ** 2 ** PWM2
 _BV( 3 ) , // PE 5 ** 3 ** PWM3
 _BV( 4 ) , // PE 0 ** 0 ** USART0_RX
 _BV( 5 ) , // PE 1 ** 1 ** USART0_TX
 _BV( 6 ) , // PE 4 ** 2 ** PWM2
 _BV( 7 ) , // PE 5 ** 3 ** PWM3
 _BV( 0 ) , // PF 0 ** 54 ** A0
 _BV( 1 ) , // PF 1 ** 55 ** A1
 _BV( 2 ) , // PF 2 ** 56 ** A2
 _BV( 3 ) , // PF 3 ** 57 ** A3
 _BV( 4 ) , // PF 4 ** 58 ** A4
 _BV( 5 ) , // PF 5 ** 59 ** A5
 _BV( 6 ) , // PF 6 ** 60 ** A6
 _BV( 7 ) , // PF 7 ** 61 ** A7
 _BV( 0 ) , // PG 0 ** 62 ** A8
 _BV( 1 ) , // PG 1 ** 63 ** A9
 _BV( 2 ) , // PG 2 ** 64 ** A10
 _BV( 3 ) , // PG 3 ** 65 ** A11
 _BV( 4 ) , // PG 4 ** 66 ** A12
 
};
 
const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
 // TIMERS
 // -------------------------------------------

NOT_ON_TIMER  ,  // PD 0 ** 0 ** D22
NOT_ON_TIMER  ,  // P 1 ** 1 ** D23
NOT_ON_TIMER  ,  // P 2 ** 2 ** D24
NOT_ON_TIMER   , // P 3 ** 3 ** D25
NOT_ON_TIMER   , // P 4 ** 4 ** D26
NOT_ON_TIMER   , // P 5 ** 5 ** D27
NOT_ON_TIMER   , // P 6 ** 6 ** D28
NOT_ON_TIMER   , // P 7 ** 7 ** D29
NOT_ON_TIMER   , // PC 0 ** 8 ** SPI_MISO
NOT_ON_TIMER   , // P 1 ** 9 ** SPI_MOSI
NOT_ON_TIMER   , // P 2 ** 10 ** SPI_SCK
NOT_ON_TIMER   , // P 3 ** 11 ** SPI_SS
NOT_ON_TIMER   , // P 4 ** 12 ** PWM10
NOT_ON_TIMER   , // P 5 ** 13 ** PWM11
NOT_ON_TIMER   , // P 6 ** 14 ** PWM12
NOT_ON_TIMER   , // P 7 ** 15 ** PWM13
NOT_ON_TIMER   , // PA 0 ** 16 ** D30
NOT_ON_TIMER   , // P 1 ** 17 ** D31
NOT_ON_TIMER   , // P 2 ** 18 ** D32
NOT_ON_TIMER   , // P 3 ** 19 ** D33
NOT_ON_TIMER   , // P 4 ** 20 ** D34
NOT_ON_TIMER   , // P 5 ** 21 ** D35
NOT_ON_TIMER   , // P 6 ** 22 ** D36
NOT_ON_TIMER   , // P 7 ** 23 ** D37
NOT_ON_TIMER   , // PB 0 ** 24 ** USART1_TX
NOT_ON_TIMER   , // P 1 ** 25 ** USART1_RX
NOT_ON_TIMER   , // P 2 ** 26 ** I2C_SDA
NOT_ON_TIMER   , // P 3 ** 27 ** I2C_SCL
 TIMER0A   , // P 4 ** 28 ** D38
 TIMER1A   , // P 5 ** 29 ** I2C_SDA
 TIMER1B   , // P 6 ** 30 ** I2C_SCL
TIMER2   ,// TIMER2A   , // P 7 ** 31 ** D38
NOT_ON_TIMER   , // PE 0 ** 32 ** USART0_RX
NOT_ON_TIMER   , // PE 1 ** 33 ** USART0_TX
TIMER2  , // PE 2 ** 34 ** PWM2
TIMER2   , // PE 3 ** 35 ** PWM3
 TIMER3B   , // PE 4 ** 36 ** PWM5 TIMER3C    
 TIMER3C   ,// PE 5 ** 37 ** PWM2
NOT_ON_TIMER   , // PE 6 ** 38 ** PWM3
NOT_ON_TIMER   , // PE 7 ** 39 ** PWM5
NOT_ON_TIMER   , // PF 0 ** 40 ** A0
NOT_ON_TIMER   , // PF 1 ** 41 ** A1
NOT_ON_TIMER   , // PF 2 ** 42 ** A2
NOT_ON_TIMER   , // PF 3 ** 43 ** A3
NOT_ON_TIMER   , // PF 4 ** 44 ** A4
NOT_ON_TIMER   , // PF 5 ** 45 ** A5
NOT_ON_TIMER   , // PF 6 ** 46 ** A6
NOT_ON_TIMER   , // PF 7 ** 47 ** A7
NOT_ON_TIMER   , // PG 0 ** 48 ** D39
NOT_ON_TIMER   , // PG 1 ** 49 ** D40
NOT_ON_TIMER   , // PG 2 ** 50 ** D41
NOT_ON_TIMER   , // PG 3 ** 51 ** D41
NOT_ON_TIMER   , // PG 4 ** 52 ** D41



/* NOT_ON_TIMER , // PA 0 ** 22 ** D22
 NOT_ON_TIMER , // PA 1 ** 23 ** D23
 NOT_ON_TIMER , // PA 2 ** 24 ** D24
 NOT_ON_TIMER , // PA 3 ** 25 ** D25
 NOT_ON_TIMER , // PA 4 ** 26 ** D26
 NOT_ON_TIMER , // PA 5 ** 27 ** D27
 NOT_ON_TIMER , // PA 6 ** 28 ** D28
 NOT_ON_TIMER , // PA 7 ** 29 ** D29
 NOT_ON_TIMER , // PB 0 ** 13 ** PWM13
 NOT_ON_TIMER , // PB 1 ** 50 ** SPI_MISO
 NOT_ON_TIMER , // PB 2 ** 51 *7* SPI_MOSI
 NOT_ON_TIMER , // PB 3 ** 52 ** SPI_SCK
 TIMER0A , // PB 4 ** 53 ** SPI_SS
 TIMER1A , // PB 5 ** 10 ** PWM10
 TIMER1B , // PB 6 ** 11 ** PWM11
 TIMER2A , // PB 7 ** 12 ** PWM12
 NOT_ON_TIMER , // PC 0 ** 30 ** D30
 NOT_ON_TIMER , // PC 1 ** 31 ** D31
 NOT_ON_TIMER , // PC 2 ** 32 ** D32
 NOT_ON_TIMER , // PC 3 ** 33 ** D33
 NOT_ON_TIMER , // PC 4 ** 34 ** D34
 NOT_ON_TIMER , // PC 5 ** 35 ** D35
 NOT_ON_TIMER , // PC 6 ** 36 ** D36
 NOT_ON_TIMER , // PC 7 ** 37 ** D37
 NOT_ON_TIMER , // PD 0 ** 18 ** USART1_TX
 NOT_ON_TIMER , // PD 1 ** 19 ** USART1_RX
 NOT_ON_TIMER , // PD 2 ** 20 ** I2C_SDA
 NOT_ON_TIMER , // PD 3 ** 21 ** I2C_SCL
 NOT_ON_TIMER , // PD 4 ** 19 ** USART1_RX
 NOT_ON_TIMER , // PD 5 ** 20 ** I2C_SDA
 NOT_ON_TIMER , // PD 6 ** 21 ** I2C_SCL
 NOT_ON_TIMER , // PD 7 ** 21 ** I2C_SCL
 NOT_ON_TIMER , // PE 0 ** 0 ** USART0_RX
 NOT_ON_TIMER , // PE 1 ** 1 ** USART0_TX
 NOT_ON_TIMER , // PE 2 ** 2 ** PWM2
 TIMER3A , // PE 3 ** 3 ** PWM3
 TIMER3B , // PE 4 ** 0 ** USART0_RX
 TIMER3C , // PE 5 ** 1 ** USART0_TX
 NOT_ON_TIMER , // PE 6 ** 2 ** PWM2
 NOT_ON_TIMER , // PE 7 ** 3 ** PWM3
 NOT_ON_TIMER , // PG 0 ** 39 ** D39
 NOT_ON_TIMER , // PG 1 ** 40 ** D40
 NOT_ON_TIMER , // PG 2 ** 41 ** D41
 NOT_ON_TIMER , // PG 3 ** 4 ** PWM4
 NOT_ON_TIMER , // PG 4 ** 41 ** D41
*/
};
 
#endif
 
#endif