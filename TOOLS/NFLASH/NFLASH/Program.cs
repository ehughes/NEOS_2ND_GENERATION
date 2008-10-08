using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.IO;
using Microsoft.Win32;
using HardwareInterfaceManagement;
using NEOSUSBMessages;

namespace NFLASH
{
    class Program
    {

       static string COMPort;
       static  bool PortFound = false;


       #region SoundFileDownloadVariables

       public const int WAITING_FOR_SOUND_MODULE = 0;
       public const int FLASH_OPERATION_COMPLETE = 1;
       public const int FLASH_OPERATION_FAILED = 2;
       static volatile int FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;
        static public volatile bool FLASHOperationFailed = false;
       public const int FLASHEraseOperationTimeOut = 500;
       public const int FLASHWriteOperationTimeOut = 200;

       public const int MAX_RETRIES_AFTER_TIMEOUT = 5;

       static bool DownloadingSoundFile = false;

        static public int TimeOutCnt = 0;
        static public int FlashStatusCurrent;

       static byte[] PageToFlash = new byte[256];
        static byte[] WriteOutPacket = new byte[266];

        static bool Quit = false;
     
       #endregion


        static bool CheckQuit()
        {
            if (Quit == true)
            {
                Console.WriteLine("Exiting gracefully....   Process not completed.");
                return true;
            }

            return false;
        }
           
     

        static int Main(string[] args)
        {

            try
            {

                Console.CancelKeyPress += new ConsoleCancelEventHandler(Console_CancelKeyPress);
                CommunicationsManager MyCommunicationsManager = new CommunicationsManager();

                if (args.Length != 2)
                {
                    Console.WriteLine("Usage:  NFLASH Filename COMPort\r\n Set COMPort to AUTO to autodetect port.\r\n Ex: NFLASH FlashImage.bin AUTO\r\n");
                    MyCommunicationsManager.Terminate();
                    return -1;
                }


                if (PortDiscovery(args[1]) != 0)
                {
                    MyCommunicationsManager.Terminate();
                    return -1;
                }


                MyCommunicationsManager.PortToConnectTo = COMPort;

                Console.WriteLine("Connecting to " + COMPort + "....");

                MyCommunicationsManager.MyPacketDetector.MyPacketProccessor += new PacketProcessor(ProcessIncomingPacket);
                MyCommunicationsManager.Connect();

                int TimeOutCnt = 0;
                while ((MyCommunicationsManager.ConnectionActive == false) && (TimeOutCnt < 50))
                {
                    Thread.Sleep(100);
                    TimeOutCnt++;
                }

                if (MyCommunicationsManager.ConnectionActive == false)
                {
                    Console.WriteLine("Could not connect to " + COMPort + " after 5 seconds");
                    MyCommunicationsManager.Terminate();
                    return -1;
                }

                Console.WriteLine("Connected to " + COMPort + ".");


                string FileName = args[0];
                if (File.Exists(FileName) == true)
                {
                    if (DownloadSoundFile(FileName, MyCommunicationsManager) == false)
                    {
                        MyCommunicationsManager.Terminate();
                        return -1;
                    }
                }
                else
                {
                    Console.WriteLine("Could not find file: " + FileName);
                    MyCommunicationsManager.Terminate();
                    return -1;
                }


                

                Console.WriteLine("Process Complete!");
                MyCommunicationsManager.Terminate();
                return 0;
            }
            catch (Exception Ex)
            {
                Console.WriteLine(Ex.Message);
               
                return -1;
            }
         
        }

        static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            e.Cancel = true;
            Quit = true;
        }


        static int PortDiscovery(string PortNameCheck)
        {
            if (PortNameCheck.ToLower() == "auto")
            {

                #region auto

                string RegistryCOMPortRecord;

                if (Environment.OSVersion.Version.Major == 6)
                {
                    RegistryCOMPortRecord = (string)Registry.GetValue("HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Enum\\USB\\Vid_10c4&Pid_ea60\\0001\\Device Parameters", "PortName", "Nope!");
                }
                else
                {
                    RegistryCOMPortRecord = (string)Registry.GetValue("HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Enum\\USB\\Vid_10c4&Pid_ea60&Mi_00\\0001_00\\Device Parameters", "PortName", "Nope!");
                }
                //now see what com  ports are available in the system

                string[] AvailablePorts = SerialPort.GetPortNames();

                if (RegistryCOMPortRecord == null)
                {
                    Console.WriteLine("It appears that a NEOS module is not connected via USB. No Entries in Registry.  Try disconnecting and reconnecting all cables.");
                    return -1;
                }
                else
                {
                 
                    for (int i = 0; i < AvailablePorts.Length; i++)
                    {
                        if (RegistryCOMPortRecord == AvailablePorts[i])
                        {
                            COMPort = AvailablePorts[i];

                            Console.WriteLine("NEOS Module found on " + COMPort);
                            return 0;
                        }

                    } 
                    
                    Console.WriteLine("It appears that a NEOS module is not connected via USB.  List of available ports does not match the registry entry of " + RegistryCOMPortRecord +". Try disconnecting and reconnecting all cables.");
                    return -1;
                     
                }

                #endregion
            }
            else
            {
                #region manual

                COMPort = PortNameCheck;
                PortFound = false;

                string[] AvailablePorts = SerialPort.GetPortNames();

                for (int i = 0; i < AvailablePorts.Length; i++)
                {
                    if (AvailablePorts[i] == COMPort)
                    {
                        Console.WriteLine("Port " + COMPort + " found.");
                        PortFound = true;
                        break;
                    }
                }
                if (PortFound == false)
                {
                    Console.WriteLine("Port " + COMPort + " is not a valid Port Name.");


                    if (AvailablePorts.Length > 0)
                    {
                        Console.WriteLine("Available Ports:");


                        for (int i = 0; i < AvailablePorts.Length; i++)
                        {
                            Console.WriteLine("" + AvailablePorts[i]);
                        }
                    }
                    else
                    {
                        Console.WriteLine("No System COM ports found.\r\n");
                    }
                    return -1;
                }
                else
                {
                    return 0;
                }
                return 0;
                #endregion

            }

            return 0;
        }




        #region SoundDownload


        static public void ProcessIncomingPacket(byte[] IncomingPacketData)
        {

            switch (IncomingPacketData[0])
            {

                case MessageTypes.UNKNOWN_COMMAND:

                case MessageTypes.FLASH_WRITING_COMPLETE:
                    if (IncomingPacket.Length == 1)
                    {

                        FLASHOperationStatus = FLASH_OPERATION_COMPLETE;

                    }
                    else
                    {
                    }
                    break;

                case MessageTypes.FLASH_WRITING_ERROR:
                    if (IncomingPacket.Length == 1)
                    {

                        FLASHOperationStatus = FLASH_OPERATION_FAILED;
                    }
                    else
                    {
                    }
                    break;


                case MessageTypes.WRITE_FLASH_PAGE:
                    if (IncomingPacket.Length == 1)
                    {
                    }
                    else
                    {
                    }
                    break;

                default:

                    break;



            }


        }


        static public bool DownloadSoundFile(String Filename, CommunicationsManager MyCommunicationsManager)
        {

            FileStream fs = new FileStream(Filename, FileMode.Open, FileAccess.Read);
            BinaryReader br = new BinaryReader(fs);

            uint SoundFileLength = (uint)fs.Length;

            uint FLASHSectorsToErase;
            uint FLASHPagesToProgram;
            uint LastFlashPageSize;
            int i;
            uint FLASHSectorCounter = 0;
            uint FLASHPageCounter = 0;

            DownloadingSoundFile = true;

            for (i = 0; i < 256; i++)
            {
                PageToFlash[i] = (byte)0;
            }

            fs.Position = 0;

            PutNEOSInFLASHMode(MyCommunicationsManager);
            PutNEOSInFLASHMode(MyCommunicationsManager);
            PutNEOSInFLASHMode(MyCommunicationsManager);

            //Wait for unit to take the new mode.
            Thread.Sleep(500);

            fs.Position = 0;

            //First, Figure out the number of sectors (64k blocks)

            FLASHSectorsToErase = (SoundFileLength >> 16) + 1;
            FLASHPagesToProgram = SoundFileLength >> 8;

            if ((SoundFileLength & 0xFF) > 0)
            {

                FLASHPagesToProgram++; //Make sure to get the Scragglers

                LastFlashPageSize = SoundFileLength & 0xFF;
            }
            else
            {
                LastFlashPageSize = 0;

            }

            for (i = 0; i < FLASHSectorsToErase; i++)
            {

                if (CheckQuit() == true) return false;

                int TimeOutRetries = 0;
               
                GC.Collect();
            
            FlashEraseRetryEntry:

              Console.WriteLine("Erasing Sector " + (i + 1) + ":" + FLASHSectorsToErase);



                if (EraseFLASHSector(FLASHSectorCounter, MyCommunicationsManager) == false)
                {
                   Console.WriteLine("Error sending erase sector command at " + FLASHSectorCounter);

                    DownloadingSoundFile = false;
                    return false;
                }
                else
                {

                    TimeOutCnt = 0;
                    FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;

                    while (FLASHOperationStatus == WAITING_FOR_SOUND_MODULE)
                    {
                        if (CheckQuit() == true) return false;
                        
                        FlashStatusCurrent = i;
                        Thread.Sleep(1);
                        TimeOutCnt++;

                        if (TimeOutCnt > FLASHEraseOperationTimeOut)
                        {
                            TimeOutRetries++;

                            if (TimeOutRetries > MAX_RETRIES_AFTER_TIMEOUT)
                            {
                           Console.WriteLine("Could not erase Flash Sector [" + FLASHSectorCounter + "] after " + MAX_RETRIES_AFTER_TIMEOUT + " retries.  Timeout error.");
                                DownloadingSoundFile = false;
                                return false;

                            }
                            else
                            {
                                PutNEOSInFLASHMode(MyCommunicationsManager);
                                PutNEOSInFLASHMode(MyCommunicationsManager);
                                PutNEOSInFLASHMode(MyCommunicationsManager);
                                Thread.Sleep(250);
                                goto FlashEraseRetryEntry;
                            }
                        }

                    }

                    FLASHSectorCounter += 0x10000;
                }
            }




            FLASHPageCounter = 0;

            for (i = 0; i < FLASHPagesToProgram; i++)
            {
                if (CheckQuit() == true) return false;
                        

                int TimeOutRetries = 0;

              Console.WriteLine("Programming Page " + (i + 1) + ":" + FLASHPagesToProgram);

                if (i == (FLASHPagesToProgram - 1) && (LastFlashPageSize != 0))
                {

                    byte[] PartialData = br.ReadBytes((int)LastFlashPageSize);

                    int j;
                    for (j = 0; j < 256; j++)
                    {
                        if (j < LastFlashPageSize)
                        {
                            PageToFlash[j] = PartialData[j];
                        }
                        else
                        {
                            PageToFlash[j] = 0x00;
                        }
                    }

                }

                else
                {
                    PageToFlash = br.ReadBytes(256);
                }


                FlashStatusCurrent = i;

            TimeOutRetry:

                if (WriteFLASH(PageToFlash, FLASHPageCounter, MyCommunicationsManager) == false)
                {
                    Console.WriteLine("Error sending page program command at page [" + FLASHPageCounter + "]");
                    DownloadingSoundFile = false;
                    return false;
                }

                TimeOutCnt = 0;

                FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;

                while (FLASHOperationStatus == WAITING_FOR_SOUND_MODULE)
                {


                    Thread.Sleep(1);

                    TimeOutCnt++;

                    if (TimeOutCnt > FLASHWriteOperationTimeOut)
                    {
                        TimeOutRetries++;
                        if (TimeOutRetries > MAX_RETRIES_AFTER_TIMEOUT)
                        {
                            Console.WriteLine("Could not program page [" + FLASHPageCounter + "] after " + MAX_RETRIES_AFTER_TIMEOUT + " retries.  Timeout error.");

                            DownloadingSoundFile = false;
                            return false;

                        }
                        else
                        {
                            PutNEOSInFLASHMode(MyCommunicationsManager);
                            PutNEOSInFLASHMode(MyCommunicationsManager);
                            PutNEOSInFLASHMode(MyCommunicationsManager);
                            Thread.Sleep(250);
                            goto TimeOutRetry;
                        }
                    }

                }

                if (FLASHOperationStatus == FLASH_OPERATION_FAILED)
                {
                    Console.WriteLine("Flash program/verify error at address " + FLASHPageCounter);
                    DownloadingSoundFile = false;
                    return false;
                }

                FLASHPageCounter += 0x100;
            }

            FlashStatusCurrent = 0;
            Console.WriteLine("Reseting the NEOS Module.....");
            ResetNEOS(MyCommunicationsManager);
            ResetNEOS(MyCommunicationsManager);

          Console.WriteLine(FLASHPagesToProgram + " pages (" + FLASHPagesToProgram * 256 + "  bytes) successfully programmed");


            br.Close();
            fs.Close();

            DownloadingSoundFile = false;
            return true;
        }

        static public bool WriteFLASH(byte[] Page, uint PageAddressOut, CommunicationsManager MyCommunicationsManager)
        {

            uint PageAddress;

            PageAddress = PageAddressOut;

            if (MyCommunicationsManager.ConnectionActive)
            {

                if (Page.Length != 256)
                {
                    return false;
                }
                else
                {

                    ushort MyCRC;

                    PageAddress &= 0xFFFFFF00;

                    WriteOutPacket[0] = 0xAA;
                    WriteOutPacket[1] = 0xAA;
                    WriteOutPacket[2] = 0x04;
                    WriteOutPacket[3] = 0x01;
                    WriteOutPacket[4] = MessageTypes.WRITE_FLASH_PAGE;
                    WriteOutPacket[5] = (byte)(PageAddress >> 8);
                    WriteOutPacket[6] = (byte)(PageAddress >> 16);
                    WriteOutPacket[7] = (byte)(PageAddress >> 24);


                    int i;
                    for (i = 0; i < 256; i++)
                    {
                        WriteOutPacket[i + 8] = Page[i];
                    }

                    MyCRC = CRC.Calculate(WriteOutPacket, 256 + 8);

                    WriteOutPacket[256 + 8] = (Byte)(MyCRC & 0xff);
                    WriteOutPacket[256 + 9] = (Byte)(MyCRC >> 8);


                    MyCommunicationsManager.Write(WriteOutPacket);
                    FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;
                    return true;


                }
            }
            else
            {
                return false;
            }


        }

        static public bool EraseFLASHSector(uint SectorAddressOut, CommunicationsManager MyCommunicationsManager)
        {
            if (MyCommunicationsManager.ConnectionActive)
            {
                Byte[] PacketOut = new Byte[9];
                ushort MyCRC;
                uint SectorAddress;


                SectorAddress = SectorAddressOut;

                SectorAddress &= 0xFFFF0000;

                PacketOut[0] = 0xAA;
                PacketOut[1] = 0xAA;
                PacketOut[2] = 0x03;
                PacketOut[3] = 0x00;
                PacketOut[4] = MessageTypes.SECTOR_ERASE;
                PacketOut[5] = (byte)(SectorAddress >> 16);
                PacketOut[6] = (byte)(SectorAddress >> 24);
                MyCRC = CRC.Calculate(PacketOut, 7);

                PacketOut[7] = (Byte)(MyCRC & 0xff);
                PacketOut[8] = (Byte)(MyCRC >> 8);


                MyCommunicationsManager.Write(PacketOut);
                FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;
                return true;
            }
            else
            {
                return false;
            }
        }

        static void PutNEOSInFLASHMode(CommunicationsManager MyCommunicationsManager)
        {
            MessageTypes.NewSystemMode = SystemModes.USB_FLASH_UPDATE;
            byte[] OutPacket = MessageTypes.AssembleMessage(MessageTypes.SWITCH_SYSTEM_MODE);
            MyCommunicationsManager.Write(OutPacket);
        }

        static void PutNEOSInGAMEMode(CommunicationsManager MyCommunicationsManager)
        {
            MessageTypes.NewSystemMode = SystemModes.GAME_ACTIVE;
            byte[] OutPacket = MessageTypes.AssembleMessage(MessageTypes.SWITCH_SYSTEM_MODE);
            MyCommunicationsManager.Write(OutPacket);
        }

        static void ResetNEOS(CommunicationsManager MyCommunicationsManager)
        {
           
            byte[] OutPacket = MessageTypes.AssembleMessage(MessageTypes.SOFTWARE_RESET);
            MyCommunicationsManager.Write(OutPacket);
        }



        #endregion
    }
}
