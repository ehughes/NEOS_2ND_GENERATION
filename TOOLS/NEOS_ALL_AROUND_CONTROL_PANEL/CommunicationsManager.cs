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
using PlayworldMessages;
using Playworld;


namespace HardwareInterfaceManagement
{

    public delegate void PacketProcessor(byte[] Data);

    public class CommunicationsManager
    {

   

        private int ManagementState = 0;
        public SerialPort MyConnection;

        const int WAITING_FOR_CONNECT_COMMAND = 0x00;
        const int SCANNING_FOR_MODULE = 0x01;
        const int CONNECTING = 0x02;
        const int CONNECTION_ACTIVE = 0x03;
        const int WAIT_FOR_RECONNECT = 0x04;
        const int RECONNECT_PERIOD = 1000;

        public bool ConnectionActive = false;
        private string RegistryCOMPortRecord;

        private bool AttemptToConnect = false;

        public PacketDetect MyPacketDetector;

        byte[] StreamBuffer = new byte[512];

        public int BytesRead = 0;

        //This routine does the job of maintaining an active link.  
        private string[] ManagerMessages = { "Disconnected ", "Connecting", "Connected", "Waiting before retry", "Connection Exception" };
        public string ConnectionStatus;
        private Thread CommunicationsManagerThread;

        public CommunicationsManager()
        {
            ManagementState = WAITING_FOR_CONNECT_COMMAND;

            ConnectionStatus = ManagerMessages[0];
            ConnectionActive = false;

            MyPacketDetector = new PacketDetect();
            CommunicationsManagerThread = new Thread(new ThreadStart(Tick));
            CommunicationsManagerThread.Priority = ThreadPriority.Highest;
            CommunicationsManagerThread.Start();

        }


        public void Tick()
        {

            while (true)
            {

                Thread.Sleep(10);
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
                                ManagementState = WAITING_FOR_CONNECT_COMMAND;
                                //MessageBox.Show("It doesn't appear that a Talking Bob Module is connected.  Try disconnecting and reconnecting USB cable.", "Error Connecting to Talking Bob Module");
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

                            break;

                        case CONNECTING:
                            ConnectionStatus = ManagerMessages[1];
                            ConnectionActive = false;

                            try
                            {
                                MyConnection = new SerialPort();
                                MyConnection.PortName = RegistryCOMPortRecord;
                                MyConnection.BaudRate = 128000;
                                MyConnection.DataBits = 8;
                                MyConnection.StopBits = StopBits.One;
                                MyConnection.Handshake = Handshake.None;
                                MyConnection.Open();
                                ManagementState = CONNECTION_ACTIVE;
                                BytesRead = 0;
                            }

                            catch
                            {
                                Thread.Sleep(RECONNECT_PERIOD);
                                ManagementState = WAITING_FOR_CONNECT_COMMAND;

                            }
                            break;

                        case CONNECTION_ACTIVE:

                            ConnectionActive = true;
                            ConnectionStatus = ManagerMessages[2];
                            ManagementState = CONNECTION_ACTIVE;

                            int BytesToGrab = MyConnection.BytesToRead;

                            if (BytesToGrab > 0)
                            {

                                if (BytesToGrab > 512)
                                {
                                    BytesToGrab = 512;
                                }

                                MyConnection.Read(StreamBuffer, 0, BytesToGrab);
                                BytesRead += BytesToGrab;

                                try
                                {
                                    for (int e = 0; e < BytesToGrab; e++)
                                    {
                                        MyPacketDetector.ParseStream(StreamBuffer[e]);
                                        
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
                    ManagementState = WAITING_FOR_CONNECT_COMMAND;
                }
            }

        }





        public void Write(byte[] BufOut)
        {


            if (ConnectionActive == true)
            {
                MyConnection.Write(BufOut, 0, BufOut.Length);
            }

        }

        public void Close()
        {
            if (MyConnection != null)
            {

                try
                {

                    MyConnection.Close();
                    ConnectionStatus = ManagerMessages[0];
                    ConnectionActive = false;
                    ManagementState = WAITING_FOR_CONNECT_COMMAND;
                }
                catch
                {
                }
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
        static public bool PacketFound;
    }

    public class PacketDetect
    {

        public event PacketProcessor MyPacketProccessor;

        public bool Enable = false;

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

        public int PacketsIn = 0;

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
                        IncomingPacket.PacketFound = false;

                    }
                    else
                    {
                        DetectState = SCAN_FOR_HEADER1;
                        IncomingPacket.PacketFound = false;
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
                        PacketsIn++;
                      
                            if (MyPacketProccessor != null)
                            {
                                byte[] NextPacketToProcess = new byte[IncomingPacket.Length];
                                for (int p = 0; p < IncomingPacket.Length; p++)
                                {
                                    NextPacketToProcess[p] = IncomingPacket.Payload[p];

                                }
                                MyPacketProccessor(NextPacketToProcess);
                            }

                      }
                    else
                    {
                        IncomingPacket.PacketFound = false;
                    }

                    break;


                default:
                    DetectState = SCAN_FOR_HEADER1;
                    break;

            }

        }

    }
}
