#ifndef _USB_COMMANDS_H
#define _USB_COMMANDS_H

//MACROS for the commands.  All responses will use same number as the command
#define UNKNOWN_COMMAND      0x01
#define GET_FIRMWARE_VERSION 0x02
#define READ_FLASH_STATUS    0x05
#define WRITE_FLASH_STATUS   0x06
#define READ_PAGE 		  	 0x07
#define WRITE_PAGE  		 0x08
#define SECTOR_ERASE		 0x09
#define BULK_ERASE			 0x0A
#define WRITING_COMPLETE	 0x10
#define WRITING_ERROR        0x12
#define SOFTWARE_RESET       0x13
#define SWITCH_SYSTEM_MODE   0x90
#define PLAY_SOUND		     0x20





#endif
