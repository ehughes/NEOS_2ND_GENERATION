using System;
using System.Collections.Generic;
using System.Text;
using CANInterfaceManagement;

namespace CAN_SNIFFER
{
    public class NEOSMessages
    {
        public const Int32 PLAY_INTERNAL_SOUND = 0x8B;
        public const Int32 LED_COMMAND = 0x010;
        public const Int32 BUTTON_PRESSED = 0xF0;
        public const Int32 RESET_SLAVES = 0xE0;
        public const Int32 SCORE_BUTTONS_CHANGED = 0xF3;
        public const Int32 SCORE_LED_COMMAND = 0x72;
        public const Int32 SEND_SCORE_SEGMENT_DATA = 0x70;
        public const Int32 GENERAL_PURPOSE_BUFFER_OPERATION = 0x2F0;
        public const Int32 NODE_OPERATIONS = 0x2F2;
        public const Int32 WRITE_GENERAL_PURPOSE_BUFFER = 0x200;
        public const Int32 SET_MASTER_SYSTEM_MODE = 0x2F3;
        public const Int32 NODE_FLASH_MEMORY_OPERATION = 0x2F1;




        public static class NODE_FLASH_MEMORY_OPERATION_PARAMS
        {

            public const byte WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER	=0x00;
		    public const byte READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER		=0x01;
		    public const byte ERASE_SECTOR						    	=0x02;

            public static byte CommandByte;
            public static UInt32 FlashAddress;
            public static byte Node;

        }

        public static class SET_MASTER_SYSTEM_MODE_PARAMS
        {

            public static byte GAME_ACTIVE = 0;
            public static byte USB_FLASH_UPDATE = 1;
            public static byte SYSTEM_IDLE = 2;
            public static byte SYSTEM_SOFTWARE_RESET = 3;
            public static byte SYSTEM_DIAGNOSTICS = 4;


            public static byte Mode;

        }
        public static class NODE_OPERATIONS_PARAMS
        {
            public const byte NODE_PING                         =	0x01;
		    public const byte NODE_PONG	                    	=   0x02;
		    public const byte NODE_STATISCALLY_ACCEPT_UID	    =	0x03;
            public const byte NODE_UNCONDITIONALLY_ACCEPT_UID   =   0x04;
            public const byte NODE_READ_SUPPLY_VOLTAGE          =   0x05;
            public const byte NODE_SUPPLY_VOLTAGE_RESPONSE      =   0x06;

           
            public static byte Node;
            public static byte Command;
            public static UInt32 UID;
           
        }

        public static class WRITE_GENERAL_PURPOSE_BUFFER_PARAMS
        {
            public static byte [] WriteData = new byte[8];
            public static byte Node;
            public static byte BufferSegment;
        }

        public static class GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS
        {
            public const byte CLEAR_BUFFER = 0x01;
            public const byte SET_BUFFER_ACCEPTANCE_FLAG = 0x02;
            public const byte CLEAR_BUFFER_ACCEPTANCE_FLAG = 0x03;
            public const byte READ_BUFFER_SEGMENT = 0x04;
            public const byte READ_BUFFER_ENTIRE_BUFFER = 0x05;
            public const byte WRITE_BUFFER_ACK = 0x06;
            public const int GP_BUFFER_READ_LOWER_CAN_ID_BOUND = 0x240;
            public const int GP_BUFFER_READ_UPPER_CAN_ID_BOUND = 0x27F;

            public static byte Node;
            public static byte CommandByte;
            public static byte BufferSegment;
        }

        public static class SCORE_BUTTONS_CHANGED_PARAMS
        {
            public static byte Node;
            public static ushort Buttons1to16;
            public static ushort Buttons17to20;
        }

        public static class BUTTON_PRESS_PARAMS
        {
            public static byte Node;
        }

        public static class PLAY_INTERNAL_SOUND_PARAM
        {
            public static byte Node;
            public static byte InternalSoundNumber;
            public static byte PlaybackVolume;
            public static byte Repeats;
            public static byte StreamWhenComplete;
            public static byte AudioStatusWhenComplete;
            public static byte VolumeWhenComplete;

            public const byte NO_CHANGE = 0xFF;
            public const byte AUDIO_ON = 0x1;
            public const byte AUDIO_OFF = 0x0;
        }

        public static CANMessage AssembleNEOSCANMessage(Int32 MessageType)
        {

            CANMessage NewCANMessage = new CANMessage();

            switch (MessageType)
            {

                case NODE_FLASH_MEMORY_OPERATION:
                    NewCANMessage.CANId = NODE_FLASH_MEMORY_OPERATION;
                    NewCANMessage.CANData[0] = NODE_FLASH_MEMORY_OPERATION_PARAMS.Node;
                    NewCANMessage.CANData[1] = NODE_FLASH_MEMORY_OPERATION_PARAMS.CommandByte;
                    NewCANMessage.CANData[2] = (byte)(NODE_FLASH_MEMORY_OPERATION_PARAMS.FlashAddress);
                    NewCANMessage.CANData[3] = (byte)(NODE_FLASH_MEMORY_OPERATION_PARAMS.FlashAddress>>8);
                    NewCANMessage.CANData[4] = (byte)(NODE_FLASH_MEMORY_OPERATION_PARAMS.FlashAddress>>16);
                    NewCANMessage.CANData[5] = (byte)(NODE_FLASH_MEMORY_OPERATION_PARAMS.FlashAddress>>24);
                    NewCANMessage.CANData[6] = 0;
                    NewCANMessage.CANData[7] = 0;

                    break;

                case SET_MASTER_SYSTEM_MODE:

                    NewCANMessage.CANId = SET_MASTER_SYSTEM_MODE;
                    NewCANMessage.CANData[0] = SET_MASTER_SYSTEM_MODE_PARAMS.Mode;
                    NewCANMessage.CANData[1] = 0;
                    NewCANMessage.CANData[2] = 0;
                    NewCANMessage.CANData[3] = 0;
                    NewCANMessage.CANData[4] = 0;
                    NewCANMessage.CANData[5] = 0;
                    NewCANMessage.CANData[6] = 0;
                    NewCANMessage.CANData[7] = 0;

                    break;

                case WRITE_GENERAL_PURPOSE_BUFFER:
                    NewCANMessage.CANId = (uint)(WRITE_GENERAL_PURPOSE_BUFFER + WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.BufferSegment);
                    NewCANMessage.CANData[0] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[0];
                    NewCANMessage.CANData[1] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[1];
                    NewCANMessage.CANData[2] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[2];
                    NewCANMessage.CANData[3] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[3];
                    NewCANMessage.CANData[4] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[4];
                    NewCANMessage.CANData[5] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[5];
                    NewCANMessage.CANData[6] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[6];
                    NewCANMessage.CANData[7] = WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[7];
                break; 
                
                case NODE_OPERATIONS:
                    NewCANMessage.CANId = NODE_OPERATIONS;

                    switch (NODE_OPERATIONS_PARAMS.Command)
                    {
                        default:
                        case NODE_OPERATIONS_PARAMS.NODE_PING:
                            NewCANMessage.CANData[0] = NODE_OPERATIONS_PARAMS.NODE_PING;
                            NewCANMessage.CANData[1] = NODE_OPERATIONS_PARAMS.Node;
                            NewCANMessage.CANData[2] = 0;
                            NewCANMessage.CANData[3] = 0;
                            NewCANMessage.CANData[4] = 0;
                            NewCANMessage.CANData[5] = 0;
                            NewCANMessage.CANData[6] = 0;
                            NewCANMessage.CANData[7] = 0;
                            break;

                        case NODE_OPERATIONS_PARAMS.NODE_STATISCALLY_ACCEPT_UID:
                        case NODE_OPERATIONS_PARAMS.NODE_UNCONDITIONALLY_ACCEPT_UID:
                            NewCANMessage.CANData[0] = NODE_OPERATIONS_PARAMS.Command;
                            NewCANMessage.CANData[1] = NODE_OPERATIONS_PARAMS.Node;
                            NewCANMessage.CANData[2] = (byte)(NODE_OPERATIONS_PARAMS.UID);
                            NewCANMessage.CANData[3] = (byte)(NODE_OPERATIONS_PARAMS.UID>>8);
                            NewCANMessage.CANData[4] = (byte)(NODE_OPERATIONS_PARAMS.UID>>16);
                            NewCANMessage.CANData[5] = (byte)(NODE_OPERATIONS_PARAMS.UID>>24);
                            NewCANMessage.CANData[6] = 0;
                            NewCANMessage.CANData[7] = 0;
                            break;

                        case NODE_OPERATIONS_PARAMS.NODE_READ_SUPPLY_VOLTAGE:
                            NewCANMessage.CANData[0] = NODE_OPERATIONS_PARAMS.NODE_READ_SUPPLY_VOLTAGE;
                            NewCANMessage.CANData[1] = NODE_OPERATIONS_PARAMS.Node;
                            NewCANMessage.CANData[2] = 0;
                            NewCANMessage.CANData[3] = 0;
                            NewCANMessage.CANData[4] = 0;
                            NewCANMessage.CANData[5] = 0;
                            NewCANMessage.CANData[6] = 0;
                            NewCANMessage.CANData[7] = 0;
                            break;

                    }


                    break;


                case GENERAL_PURPOSE_BUFFER_OPERATION:

                    NewCANMessage.CANId = GENERAL_PURPOSE_BUFFER_OPERATION;

                    NewCANMessage.CANData[0] = GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.Node;
                    NewCANMessage.CANData[1] = GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CommandByte;
                    NewCANMessage.CANData[2] = GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.BufferSegment;
                    NewCANMessage.CANData[3] = 0;
                    NewCANMessage.CANData[4] = 0;
                    NewCANMessage.CANData[5] = 0;
                    NewCANMessage.CANData[6] = 0;
                    NewCANMessage.CANData[7] = 0;
                    break;

                case SCORE_BUTTONS_CHANGED:


                    NewCANMessage.CANId = SCORE_BUTTONS_CHANGED;

                    NewCANMessage.CANData[0] = SCORE_BUTTONS_CHANGED_PARAMS.Node;
                    NewCANMessage.CANData[1] = 0;
                    NewCANMessage.CANData[2] = (byte)SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16;
                    NewCANMessage.CANData[3] = (byte)(SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 >> 8);

                    NewCANMessage.CANData[4] = (byte)SCORE_BUTTONS_CHANGED_PARAMS.Buttons17to20;
                    NewCANMessage.CANData[5] = (byte)(SCORE_BUTTONS_CHANGED_PARAMS.Buttons17to20 >> 8);
                    NewCANMessage.CANData[6] = 0;
                    NewCANMessage.CANData[7] = 0;

                    break;



                case PLAY_INTERNAL_SOUND:

                    NewCANMessage.CANId = PLAY_INTERNAL_SOUND;

                    NewCANMessage.CANData[0] = PLAY_INTERNAL_SOUND_PARAM.Node;
                    NewCANMessage.CANData[1] = PLAY_INTERNAL_SOUND_PARAM.InternalSoundNumber;
                    NewCANMessage.CANData[2] = PLAY_INTERNAL_SOUND_PARAM.PlaybackVolume;
                    NewCANMessage.CANData[3] = PLAY_INTERNAL_SOUND_PARAM.Repeats;

                    NewCANMessage.CANData[4] = PLAY_INTERNAL_SOUND_PARAM.StreamWhenComplete;
                    NewCANMessage.CANData[5] = PLAY_INTERNAL_SOUND_PARAM.VolumeWhenComplete;
                    NewCANMessage.CANData[6] = PLAY_INTERNAL_SOUND_PARAM.AudioStatusWhenComplete;
                    NewCANMessage.CANData[7] = 0;
                    break;

                case BUTTON_PRESSED:
                    NewCANMessage.CANId = BUTTON_PRESSED;

                    NewCANMessage.CANData[0] = BUTTON_PRESS_PARAMS.Node;
                    NewCANMessage.CANData[1] = 0;
                    NewCANMessage.CANData[2] = 0;
                    NewCANMessage.CANData[3] = 0;

                    NewCANMessage.CANData[4] = 0;
                    NewCANMessage.CANData[5] = 0;
                    NewCANMessage.CANData[6] = 0;
                    NewCANMessage.CANData[7] = 0;
                    break;


            }

            return NewCANMessage;
        }

    }

}
