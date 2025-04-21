#ifndef __PACK_H__
#define __PACK_H__

#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"


uint32_t struct_unpack(char* buff,uint32_t buff_size, char*fmt, ...);
uint32_t struct_pack(char* buff,uint32_t buff_size, char*fmt, ...);

#endif

