#ifndef AL02_HAIRDRYER_GLOBAL_REGISTERS_H_

#define AL02_HAIRDRYER_GLOBAL_REGISTERS_H_

#include "AL01_flash_management.h"


struct global_registers
{

};

union global_reg_bytes_split 
{
	struct global_registers gv;
	uint8_t	global_reg_bytes[sizeof(struct global_registers)];
};


#endif