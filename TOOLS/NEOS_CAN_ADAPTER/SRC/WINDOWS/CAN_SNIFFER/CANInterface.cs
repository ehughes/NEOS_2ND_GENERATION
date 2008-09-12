using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.IO;
using Microsoft.Win32;
using FTD2XX_NET;

namespace CANInterfaceManagement
    {

        public class CANMessage
        {
            public UInt32 CANId=0;
            public Byte[] CANData;

            public CANMessage()
            {
                CANId = 0;
                CANData = new Byte[8];

                for (int i = 0; i < 8; i++)
                {
                    CANData[i] = 0;

                }
            }
            public CANMessage(UInt32 Id, byte Byte0, byte Byte1,byte Byte2,byte Byte3,byte Byte4,byte Byte5,byte Byte6, byte Byte7)
            {
                CANId = Id;
                CANData = new Byte [8];
                CANData[0] = Byte0;
                CANData[1] = Byte1;
                CANData[2] = Byte2;
                CANData[3] = Byte3;
                CANData[4] = Byte4;
                CANData[5] = Byte5;
                CANData[6] = Byte6;
                CANData[7] = Byte7;
            }
            public CANMessage(UInt32 Id, Byte [] Data)
            {
                if (Data == null)
                {
                    throw new Exception("CAN Data can not be null.");
                }

                if (Data.Length != 8)
                {
                    throw new Exception("CAN data length must be 8 bytes.");

                }

                CANId = Id;
                CANData = Data;
            }
        
            public static CANMessage Clone(CANMessage ExsistingMessage)
            {
                CANMessage ClonedCANMessage = new CANMessage();

                ClonedCANMessage.CANId = ExsistingMessage.CANId;

                for (int i = 0; i < 8; i++)
                {
                    ClonedCANMessage.CANData[i] = ExsistingMessage.CANData[i];
                }

                return ClonedCANMessage;
            }
        }


    public class CANMessageQueue
    {
        CANMessage[] MessageStore;
        UInt32 ReadPtr = 0;
        UInt32 WritePtr = 0;
       

        public CANMessageQueue()
        {
            MessageStore = new CANMessage[256];
            for (int i = 0; i < MessageStore.Length; i++)
            {
                MessageStore[i] = new CANMessage();
            }
            ReadPtr = 0;
            WritePtr = 0;
           
        }

        public CANMessageQueue(UInt32 CANQueueSize)
        {
            MessageStore = new CANMessage[CANQueueSize];
            for (int i = 0; i < MessageStore.Length; i++)
            {
                MessageStore[i] = new CANMessage();
            }
            ReadPtr = 0;
            WritePtr = 0;
           
        }


        public void Clear()
        {
            for (int i = 0; i < MessageStore.Length; i++)
            {
                MessageStore[i].CANId = 0;

                for (int j = 0; j < 8; j++)
                {
                
                    MessageStore[i].CANData[j] = 0;
                }
            }
            ReadPtr = 0;
            WritePtr = 0;
            

        }
        public void Enqueue(CANMessage Message)
        {
            this.MessageStore[WritePtr].CANId= Message.CANId;
            for (int i = 0; i < 8; i++)
            {
                this.MessageStore[WritePtr].CANData[i] = Message.CANData[i];

            }
            
            this.WritePtr++;
            if (WritePtr == MessageStore.Length)
            {
                WritePtr = 0;
            }
          
        }
        public void Dequeue(ref CANMessage Message)
        {
            Message.CANId = this.MessageStore[ReadPtr].CANId;

            for (int i = 0; i < 8; i++)
            {
                Message.CANData[i] = this.MessageStore[ReadPtr].CANData[i];
            }

           
            this.ReadPtr++;
            
            if (ReadPtr == this.MessageStore.Length)
            {
                ReadPtr = 0;
            }
           
        }

        public uint GetCount()
        {
            if (ReadPtr > WritePtr)
            {
                return (uint)(this.MessageStore.Length - ReadPtr + WritePtr);
            }
            else if (WritePtr > ReadPtr)
            {

                return (WritePtr - ReadPtr);
            }
            else
            {
                return 0;
            }

        }
    }

    static class CANInterfaceMessages
        {

            public const byte CAN_INTERFACE_PING = 0x00;
            public const byte CAN_INTERFACE_PONG = 0x11;
            public const byte SYSTEM_RESET = 0x01;
            public const byte TX_CAN_MESSAGE = 0x03;
            public const byte RX_CAN_MESSAGE = 0x04;
            public const byte SET_ID_EXCLUDE_RANGE = 0x05;
            public const byte SET_CAN_TERMINATION = 0x06;


            public static UInt32 IDExcludeLow = 0x100;
            public static UInt32 IDExcludeHigh = 0x1FF;

            public static byte CANTermination = 0;

            public static byte[] AssembleCANTxMessage(CANMessage OutgoingMessage)
            {
                        byte[] Packet;
                        ushort MyCRC;
                       
                        if(OutgoingMessage == null)
                        {
                            return null;
                        }

                        Packet = new Byte[19];

                        Packet[0] = 0xAA;
                        Packet[1] = 0xAA;
                        Packet[2] = 13;
                        Packet[3] = 0;
                        Packet[4] = TX_CAN_MESSAGE;
                        Packet[5] = (byte)(OutgoingMessage.CANId&0xff);
                        Packet[6] = (byte)(OutgoingMessage.CANId>>8);
                        Packet[7] = 0;
                        Packet[8] = 0;
                        Packet[9] = OutgoingMessage.CANData[0];
                        Packet[10] = OutgoingMessage.CANData[1];
                        Packet[11] = OutgoingMessage.CANData[2];
                        Packet[12] = OutgoingMessage.CANData[3];
                        Packet[13] = OutgoingMessage.CANData[4];
                        Packet[14] = OutgoingMessage.CANData[5];
                        Packet[15] = OutgoingMessage.CANData[6];
                        Packet[16] = OutgoingMessage.CANData[7];
                        MyCRC = CRC.Calculate(Packet, 17);
                        Packet[17] = (Byte)(MyCRC & 0xff);
                        Packet[18] = (Byte)(MyCRC >> 8);
                        
                        
                        return Packet;
            }   
            public static byte[] AssembleMessage(byte Type)
            {
                byte[] Packet;
                ushort MyCRC;


                switch (Type)
                {
                    default:
                        Packet = null;
                        break;

                    case CAN_INTERFACE_PING:

                        Packet = new Byte[7];
                        Packet[0] = 0xAA;
                        Packet[1] = 0xAA;
                        Packet[2] = 1;
                        Packet[3] = 0;
                        Packet[4] = CAN_INTERFACE_PING;
                        MyCRC = CRC.Calculate(Packet, 5);
                        Packet[5] = (Byte)(MyCRC & 0xff);
                        Packet[6] = (Byte)(MyCRC >> 8);
                       
                        break;

                    case SET_CAN_TERMINATION:

                        Packet = new Byte[8];
                        Packet[0] = 0xAA;
                        Packet[1] = 0xAA;
                        Packet[2] = 2;
                        Packet[3] = 0;
                        Packet[4] = SET_CAN_TERMINATION;
                        Packet[5] = (byte)CANTermination;
                        MyCRC = CRC.Calculate(Packet, 6);
                        Packet[6] = (Byte)(MyCRC & 0xff);
                        Packet[7] = (Byte)(MyCRC >> 8);
                         break;

                    case SET_ID_EXCLUDE_RANGE:

                        Packet = new Byte[15];
                        Packet[0] = 0xAA;
                        Packet[1] = 0xAA;
                        Packet[2] = 9;
                        Packet[3] = 0;
                        Packet[4] = SET_ID_EXCLUDE_RANGE;
                        Packet[5] = (byte)(IDExcludeLow);
                        Packet[6] = (byte)(IDExcludeLow >> 8);
                        Packet[7] = (byte)(IDExcludeLow >> 16);
                        Packet[8] = (byte)(IDExcludeLow >> 24);
                        Packet[9] = (byte)(IDExcludeHigh);
                        Packet[10] = (byte)(IDExcludeHigh >> 8);
                        Packet[11] = (byte)(IDExcludeHigh >> 16);
                        Packet[12] = (byte)(IDExcludeHigh >> 24);
                        MyCRC = CRC.Calculate(Packet, 13);
                        Packet[13] = (Byte)(MyCRC & 0xff);
                        Packet[14] = (Byte)(MyCRC >> 8);

                         break;

                    case SYSTEM_RESET:

                        Packet = new Byte[7];
                        Packet[0] = 0xAA;
                        Packet[1] = 0xAA;
                        Packet[2] = 1;
                        Packet[3] = 0;
                        Packet[4] = SYSTEM_RESET;
                        MyCRC = CRC.Calculate(Packet, 5);
                        Packet[5] = (Byte)(MyCRC & 0xff);
                        Packet[6] = (Byte)(MyCRC >> 8);
                        
                        break;
                }

                return Packet;


            }
        }



   
     public class CANCommunicationsManager
        {

         public CANMessageQueue OutgoingCANMessageQueue = new CANMessageQueue(65535);
         public CANMessageQueue IncomingCANMessageQueue = new CANMessageQueue(65535);

         private CANMessage IncomingCANMessage = new CANMessage();
         private CANMessage OutgoingCANMessage = new CANMessage();

         private FTDI MyFTDI = new FTDI();
         private FTD2XX_NET.FTDI.FT_STATUS MyFTDIStatus;
         
         public bool CANInterfacePongRecieved = false;
         public int PacketsIn = 0;
         public string Port;
         public int BytesRead = 0;
         public bool ConnectionActive = false;
         public string ConnectionStatus; 

         //State Machine Macros
         const byte H1 = 0xAA;
         const byte H2 = 0xAA;
         const byte SCAN_FOR_HEADER1 = 0x00;
         const byte SCAN_FOR_HEADER2 = 0x01;
         const byte GRAB_LENGTH_LOW = 0x02;
         const byte GRAB_LENGTH_HIGH = 0x03;
         const byte GRAB_PAYLOAD = 0x04;
         const byte GRAB_CRC_LOW = 0x05;
         const byte GRAB_CRC_HIGH = 0x06;

         byte[] Temp = new byte[1024];

         protected byte DetectState = SCAN_FOR_HEADER1;
         protected ushort DataCnt = 0;
         protected ushort CheckCRC = 0;
  
         private int ManagementState = 0 ;
         private SerialPort MyConnection;
	   
         const int WAITING_FOR_CONNECT_COMMAND = 0x00;
         const int SCANNING_FOR_MODULE = 0x01;
         const int CONNECTING = 0x02;
         const int CONNECTION_ACTIVE = 0x05;
         const int WAIT_FOR_RECONNECT = 0x06;
	     const int RECONNECT_PERIOD = 1000;
    
         private string RegistryCOMPortRecord;
         private bool AttemptToConnect = false;
         private byte[] StreamBuffer = new byte[512];
         private string[] ManagerMessages = { "Disconnected [Press Blue USB Icon to Connect]", "Connecting", "Connected", "Waiting before retry", "Connection Exception" };
         private Thread CommunicationsManagerThread;
       
        public CANCommunicationsManager()
            {
                ManagementState = WAITING_FOR_CONNECT_COMMAND;
                ConnectionStatus = ManagerMessages[0];
                ConnectionActive = false;
                CommunicationsManagerThread = new Thread(new ThreadStart(Tick));
                CommunicationsManagerThread.Priority = ThreadPriority.Highest;
                CommunicationsManagerThread.Start();
             }

         public void Tick()
            {
                while (true)
                {
                    Thread.Sleep(1);
                    //This Entire Routine Needs to be in a try catch for Virtual Com ports.  
                    try
                    {
                        //here is the main state machine 
                        switch (ManagementState)
                        {
                            case WAITING_FOR_CONNECT_COMMAND:
                                ConnectionStatus = ManagerMessages[0];
                                if (AttemptToConnect == true)
                                    {
                                        ManagementState = SCANNING_FOR_MODULE;
                                        AttemptToConnect = false;
                                    }
                                else
                                    {
                                        ManagementState = WAITING_FOR_CONNECT_COMMAND;
                                    }

                                break;
                            
                            case SCANNING_FOR_MODULE:

                                //First, check to see if there is a Virutalcom port available in the registry
                                ConnectionActive = false;
                                ConnectionStatus = ManagerMessages[1];

                                uint NumFTDIDevices=0;

                                MyFTDI.GetNumberOfDevices(ref NumFTDIDevices);

                                if (NumFTDIDevices == 0)
                                {
                                    MessageBox.Show( "No CAN Adapters Found","Error");
                                    ManagementState = WAIT_FOR_RECONNECT;
                                }
                                else
                                {
                                    ManagementState = CONNECTING;
                                }

                                /*
                                if (Environment.OSVersion.Version.Major == 6)
                                {
                                  RegistryCOMPortRecord = (string)Registry.GetValue("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\FTDIBUS\\VID_0403+PID_6001+00000000A\\0000\\Device Parameters", "PortName", "Nope!");
                                }
                                else
                                {
                                    RegistryCOMPortRecord = (string)Registry.GetValue("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\FTDIBUS\\VID_0403+PID_6001+00000000A\\0000\\Device Parameters", "PortName", "Nope!");
                                  }
                                //now see what com  ports are available in the system

                                string[] AvailablePorts = SerialPort.GetPortNames();


                                if (RegistryCOMPortRecord == null)
                                {
                                     ManagementState = WAITING_FOR_CONNECT_COMMAND;
                                }
                                else
                                {
                                    int i;
                                    for (i = 0; i < AvailablePorts.Length; i++)
                                    {
                                        if (RegistryCOMPortRecord == AvailablePorts[i])
                                        {
                                            ManagementState = CONNECTING;
                                            break;
                                        }
                                        else
                                        {
                                             ManagementState = WAITING_FOR_CONNECT_COMMAND;
                                                  
                                        }

                                    }
                                }
                                */
                                break;

                            case CONNECTING:
                                ConnectionStatus = ManagerMessages[1];
                                ConnectionActive = false;

                                

                                MyFTDIStatus = MyFTDI.OpenByIndex(0);

                                if (MyFTDIStatus == FTDI.FT_STATUS.FT_OK)
                                {
                                    ManagementState = CONNECTION_ACTIVE;
                                }
                                else
                                {
                                    MessageBox.Show(MyFTDIStatus.ToString(), "Error");
                                    ManagementState = WAIT_FOR_RECONNECT;
                                }

                                /*
                                try
                                    {
                                    MyConnection = new SerialPort();
                                    MyConnection.PortName = RegistryCOMPortRecord;
                                    Port = MyConnection.PortName;
                                    MyConnection.Open();
                                    ManagementState = CONNECTION_ACTIVE;
                                    BytesRead = 0;
                                    MyConnection.DiscardInBuffer();                              
                                    }

                                catch
                                    {
                                    Thread.Sleep(RECONNECT_PERIOD);
                                    ManagementState = WAITING_FOR_CONNECT_COMMAND;

                                    }
                                break;
                                 * */

                                break;
                            case CONNECTION_ACTIVE:
 
                                ConnectionActive = true;
                                ConnectionStatus = ManagerMessages[2];
                                ManagementState = CONNECTION_ACTIVE;

                                //See if there are any CAN messages To be sent
                                //lock (OutgoingCANMessageQueue)
                                {

                                    for (int q = 0; q < this.OutgoingCANMessageQueue.GetCount(); q++)
                                   // while (OutgoingCANMessageQueue.GetCount() > 0)
                                    {
                                        OutgoingCANMessageQueue.Dequeue(ref OutgoingCANMessage);
                                        IncomingCANMessageQueue.Enqueue(OutgoingCANMessage);
                                        byte [] OutPacket = CANInterfaceMessages.AssembleCANTxMessage(OutgoingCANMessage);
                                        if (OutPacket != null)
                                        {
                                            // MyConnection.Write(OutPacket,0,OutPacket.Length);
                                            uint DataBytesWritten=0;
                                            MyFTDI.Write(OutPacket, OutPacket.Length, ref DataBytesWritten);
                                        }
                                    }
                                }

                                
                               // int  BytesToGrab = MyConnection.BytesToRead;
                                uint  BytesToGrab = 0;
                                MyFTDI.GetRxBytesAvailable(ref BytesToGrab);

                                if (BytesToGrab > 0)
                                {
                                    if (BytesToGrab > 512)
                                    {
                                        BytesToGrab = 512;
                                    }
                                   // MyConnection.Read(StreamBuffer,0,BytesToGrab);
                                    uint FTDIBytesRead = 0;
                                    MyFTDI.Read(StreamBuffer, BytesToGrab, ref FTDIBytesRead);

                                    BytesRead += (int)FTDIBytesRead;
                                        try
                                        {
                                            for(int e=0;e<BytesToGrab;e++)
                                            {
                                                 ParseStream(StreamBuffer[e]);
                                                
                                            }
                                        }
                                        catch 
                                        {
                                        }
                                   }
                               break;

                            case WAIT_FOR_RECONNECT:
                                                              
                                Thread.Sleep(RECONNECT_PERIOD);
                                ManagementState = WAITING_FOR_CONNECT_COMMAND;
                                MyFTDI.Rescan();
                                break;

                            default:
                                ConnectionActive = false;
                                AttemptToConnect = false;
                                ConnectionStatus = ManagerMessages[0];
                                ManagementState = WAITING_FOR_CONNECT_COMMAND;
                                break;

                        }

                    }

                    catch 
                    {

                        ConnectionActive = false;
                        ConnectionStatus = ManagerMessages[4];
                        Thread.Sleep(RECONNECT_PERIOD);
                        //this Exception is important to catch.  THis will allow a graceful recovery if someone
                        //pulls the plug  
                        ManagementState = WAITING_FOR_CONNECT_COMMAND;
                    }
                }

            }

         public void Close()
        {
            

            try
                {
                    MyFTDI.Close();
               // MyConnection.Close();
                ConnectionStatus = ManagerMessages[0];
                ConnectionActive = false;
                ManagementState = WAITING_FOR_CONNECT_COMMAND;
                }
            catch
                {
                }
          
        }

         public void Terminate()
         {

           
            CommunicationsManagerThread.Abort();
            this.Close();

         }

         public void Connect()
            {
                 AttemptToConnect = true;
            }

         public void ParseIncomingPacket()
            {
                switch (IncomingPacket.Payload[0])
                {
                    case CANInterfaceMessages.RX_CAN_MESSAGE:

                        if (IncomingPacket.Length == 13)
                        {
                            
                            IncomingCANMessage.CANId = (UInt32)(IncomingPacket.Payload[1] | (IncomingPacket.Payload[2] << 8) | (IncomingPacket.Payload[3] << 16) | (IncomingPacket.Payload[4] << 24));
                            for (int i = 0; i < 8; i++)
                            {
                               IncomingCANMessage.CANData[i] = IncomingPacket.Payload[5 + i];
                            }

                            lock (IncomingCANMessage)
                            {
                                IncomingCANMessageQueue.Enqueue(IncomingCANMessage);
                            }
                          
                        }

                        break;

                    case CANInterfaceMessages.CAN_INTERFACE_PONG:
                        if(IncomingPacket.Length == 1)
                        {
                            CANInterfacePongRecieved = true;
                        }
                         break;

                    default:
                        break;

                }

            }

         public void ParseStream(byte DataIn)
            {


                switch (DetectState)
                {

                    case SCAN_FOR_HEADER1:
                        if (DataIn == H1)
                        {
                            DetectState = SCAN_FOR_HEADER2;
                            IncomingPacket.Header1 = DataIn;
                            Temp[0] = DataIn;


                        }
                        else
                        {
                            DetectState = SCAN_FOR_HEADER1;

                        }


                        break;

                    case SCAN_FOR_HEADER2:
                        if (DataIn == H2)
                        {
                            DetectState = GRAB_LENGTH_LOW;
                            IncomingPacket.Header2 = DataIn;
                            Temp[1] = DataIn;

                        }
                        else
                        {
                            DetectState = SCAN_FOR_HEADER1;
                        }
                        break;


                    case GRAB_LENGTH_LOW:

                        IncomingPacket.Length = (ushort)DataIn;
                        Temp[2] = DataIn;


                        DetectState = GRAB_LENGTH_HIGH;



                        break;

                    case GRAB_LENGTH_HIGH:
                        IncomingPacket.Length += (ushort)(((ushort)DataIn) << 8);
                        DataCnt = 0;
                        Temp[3] = DataIn;
                        DetectState = GRAB_PAYLOAD;


                        break;


                    case GRAB_PAYLOAD:
                        IncomingPacket.Payload[DataCnt] = DataIn;
                        Temp[4 + DataCnt] = DataIn;

                        DataCnt++;


                        if (DataCnt == (IncomingPacket.Length))
                        {
                            DetectState = GRAB_CRC_LOW;
                        }

                        else
                        {
                            DetectState = GRAB_PAYLOAD;
                        }

                        break;

                    case GRAB_CRC_LOW:

                        IncomingPacket.CRC = 0;
                        IncomingPacket.CRC = (ushort)DataIn;
                        DetectState = GRAB_CRC_HIGH;
                        break;

                    case GRAB_CRC_HIGH:

                        IncomingPacket.CRC += (ushort)(((ushort)(DataIn)) << 8);
                        DetectState = SCAN_FOR_HEADER1;

                        CheckCRC = CRC.Calculate(Temp, (ushort)(IncomingPacket.Length + 4));

                        if (CheckCRC == IncomingPacket.CRC)
                        {
                            ParseIncomingPacket();
                            PacketsIn++;
                        }
                        break;


                    default:
                        DetectState = SCAN_FOR_HEADER1;
                        break;


                }

            }


         public void PingCANInterface()
         {
             if(ConnectionActive == true)
             {
                 byte[] OutPacket = CANInterfaceMessages.AssembleMessage(CANInterfaceMessages.CAN_INTERFACE_PING);
                 MyConnection.Write(OutPacket,0,OutPacket.Length);
             }
         }

         public void SetExcludeRange(UInt32 RangeLow, UInt32 RangeHigh)
         {
            CANInterfaceMessages.IDExcludeLow = RangeLow;
            CANInterfaceMessages.IDExcludeHigh = RangeHigh;
            byte[] OutPacket = CANInterfaceMessages.AssembleMessage(CANInterfaceMessages.SET_ID_EXCLUDE_RANGE);
            this.MyConnection.Write(OutPacket, 0, OutPacket.Length);
   
         }

         public void SetCANTerminationState(bool TerminationState)
         {
             if (ConnectionActive == true)
             {
                 if (TerminationState == true)
                 {
                     CANInterfaceMessages.CANTermination = 1;
                     byte[] OutPacket = CANInterfaceMessages.AssembleMessage(CANInterfaceMessages.SET_CAN_TERMINATION);
                     this.MyConnection.Write(OutPacket, 0, OutPacket.Length);
                 }
                 else
                 {

                     CANInterfaceMessages.CANTermination = 0;
                     byte[] OutPacket = CANInterfaceMessages.AssembleMessage(CANInterfaceMessages.SET_CAN_TERMINATION);
                     this.MyConnection.Write(OutPacket, 0, OutPacket.Length);
                 }
             }

         }
         public void ResetCANInterface()
         {
             byte[] OutPacket = CANInterfaceMessages.AssembleMessage(CANInterfaceMessages.SYSTEM_RESET);
             this.MyConnection.Write(OutPacket, 0, OutPacket.Length);
         }
    }



    static class CRC
    {

        static public ushort Calculate(byte[] Data, ushort Length)
        {

            ushort i, ecrc;
            ecrc = 0xffff;
            for (i = 0; i < Length; i++)
            {
                ecrc = (ushort)((Byte)(ecrc >> 8) | (ecrc << 8));
                ecrc ^= Data[i];
                ecrc ^= (ushort)((Byte)(ecrc & 0xff) >> 4);
                ecrc ^= (ushort)((ecrc << 8) << 4);
                ecrc ^= (ushort)(((ecrc & 0xff) << 4) << 1);
            }
            return ecrc;
        }

    }

    static class IncomingPacket
    {
        static public byte Header1;
        static public byte Header2;
        static public ushort Length;
        static public byte[] Payload = new byte[1024];
        static public ushort CRC;
     }

  
}
