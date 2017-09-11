/*
 * AM29F040B.h
 *
 * Created: 07/10/2013 1:45:03 PM
 *  Author: Michael
 */ 


#ifndef  AM29F040B_H_
#define  AM29F040B_H_
#define  F_CPU    16000000UL
#include "AtmegaPins.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h> // provides delays
#include <avr/power.h> // used for setting clock prescaler
#include <stdbool.h>

/*
 The AMD Chip holds a total of 524,288 bytes of data (2^19) 
  
--------------------------------
--     CHR AMD                --
--------------------------------
A0-7   = Green - PORT C
A8-15  = Green - PORT A
A16-18 = Green - PORT G

D0-7 = Blue - PORT L

CE#		-	PORTE0
OE#		-	PORTE1
WE#		-	PORTE2
*/

#define EEPROM_SECTORS 17
#define EOK 							0 
#define AMD_ERROR_AMD_READ_WRITE		1


#define AMD_CE PA0
#define AMD_OE PA1
#define AMD_WE PA2

#define AMD_CTRL_DDR_CEOE	DDRA
#define AMD_CTRL_PORT_CEOE	PORTA
#define AMD_CTRL_DDR_WE		DDRA
#define AMD_CTRL_PORT_WE	PORTA

// DATA PORT
#define AMD_DATA_PORT PORTF
#define AMD_DATA_PINS PINF
#define AMD_DATA_DDR  DDRF

// ADDRESSING A0-A7 
#define AMD_ADDR1_PORT PORTC
#define AMD_ADDR1_DDR  DDRC

// ADDRESSING A8-A15
#define AMD_ADDR2_PORT PORTA
#define AMD_ADDR2_DDR  DDRA

// ADDRESSING A16-A18 
#define AMD_ADDR3_PORT PORTG  // G0-G1 and G2 are used only 
#define AMD_ADDR3_DDR  DDRG

#define ALL_PORT_MASK	0xFF
#define AMD_ADDR3_MASK	0xFF


/// Define Command Masks for unlock codes 
/// unlock erase
#define AMD_ADDR_MASK_555	0x555  // used for 1 and 3,4
#define AMD_ADDR_MASK_2AA	0x2AA  // used for 2 and 5

#define AMD_DATA_MASK_AA	0xAA // 1 ,4
#define AMD_DATA_MASK_55	0x55 // 2, 5
#define AMD_DATA_MASK_80	0x80 // 3
#define AMD_DATA_MASK_30	0x30 // 6
#define AMD_DATA_MASK_A0	0xA0
#define AMD_DATA_MASK_10	0x10

#define BLANK_MASK 0x0000

void amdWriteCustomLen(char *data, uint32_t starting_address, int length);
void amdPortsEnable(void);
void amdSetAddress(uint32_t address);
void amdSetData(char data_input);
char amdReadData(void);
void amdSendCommand(char data, uint32_t address);
void amdWriteChar(char data, uint32_t address);
char amdReadChar(uint32_t address);
char amdEraseSector(char sector);
void amdEraseChip(void);
char amdTestDevice(void);

struct AMD_Settings{
	uint32_t Address;
	uint8_t Data;
	uint8_t State; 
}Data;

#endif /* AM29F040B_H_ */


