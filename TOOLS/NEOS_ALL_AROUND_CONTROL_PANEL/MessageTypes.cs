using System;
using HardwareInterfaceManagement;
using System.Collections;

namespace PlayworldMessages
 {


    public static class SystemModes
    {

        public const byte GAME_ACTIVE = 0x00;
        public const byte FLASH_UPDATE = 0x01;

    }

    public static class MessageTypes
    {
        //Define Message Types here

       

       public const byte UNKNOWN_COMMAND           =0x01;
       public const byte GET_FIRMWARE_VERSION      =0x02;
       public const byte READ_FLASH_STATUS         =0x05;
       public const byte WRITE_FLASH_STATUS        =0x06;
       public const byte READ_FLASH_PAGE   	       =0x07;
       public const byte WRITE_FLASH_PAGE	       =0x08;
       public const byte SECTOR_ERASE		       =0x09;
       public const byte BULK_ERASE			       =0x0A;
       public const byte FLASH_WRITING_COMPLETE	       =0x10;
       public const byte FLASH_WRITING_ERROR             =0x12;
       public const byte SOFTWARE_RESET            =0x13;
       public const byte PLAY_SOUND	               =0x20;
       public const byte SWITCH_SYSTEM_MODE = 0x90;
      
        
     

        public static ushort MemPage=0;
        public static UInt16 SoundToPlay;
        public static UInt16 SoundLength;
        public static byte SoundStream;

        public static byte NewSystemMode=0;
        public static byte NewRiderProgram=0;

        public static byte[] AssembleMessage(byte Type)
            {
            byte[] Packet;
            ushort MyCRC;

            switch (Type)
                {

                default:
                    Packet = null;
                    break;

                case PLAY_SOUND:
                    Packet = new Byte[12];

                    Packet[0] = 0xAA;
                    Packet[1] = 0xAA;
                    Packet[2] = 6;
                    Packet[3] = 0;
                    Packet[4] = MessageTypes.PLAY_SOUND;
                    Packet[5] = (byte)SoundToPlay;
                    Packet[6] = (byte)(SoundToPlay>>8);
                    Packet[7] = (byte)SoundLength;
                    Packet[8] = (byte)(SoundLength>>8);
                    Packet[9] = (byte)SoundStream;
                                       
                    MyCRC = CRC.Calculate(Packet, 10);

                    Packet[10] = (Byte)(MyCRC & 0xff);
                    Packet[11] = (Byte)(MyCRC >> 8);

                    break;     
                
        
                case SWITCH_SYSTEM_MODE:

                    Packet = new Byte[8];

                    Packet[0] = 0xAA;
                    Packet[1] = 0xAA;
                    Packet[2] = 2;
                    Packet[3] = 0;
                    Packet[4] = MessageTypes.SWITCH_SYSTEM_MODE;
                    Packet[5] = NewSystemMode;

                    MyCRC = CRC.Calculate(Packet, 6);

                    Packet[6] = (Byte)(MyCRC & 0xff);
                    Packet[7] = (Byte)(MyCRC >> 8);

                    break;



          




                case GET_FIRMWARE_VERSION:

                    
                    Packet = new Byte[7];
                    

                    Packet[0] = 0xAA;
                    Packet[1] = 0xAA;
                    Packet[2] = 1;
                    Packet[3] = 0;
                    Packet[4] = MessageTypes.GET_FIRMWARE_VERSION;

                    MyCRC = CRC.Calculate(Packet, 5);

                    Packet[5] = (Byte)(MyCRC & 0xff);
                    Packet[6] = (Byte)(MyCRC >> 8);

                    break;

                case SOFTWARE_RESET:


                    Packet = new Byte[7];
                   

                    Packet[0] = 0xAA;
                    Packet[1] = 0xAA;
                    Packet[2] = 1;
                    Packet[3] = 0;
                    Packet[4] = MessageTypes.SOFTWARE_RESET;

                    MyCRC = CRC.Calculate(Packet, 5);

                    Packet[5] = (Byte)(MyCRC & 0xff);
                    Packet[6] = (Byte)(MyCRC >> 8);

                break;

               
                }






            return Packet;


            }
      
        
  }




}
