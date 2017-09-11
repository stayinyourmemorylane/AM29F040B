/*
 * AM29F040B.c
 *
 * Created: 07/10/2013 1:44:45 PM
 *  Author: Michael
 */ 

/// key information on page 12 regarding this//
// A bit can not be programmed from a 0 -> 1, only erase operations can convert a 0 -> 1.
// Write operations can only program a 1 -> 0.  Erase must occur before a data write.
// Byte programming operation - 7us

#include "AM29F040B.h"


void amdWriteCustomLen(char *data, uint32_t starting_address, int length){
	uint32_t  current_address = 0; 
	current_address = starting_address; 

	for (int x=0; x < length; x++) {
		AMD_Write_Data((uint8_t) data[x], (uint32_t) current_address);
		current_address+=1;

	}
	
	return;
}


void amdPortsEnable(void){
	// ENABLE the control pins for output 
	AMD_CTRL_DDR_CEOE   |= (1 << AMD_CE) | (1 << AMD_OE);
	AMD_CTRL_DDR_WE |=  (1 << AMD_WE);
	// Addressing is output, will set data later during read and write instructions
	AMD_ADDR1_DDR |= ALL_PORT_MASK; 
	AMD_ADDR2_DDR |= ALL_PORT_MASK; 
	AMD_ADDR3_DDR |= AMD_ADDR3_MASK; 
	
	AMD_CTRL_PORT_CEOE   |= (1 << AMD_CE) | (1 << AMD_OE);
	AMD_CTRL_PORT_WE |=  (1 << AMD_WE); 

	return; 
}


void amdSetAddress(uint32_t address){
	struct AMD_Settings *Point_AMD = &Data;
	uint32_t split_address= 0; 
	
	split_address = address; 
	split_address &= 0xFF; 
	AMD_ADDR1_PORT = split_address; 
		
	split_address = address; 
	split_address >>=8;
	split_address &= 0xFF; 
	AMD_ADDR2_PORT = split_address;

	split_address = address;
	split_address >>=16;
	split_address &= 0x07;
	split_address >>= 4; 
	AMD_ADDR3_PORT = split_address;
	
	return; 
}

void amdSetData(char data_input){
	struct AMD_Settings *Point_AMD = &Data;
	
	AMD_DATA_PORT = ((uint8_t) data_input); 

	return;
}

char amdReadData(void){
	struct AMD_Settings *Point_AMD = &Data;
	char AMD_DATA_OUT; 
	
	AMD_DATA_OUT = AMD_DATA_PINS;

	return AMD_DATA_OUT;
}

void amdSendCommand(char data, uint32_t address){
	struct AMD_Settings *Point_AMD = &Data;
	AMD_Set_Address((uint32_t) address);
	AMD_Set_Data((uint8_t) data);
		
	CLEARBIT(AMD_CTRL_PORT_WE, AMD_WE);
	_delay_us(2);
	SETBIT(AMD_CTRL_PORT_WE, AMD_WE);
	
	return;
}
 // THIS AND ABOVE WORKS FINE 
 
 
 // all data is 8 bit address should be set before writing
void amdWriteChar(char data, uint32_t address){
	struct AMD_Settings *Point_AMD = &Data;	
	
	AMD_DATA_DDR |= ALL_PORT_MASK;  // SET DATA for output
	// Set the control port for proper Write instructions
	AMD_CTRL_PORT |= (1 << AMD_OE) & ~(1 << AMD_CE);
	CLEARBIT(AMD_CTRL_PORT, AMD_CE);
	//CE STAYS LOW  /OE PULLED HIGH

	// Set the unlock commands for write
	AMD_Send_Command(AMD_DATA_MASK_AA, AMD_ADDR_MASK_555);
	AMD_Send_Command(AMD_DATA_MASK_55, AMD_ADDR_MASK_2AA);
	AMD_Send_Command(AMD_DATA_MASK_A0, AMD_ADDR_MASK_555);	
	// Send the data to the requested address 
	AMD_Send_Command((uint8_t) data, (uint32_t) address);	
	// counts fine 

	AMD_CTRL_PORT &= ~(1<< AMD_OE);
		
	_delay_us(2);
	return; 
}


char amdReadChar(uint32_t address){
	struct AMD_Settings *Point_AMD = &Data;
	char AMD_DATA = 0x00; 

	/// Set all data pins as input 
	AMD_DATA_DDR &= 0x00;
	SETBIT(AMD_CTRL_PORT_WE, AMD_WE);  // Pull WE high

	AMD_Set_Address(address);  // set the address 
	 
	// Set the control port for proper READ instructions
	CLEARBIT(AMD_CTRL_PORT_CEOE, AMD_CE);
	_delay_us(2);
	CLEARBIT(AMD_CTRL_PORT_CEOE,AMD_OE);

	/// we should be able to read the data from here 
	AMD_DATA = AMD_Read_Data_Out(); 

	AMD_CTRL_PORT_CEOE |=  (1 << AMD_OE)| (1 << AMD_CE); 
		
	_delay_us(2);
	return AMD_DATA;
}

char amdEraseSector(char sector){
	// to erase device you must execute the erase command sequence
	// DQ7 is data polling and DQ6 is toggle status bits.
	//refer to the table on page 9 to erase all sectors
	// Erase operation will take approx 8 seconds to run its 1 second per sector
	//65535bits 
	struct AMD_Settings *Point_AMD = &Data;
	uint32_t sector_erase=0; 
	

	AMD_DATA_DDR |= ALL_PORT_MASK;
	SETBIT(AMD_CTRL_PORT_WE, AMD_WE);
	AMD_CTRL_PORT_CEOE |= (1 << AMD_OE) & ~(1 << AMD_CE);
	CLEARBIT(AMD_CTRL_PORT_CEOE, AMD_CE);


	// SEND THE 5 Unlock commands 	
	AMD_Send_Command(AMD_DATA_MASK_AA, AMD_ADDR_MASK_555);
	AMD_Send_Command(AMD_DATA_MASK_55, AMD_ADDR_MASK_2AA);
	AMD_Send_Command(AMD_DATA_MASK_80, AMD_ADDR_MASK_555);
	AMD_Send_Command(AMD_DATA_MASK_AA, AMD_ADDR_MASK_555);
	AMD_Send_Command(AMD_DATA_MASK_55, AMD_ADDR_MASK_2AA);
	
	// Erase specific sectors 
	sector_erase = sector; 
	sector_erase <<= 16; 	
	AMD_Send_Command(AMD_DATA_MASK_30, sector_erase );
	_delay_ms(2);
	
	return;
}

void amdEraseChip(void)
{
	for (char chip=0; chip <= EEPROM_SECTORS; chip++)
	{
		amdEraseSector(chip);
		_delay_ms(2); 
	}

	return;
}

char amdTestDevice(void) {
	char err = EOK; 
	uint8_t chip,chipCompare = 0;
	int8_t   amdReadData = 0; 
	uint32_t amdAddress = 0;

	//test read, erase, write then read
	struct AMD_Settings *Point_AMD= &Data;
	AMD_Ports_Enable();
	
   	printf("\n Erasing sector %d", chip);
	AMD_Chip_Erase(chip);

	for(uint32_t x= 0; x < 255; x++)  {
		AMD_Write_Data((uint8_t)x,(uint32_t)x );
		chipCompare = AMD_Read_Data((uint32_t)x); 
		
		if (chipCompare != x); 
		 {
		 	err = ERROR_AMD_READ_WRITE; 
		 	return err;
		 }
	}

	return err; 
}

