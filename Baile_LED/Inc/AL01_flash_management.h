#ifndef AL01_FLASH_MANAGEMENT_H_

#include "AL01_board_iq_aed_rev20191011.h"

#define AL01_FLASH_MANAGEMENT_H_

//En el 52241 la ultima pagina empieza en 0xFC00
//En el 52231 la ultima pagina empieza en 0x7C00

#ifdef USE_MEM_HT32F52241

#define USER_FLASH_STORE_BASE_ADDRESS 0x7800 /*Es la anteultima pagina de Main Flash si fuera de 32KB.*/
#define USER_FLASH_LENGTH							 0x400 /*Se usa una sola pagina de 1KB */
																								
#endif


#ifdef USE_MEM_HT32F52231

#define USER_FLASH_STORE_BASE_ADDRESS 0x7C00 /*Es la anteultima pagina de Main Flash si fuera de 32KB.*/
#define USER_FLASH_LENGTH							 0x400 /*Se usa una sola pagina de 1KB */

#endif


#define USER_FLASH_OFFSET_THRS_TO_ERASE	(USER_FLASH_LENGTH-0x10)

#define USER_REG_FLASH_SIZE_8BITS		0
#define USER_REG_FLASH_SIZE_16BITS 	1
#define USER_REG_FLASH_SIZE_32BITS	2

/*Esta asignacion de abajo define cual va a ser el largo del registro que se almacena en FLASH*/
#define USER_FLAG_REGISTER_FLASH_SIZE USER_REG_FLASH_SIZE_8BITS 

int32_t flash_user_store_erase (void);
int32_t flash_user_get_first_byte_available_offset (void);
int32_t flash_user_is_the_flash_empty (void);

#if USER_FLAG_REGISTER_FLASH_SIZE == USER_REG_FLASH_SIZE_8BITS
int32_t flash_user_write_1byte_at_offset (uint32_t address_offset, uint8_t data);
#endif

#if USER_FLAG_REGISTER_FLASH_SIZE == USER_REG_FLASH_SIZE_16BITS
int32_t flash_user_write_2byte_at_offset (uint32_t address_offset, uint16_t data);
#endif

#if USER_FLAG_REGISTER_FLASH_SIZE == USER_REG_FLASH_SIZE_16BITS
int32_t flash_user_write_4byte_at_offset (uint32_t address_offset, uint32_t data);
#endif

#endif


