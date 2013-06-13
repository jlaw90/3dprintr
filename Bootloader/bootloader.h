/*
 * bootloader.h
 *
 * Created: 13/06/2013 19:48:03
 *  Author: James
 */ 

#include <avr/io.h>

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#define ADDR_APP_RUN (uint8_t*) E2END
#define ADDR_FW_MAJOR ADDR_APP_RUN-1
#define ADDR_FW_MINOR ADDR_FW_MAJOR-1
#define ADDR_FW_CHECKSUM (uint32_t*) (ADDR_FW_MINOR-4)

#endif /* BOOTLOADER_H_ */