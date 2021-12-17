#include "AL01_flash_management.h"


int32_t flash_user_store_erase (void)
{
	uint32_t aux;

	aux = FLASH_ErasePage(USER_FLASH_STORE_BASE_ADDRESS);
	
	return aux;
}


int32_t flash_user_get_first_byte_available_offset (void)
{
	uint8_t *flash = (uint8_t *)USER_FLASH_STORE_BASE_ADDRESS;
	uint32_t i=0;
		
	while(*flash != 0xFF && i<USER_FLASH_LENGTH) 
	{
		flash++;
		i++;
	}
	return i;
}

int32_t flash_user_is_the_flash_empty (void)
{
	return (flash_user_get_first_byte_available_offset() == 0);
}








#if USER_FLAG_REGISTER_FLASH_SIZE == USER_REG_FLASH_SIZE_8BITS
int32_t flash_user_write_1byte_at_offset (uint32_t address_offset, uint8_t data)
{
	uint32_t data32=0xFFFFFFFF;
	
	uint32_t address=0;
	
	uint32_t return_value;
	
	if(address_offset >= USER_FLASH_LENGTH)
		return -1;
	else
	{
		address = USER_FLASH_STORE_BASE_ADDRESS+(address_offset&(~0x03)); //Se accede a la flash solo a traves de words de 32 bits
																																			//Los primeros 2 bits tienen que ser 0
		switch (address_offset&0x03)
		{
			case 0:	data32=0xFFFFFF00;
							data32+= data;
							break;
			case 1: data32=0xFFFF00FF;
							data32+=(data<<8);
							break;
			case 2:	data32=0xFF00FFFF;
							data32+=(data<<16);
							break;
			case 3:	data32=0x00FFFFFF;
							data32+=(data<<24);
							break;
		}	
		return_value = FLASH_ProgramWordData(address, data32);
	}	
	return return_value;
}
#endif


#if USER_FLAG_REGISTER_FLASH_SIZE == USER_REG_FLASH_SIZE_16BITS
int32_t flash_user_write_2byte_at_offset (uint32_t address_offset, uint16_t data)
{
	uint32_t data32=0xFFFFFFFF;
	
	uint32_t address=0;
	
	uint32_t return_value;
	
	if(address_offset >= USER_FLASH_LENGTH)
		return -1;
	else
	{
		address = USER_FLASH_STORE_BASE_ADDRESS+(address_offset&(~0x03)); //Se accede a la flash solo a traves de words de 32 bits
																																			//Los primeros 2 bits tienen que ser 0
		switch (address_offset&0x02)
		{
			case 0:	data32=0xFFFF0000;
							data32+= data;
							break;
			case 2: data32=0x0000FFFF;
							data32+=(data<<16);
							break;
		}	
		return_value = FLASH_ProgramWordData(address, data32);
	}	
	return return_value;
}
#endif

#if USER_FLAG_REGISTER_FLASH_SIZE == USER_REG_FLASH_SIZE_32BITS
int32_t flash_user_write_4byte_at_offset (uint32_t address_offset, uint32_t data)
{
	uint32_t address=0;
	
	uint32_t return_value;
	
	if(address_offset >= USER_FLASH_LENGTH)
		return -1;
	else
	{
		address = USER_FLASH_STORE_BASE_ADDRESS+(address_offset&(~0x03)); //Se accede a la flash solo a traves de words de 32 bits
																																			//Los primeros 2 bits tienen que ser 0
		return_value = FLASH_ProgramWordData(address, data);
	}	
	return return_value;
}
#endif

