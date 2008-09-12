#ifndef _CANCOMMANDS_H
#define _CANCOMMANDS_H

//SLAVE SPECIFIC COMMANDS

#define LED_COMMAND						0x10
#define PLAY_AUDIO_STREAM				0x88
#define SYNC_NODE_TO_STREAM 			0x89
#define SYNC_ALL_TO_STREAM			    0x8A
#define PLAY_INTERNAL_SOUND				0x8B
#define NOOP							0x8C
#define AUDIO_OFF_ALL_NODES 			0x8E
#define RESET_SLAVES 					0xE0
#define SET_SLAVE_TO_ADDRESS 			0xE3
#define CANCEL_SET_SLAVE_TO_ADDRESS 	0xE4
#define SLAVE_BUTTON_PRESSED 			0xF0
#define SLAVE_BUTTON_RELEASED 			0xF1

//Score Board Specific Commands

#define SEND_SCORE_SEGMENT_DATA				0x70
#define SCORE_DISPLAY_FLASH_DATA		 	0x71
#define SCORE_LED_COMMAND					0x72
#define SCORE_BUTTONS_CHANGED	 			0xF3


//everything else
#define GENERAL_PURPOSE_BUFFER_OPERATION 0x2F0

//buffer operation Macros
#define CLEAR_BUFFER					0x01
#define SET_BUFFER_ACCEPTANCE_FLAG		0x02
#define CLEAR_BUFFER_ACCEPTANCE_FLAG	0x03
#define READ_BUFFER_SEGMENT				0x04

#define READ_BUFFER_ENTIRE_BUFFER		0x05
#define WRITE_BUFFER_ACK				0x06
#define READ_BUFFER_ACK					0x07
#define ACK_CMD							0x80




#define GP_BUFFER_WRITE_LOWER_CAN_ID_BOUND	0x200 //inclusive test =>
#define GP_BUFFER_WRITE_UPPPER_CAN_ID_BOUND 0x23F //Inclusive Test =>

#define GP_BUFFER_READ_LOWER_CAN_ID_BOUND	0x240 //inclusive test =>
#define GP_BUFFER_READ_UPPPER_CAN_ID_BOUND 0x27F //Inclusive Test =>



#define NODE_FLASH_MEMORY_OPERATION 			0x2F1
#define WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER	0x00
#define READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER	0x01
#define ERASE_SECTOR							0x02


#define NODE_OPERATIONS	0x2F2
	
#define NODE_PING						0x01
#define NODE_PONG						0x02
#define NODE_STATISCALLY_ACCEPT_UID		0x03
#define NODE_UNCONDITIONALLY_ACCEPT_UID	0x04
#define	NODE_READ_SUPPLY_VOLTAGE		0x05
#define	NODE_SUPPLY_VOLTAGE_RESPONSE	0x06
#define	NODE_LOOP_BACK					0x07
#define	NODE_LOOP_BACK_RESPONSE		    0x08		

#define SET_MASTER_SYSTEM_MODE	0x2F3


#endif

