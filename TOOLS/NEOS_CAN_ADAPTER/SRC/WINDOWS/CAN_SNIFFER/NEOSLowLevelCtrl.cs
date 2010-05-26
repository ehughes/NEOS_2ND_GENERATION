using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CANInterfaceManagement;
using System.Threading;
using System.Text.RegularExpressions;
using System.IO;

namespace CAN_SNIFFER
{
    public partial class NEOSLowLevelCtrl : Form
    {

        public CANMessageQueue RxCANMessageQueue = new CANMessageQueue(65535);
        public CANMessageQueue TxCANMessageQueue = new CANMessageQueue(65535);

        CANMessage NextMessage = new CANMessage();

        Thread CANProcessingThread;
        Thread FlashThread;

        Random MyRandom = new Random();

        byte[] GeneralPurposeBuffer = new byte[512];

        const int READ_BUFFER_SEGMENT_TIMEOUT = 200;
        const int WRITE_BUFFER_SEGMENT_TIMEOUT = 200;
        const int SET_BUFFER_ACCEPTANCE_FLAG_TIMEOUT = 200;
        const int CLEAR_BUFFER_ACCEPTANCE_FLAG_TIMEOUT = 200;
        byte NodeForOperation = 0;
     
        UInt32 FlashOperationAddress;

        const int FLASH_OPERATION_TIMEOUT = 200;
        int MAX_FLASH_OPERATION_RETRIES = 6;
        public String FlashStatusString;
        public String FileToFlash;

        private const int MESSAGE_HISTORY = 64;
        string[] StatusMessageBuffer = new string[MESSAGE_HISTORY];
        int StatusMessagePtr = 0;
        string[] SortedMessageBoxLines = new String[MESSAGE_HISTORY];

        bool MemoryDisplayUpdated = false;
        bool LastOperationSuccessful = false;
       // bool UpdatePONGInfo = false;
        int TotalPongs;

        public void PostStatusMessage(String Message)
        {
           

            StatusMessageBuffer[StatusMessagePtr] = Message;
            
            int StatusMsgPtrTemp = StatusMessagePtr;
            StatusMessagePtr++;
            if (StatusMessagePtr > MESSAGE_HISTORY - 1)
            {
                StatusMessagePtr = 0;
            }

            for (int i = 0; i < MESSAGE_HISTORY; i++)
            {
                SortedMessageBoxLines[i] = StatusMessageBuffer[StatusMsgPtrTemp];
                if (StatusMsgPtrTemp == 0)
                {
                    StatusMsgPtrTemp = MESSAGE_HISTORY - 1;
                }
                else
                {
                    StatusMsgPtrTemp--;
                }
               
            }

          

        }
        byte NumSegments;

        bool EraseFlashBeforeProgramming;

        bool QuietMessaging = false;

        public class NodeInfo
        {

            public UInt32 Pongs = 0;
            public bool Ponged = false;
            public UInt32[] UID = new UInt32[256];
            
            public bool [] WriteSegmentAck = new bool[64];
            public bool [] ReadSegmentAck = new bool[64];

            public bool ClearBufferAck = false;
            public bool ClearBufferAcceptanceFlagAck = false;
            public bool SetBufferAcceptanceFlagAck = false;

            public bool EraseFlashSectorAck = false;
            public bool ReadFlashPageAck = false;
            public bool WriteFlashPageAck = false;

            public double NodeVoltage;

            public NodeInfo()
            {
               for (int i = 0; i < 256; i++)
                {
                    UID[i] = 0;
                }   
            }
        }

        NodeInfo[] MyNodes = new NodeInfo[256];


        void InitNodeInfo()
        {
            for (int i = 0; i < 256; i++)
            {
                MyNodes[i] = new NodeInfo();
                for (int j = 0; j < 64; j++)
                {

                    MyNodes[i].WriteSegmentAck[j] = false;
                    MyNodes[i].ReadSegmentAck[j] = false;

                }

                MyNodes[i].ClearBufferAck = false;
                MyNodes[i].ClearBufferAcceptanceFlagAck = false;
                MyNodes[i].SetBufferAcceptanceFlagAck = false;
                MyNodes[i].EraseFlashSectorAck = false;
                MyNodes[i].ReadFlashPageAck = false;
                MyNodes[i].WriteFlashPageAck = false;
              
            }

        }

        void ResetNodeInfo()
        {
            for (int i = 0; i < 256; i++)
            {
                MyNodes[i].Pongs = 0;
                MyNodes[i].Ponged = true;
                for (int j = 0; j < 256; j++)
                {
                    MyNodes[i].UID[j] = 0;
                    for (int k = 0; k < 64; k++)
                    {

                        MyNodes[k].WriteSegmentAck[k] = false;
                        MyNodes[k].ReadSegmentAck[k] = false;

                    }
                }
                MyNodes[i].ClearBufferAck = false;
                MyNodes[i].ClearBufferAcceptanceFlagAck = false;
                MyNodes[i].SetBufferAcceptanceFlagAck = false;
                MyNodes[i].EraseFlashSectorAck = false;
                MyNodes[i].ReadFlashPageAck = false;
                MyNodes[i].WriteFlashPageAck = false;
            }
            TotalPongs = 0;
        }

        public NEOSLowLevelCtrl()
        {
            InitializeComponent();

            //this.RxCANMessageQueue.Clear();
            InitNodeInfo();
            ResetNodeInfo();

            CANProcessingThread = new Thread(new ThreadStart(CANRxProcess));
            CANProcessingThread.Priority = ThreadPriority.Highest;
            CANProcessingThread.Start();

            UpdateGeneralPurposeMemoryDisplay();

            MasterSystemModeComboBox.SelectedIndex = 0;
            
        }

        void CANRxProcess()
        {
            while (true)
            {
               Thread.Sleep(0);
               lock (RxCANMessageQueue)
               {
                   for (int q = 0; q < this.RxCANMessageQueue.GetCount(); q++)
                   {
                       RxCANMessageQueue.Dequeue(ref NextMessage);


                       if ((NextMessage.CANId >= NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.GP_BUFFER_READ_LOWER_CAN_ID_BOUND) && (NextMessage.CANId <= NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.GP_BUFFER_READ_UPPER_CAN_ID_BOUND))
                       {
                           lock (MyNodes)
                           {
                               MyNodes[NodeForOperation].ReadSegmentAck[(NextMessage.CANId - NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.GP_BUFFER_READ_LOWER_CAN_ID_BOUND)] = true;

                           }

                           uint BufferBase = NextMessage.CANId - NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.GP_BUFFER_READ_LOWER_CAN_ID_BOUND;

                           for (uint i = 0; i < 8; i++)
                           {
                               GeneralPurposeBuffer[(BufferBase * 8) + i] = NextMessage.CANData[i];
                           }
                       }
                       switch (NextMessage.CANId)
                       {
                           case 0:
                              
                               break;

                            
                           case NEOSMessages.NODE_FLASH_MEMORY_OPERATION:
                               lock (MyNodes)
                               {
                                   if ((NextMessage.CANData[1] & 0x80) > 0)
                                   {
                                       switch ((byte)(NextMessage.CANData[1] & 0x7F))
                                       {
                                           case NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER:
                                               MyNodes[NextMessage.CANData[0]].WriteFlashPageAck = true;
                                               break;
                                           case NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER:
                                               MyNodes[NextMessage.CANData[0]].ReadFlashPageAck = true;
                                               break;
                                           case NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.ERASE_SECTOR:
                                               MyNodes[NextMessage.CANData[0]].EraseFlashSectorAck = true;
                                               break;

                                       }
                                   }
                               }

                               break;


                           case NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION:

                               if ((NextMessage.CANData[1] & 0x80) > 0)
                               {

                                   switch ((byte)(NextMessage.CANData[1] & 0x7F))
                                   {

                                       case NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CLEAR_BUFFER:
                                           MyNodes[NextMessage.CANData[0]].ClearBufferAck = true;

                                           break;

                                       case NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CLEAR_BUFFER_ACCEPTANCE_FLAG:
                                           MyNodes[NextMessage.CANData[0]].ClearBufferAcceptanceFlagAck = true;

                                           break;

                                       case NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.SET_BUFFER_ACCEPTANCE_FLAG:
                                           MyNodes[NextMessage.CANData[0]].SetBufferAcceptanceFlagAck = true;

                                           break;

                                       case NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.WRITE_BUFFER_ACK:
                                           MyNodes[NextMessage.CANData[0]].WriteSegmentAck[NextMessage.CANData[2]] = true;
                                           break;

                                       default:
                                           break;
                                   }
                               }


                               break;

                           case NEOSMessages.NODE_OPERATIONS:

                               if (NextMessage.CANData[0] == NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_PONG)
                               {
                                   TotalPongs++;
                                   MyNodes[NextMessage.CANData[1]].Pongs++;
                                   MyNodes[NextMessage.CANData[1]].Ponged = true;
                                   uint UIDCount = MyNodes[NextMessage.CANData[1]].Pongs - 1;
                                   MyNodes[NextMessage.CANData[1]].UID[UIDCount] = (UInt32)(NextMessage.CANData[2]) +
                                                                                   (UInt32)(NextMessage.CANData[3] << 8) +
                                                                                   (UInt32)(NextMessage.CANData[4] << 16) +
                                                                                   (UInt32)(NextMessage.CANData[5] << 24);

                               }

                               if (NextMessage.CANData[0] == NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_SUPPLY_VOLTAGE_RESPONSE)
                               {
                                   MyNodes[NextMessage.CANData[1]].NodeVoltage = (float)(NextMessage.CANData[2] + (NextMessage.CANData[3] << 8)) / 310.0;
                               }

                               break;
                           default:
                               break;
                       }
                   }
               }
              
            }
        }
        
        void UpdateGeneralPurposeMemoryDisplay()
        {
            string View = "";

            for (int i = 0; i < 64; i++)
            {
                View += "0x" +  GeneralPurposeBuffer[(i * 8) + 0].ToString("X2") + " " +
                         "0x" + GeneralPurposeBuffer[(i * 8) + 1].ToString("X2") + " " +
                         "0x" + GeneralPurposeBuffer[(i * 8) + 2].ToString("X2") + " " +
                         "0x" + GeneralPurposeBuffer[(i * 8) + 3].ToString("X2") + " " +
                         "0x" + GeneralPurposeBuffer[(i * 8) + 4].ToString("X2") + " " +
                         "0x" + GeneralPurposeBuffer[(i * 8) + 5].ToString("X2") + " " +
                         "0x" + GeneralPurposeBuffer[(i * 8) + 6].ToString("X2") + " " +
                         "0x" + GeneralPurposeBuffer[(i * 8) + 7].ToString("X2") + "\r\n";
            }

            GeneralPurposeBufferView.Text = View;

        }

        void ValidateGeneralPurposeMemoryDisplay()
        {
            byte[] TempBuf = new byte[512];

            int NumLines = GeneralPurposeBufferView.Lines.Length;

            if (NumLines > 64) { NumLines = 64; }
                {
                  for (int i = 0; i < NumLines; i++)
                    {
                        String RegexString = @"(\s*0x\w+\s*)";

                        MatchCollection MyMatches = Regex.Matches(GeneralPurposeBufferView.Lines[i], RegexString);
                        Match[] EMatch = new Match[MyMatches.Count];
                        MyMatches.CopyTo(EMatch, 0);
                      
                     
                        UInt32[] ElementsInLine = new UInt32[MyMatches.Count];

                        for (int k = 0; k < EMatch.Length; k++)
                        {
                            string[] splits = EMatch[k].Value.Split('x');

                            try
                            {
                                ElementsInLine[k] = UInt32.Parse(splits[1], System.Globalization.NumberStyles.HexNumber);

                            }
                            catch
                            {
                                ElementsInLine[k] = 0;
                            }
                         }

                         if (ElementsInLine.Length > 8)
                         {
                             for (int j = 0; j < 8; j++)
                             {
                                 TempBuf[(i * 8) + j] = (byte)ElementsInLine[j];
                             }
                         }
                         else 
                         {
                             for (int j = 0; j<ElementsInLine.Length ; j++)
                             {
                                 TempBuf[(i * 8) + j] = (byte)ElementsInLine[j];
                             }
                         }
                    }
                }

                for (int l = 0; l < TempBuf.Length; l++)
                {
                    GeneralPurposeBuffer[l] = TempBuf[l];
                }
             UpdateGeneralPurposeMemoryDisplay();
        }

        void Cmd_ClearGeneralPurposeBuffer(byte Node)
        {
            MyNodes[Node].ClearBufferAck = false;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.Node = Node;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CommandByte = NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CLEAR_BUFFER;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION);
            TxCANMessageQueue.Enqueue(Outgoing);
        }

        void Cmd_ClearGeneralPurposeBufferAcceptanceFlag(byte Node)
        {
            MyNodes[Node].ClearBufferAcceptanceFlagAck = false;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.Node = Node;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CommandByte = NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CLEAR_BUFFER_ACCEPTANCE_FLAG;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION);
            TxCANMessageQueue.Enqueue(Outgoing);
        }

        void Cmd_SetGeneralPurposeBufferAcceptanceFlag(byte Node)
        {
            MyNodes[Node].SetBufferAcceptanceFlagAck = false;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.Node = Node;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CommandByte = NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.SET_BUFFER_ACCEPTANCE_FLAG;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION);
            TxCANMessageQueue.Enqueue(Outgoing);
        }

        void Cmd_ReadGeneralPurposeBufferSegement(byte Node, byte BufferSegment)
        {
            MyNodes[Node].ReadSegmentAck[BufferSegment] = false;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.Node = Node;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.BufferSegment = BufferSegment;
            NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.CommandByte = NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.READ_BUFFER_SEGMENT;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION);
            TxCANMessageQueue.Enqueue(Outgoing);

        }

        void Cmd_WriteGeneralPurposeBufferSegement(byte Node, byte BufferSegment, byte [] Data)
        {
            MyNodes[Node].WriteSegmentAck[BufferSegment] = false;
            NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.Node = Node;
            NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.BufferSegment = BufferSegment;

            for (int i = 0; i < 8; i++)
            {
                NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[i] = Data[i];
                
            }

            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER);
            TxCANMessageQueue.Enqueue(Outgoing);

        }

        void Cmd_ReadFlashPageIntoGeneralPurposeBuffer(byte Node, UInt32 FlashAddress)
        {
            MyNodes[Node].ReadFlashPageAck = false;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.CommandByte = NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.Node = Node;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.FlashAddress = FlashAddress;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_FLASH_MEMORY_OPERATION);
            TxCANMessageQueue.Enqueue(Outgoing);
        }

        void Cmd_WriteFlashPageFromGeneralPurposeBuffer(byte Node, UInt32 FlashAddress)
        {
            MyNodes[Node].WriteFlashPageAck = false;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.CommandByte = NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.Node = Node;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.FlashAddress = FlashAddress;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_FLASH_MEMORY_OPERATION);
            TxCANMessageQueue.Enqueue(Outgoing);
        }

        void Cmd_EraseFlashSector(byte Node, UInt32 FlashAddress)
        {
            MyNodes[Node].EraseFlashSectorAck = false;

            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.CommandByte = NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.ERASE_SECTOR;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.Node = Node;
            NEOSMessages.NODE_FLASH_MEMORY_OPERATION_PARAMS.FlashAddress = FlashAddress;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_FLASH_MEMORY_OPERATION);
            TxCANMessageQueue.Enqueue(Outgoing);
        }
          
        private void PongResponseTimer_Tick(object sender, EventArgs e)
        {
            this.Text = "Pongs :" + TotalPongs;               
                for (int i = 0; i < 256; i++)
                {
                    if (MyNodes[i].Ponged == true)
                    {
                        for (int j = 0; j < MyNodes[i].Pongs; j++)
                        {
                            if (i == 0 && j == 0)
                            {
                                PONGResponseDisplay.Text = "Node: 0x" + i.ToString("X2") + " UID: 0x" + MyNodes[i].UID[j].ToString("X8") + "\r\n";
                            }
                            else
                            {
                                PONGResponseDisplay.Text += "Node: 0x" + i.ToString("X2") + " UID: 0x" + MyNodes[i].UID[j].ToString("X8") + "\r\n";
                            
                            }
                       }
                       MyNodes[i].Ponged = false;

                    }
                }
                
        }

        private void ClearBufferButton_Click(object sender, EventArgs e)
        {
           NodeForOperation = (byte)GPTargetNode.Value;
           ClearGPBuffer();
        }

        private void ReadBufferButton_Click(object sender, EventArgs e)
        {
           
            NodeForOperation = (byte)GPTargetNode.Value;
            NumSegments = 64;
            Thread E = new Thread(new ThreadStart(ReadGPBuffer));
            E.Start();
        }
     
        void ClearGPBuffer()
        {
            int OperationTimeout;
            if (QuietMessaging == false)
            {
                PostStatusMessage("Sending CLEAR_BUFFER_BUFFER message to node " + NodeForOperation);
            }
            Cmd_ClearGeneralPurposeBuffer(NodeForOperation);

            OperationTimeout = 0;

            while (MyNodes[NodeForOperation].ClearBufferAck== false)
            {
                OperationTimeout++;
                if (OperationTimeout > SET_BUFFER_ACCEPTANCE_FLAG_TIMEOUT)
                {
                    PostStatusMessage("Timeout while waiting for acknowledge of CLEAR_BUFFER command on node " + NodeForOperation);
                    ResetCANQueue();
                    LastOperationSuccessful = false;
                    return;
                }
                Thread.Sleep(10);
            }

            if (QuietMessaging == false)
            {
                PostStatusMessage("CLEAR_BUFFER message acknowledged by node " + NodeForOperation);
            }
            LastOperationSuccessful = true;

        }

        void ResetCANQueue()
        {
            this.TxCANMessageQueue.Clear();
            this.RxCANMessageQueue.Clear();


        }

        void SetBufferAcceptanceFlag()
        {
            int OperationTimeout;

            if (QuietMessaging == false)
            {
                PostStatusMessage("Sending SET_BUFFER_ACCEPTANCE_FLAG message to node " + NodeForOperation);
            }
            Cmd_SetGeneralPurposeBufferAcceptanceFlag(NodeForOperation);

            OperationTimeout = 0;

            while (MyNodes[NodeForOperation].SetBufferAcceptanceFlagAck == false)
            {
                OperationTimeout++;
                if (OperationTimeout > SET_BUFFER_ACCEPTANCE_FLAG_TIMEOUT)
                {
                    PostStatusMessage("Timeout while waiting for acknowledge of SET_BUFFER_ACCEPTANCE_FLAG command on node " + NodeForOperation);
                    ResetCANQueue();
                    LastOperationSuccessful = false;
                    return;
                }
                Thread.Sleep(10);

                
            }

            LastOperationSuccessful = true;
            if (QuietMessaging == false)
            {
                PostStatusMessage("SET_BUFFER_ACCEPTANCE_FLAG message Acknowledged by node " + NodeForOperation);
            }
        }

        void ClearBufferAcceptanceFlag()
        {
            int OperationTimeout;
            if (QuietMessaging == false)
            {
                PostStatusMessage("Sending CLEAR_BUFFER_ACCEPTANCE_FLAG message to node " + NodeForOperation);
            }


            Cmd_ClearGeneralPurposeBufferAcceptanceFlag(NodeForOperation);
            OperationTimeout = 0;

            while (MyNodes[NodeForOperation].ClearBufferAcceptanceFlagAck == false)
            {
                OperationTimeout++;
                if (OperationTimeout > CLEAR_BUFFER_ACCEPTANCE_FLAG_TIMEOUT)
                {

                    PostStatusMessage("Timeout while waiting for acknowledge of CLEAR_BUFFER_ACCEPTANCE_FLAG command on node " + NodeForOperation);
                    ResetCANQueue();
                    LastOperationSuccessful = false;
                    return;
                }
                Thread.Sleep(10);
            }
            if (QuietMessaging == false)
            {
                PostStatusMessage("CLEAR_BUFFER_ACCEPTANCE_FLAG message Acknowledged by node " + NodeForOperation);
            }
            LastOperationSuccessful = true;
        }

       void ReadGPBuffer()
        {
            int OperationTimeout;

            SetBufferAcceptanceFlag();

            if (LastOperationSuccessful == true)
            {
                for (byte i = 0; i < NumSegments; i++)
                {
                    if (QuietMessaging == false)
                    {
                        PostStatusMessage("Sending READ_BUFFER_SEGMENT [" + i + "] command on node " + NodeForOperation);
                    }

                    Cmd_ReadGeneralPurposeBufferSegement(NodeForOperation, i);
                    Thread.Sleep(10);
                    OperationTimeout = 0;

                    while (MyNodes[NodeForOperation].ReadSegmentAck[i] == false)
                    {
                        OperationTimeout++;
                        if (OperationTimeout > READ_BUFFER_SEGMENT_TIMEOUT)
                        {
                            PostStatusMessage("Timeout while waiting for acknowledge of READ_BUFFER_SEGMENT [" + i + "] command on node " + NodeForOperation);
                            ResetCANQueue();
                            LastOperationSuccessful = false;
                            return;
                        }
                        Thread.Sleep(10);
                    }
                    MemoryDisplayUpdated = true;
                }

                ClearBufferAcceptanceFlag();
                if (LastOperationSuccessful == true)
                {
                    PostStatusMessage("General purpose buffer read successfully from node " + NodeForOperation);
                }
            }


        }

        void WriteGPBuffer()
        {
            int OperationTimeout;
            
            SetBufferAcceptanceFlag();

            if (LastOperationSuccessful == true)
            {
                OperationTimeout = 0;

                for (byte i = 0; i < NumSegments; i++)
                {
                    if (QuietMessaging == false)
                    {
                        PostStatusMessage("Sending WRITE_BUFFER_SEGMENT [" + i + "] command on node " + NodeForOperation); ;
                    }
                    
                    MyNodes[NodeForOperation].WriteSegmentAck[i] = false;

                    NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.Node = NodeForOperation;
                    NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.BufferSegment = (byte)i;

                    for (int j = 0; j < 8; j++)
                    {
                        NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[j] = GeneralPurposeBuffer[(i * 8) + j];
                    }

                    CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER);
                    TxCANMessageQueue.Enqueue(Outgoing);


                }


                while (MyNodes[NodeForOperation].WriteSegmentAck[NumSegments - 1] == false)
                {
                    OperationTimeout++;
                    if (OperationTimeout > WRITE_BUFFER_SEGMENT_TIMEOUT)
                    {
                        PostStatusMessage("Timeout while waiting for acknowledge of WRITE_BUFFER_SEGMENT [" + "63" + "] command on node " + NodeForOperation);
                        ResetCANQueue();
                        LastOperationSuccessful = false;
                        return;
                    }
                    Thread.Sleep(10);

                }

                ClearBufferAcceptanceFlag();
                if (LastOperationSuccessful == true)
                {
                    PostStatusMessage("General purpose buffer written successfully to node " + NodeForOperation);
                }
                 
            }
        }

        void SetMasterMode(byte Mode)
        {
            NEOSMessages.SET_MASTER_SYSTEM_MODE_PARAMS.Mode = Mode;
            CANMessage OutMessage = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SET_MASTER_SYSTEM_MODE);
            TxCANMessageQueue.Enqueue(OutMessage);
        }

        private void GeneralPurposeBufferView_TextChanged(object sender, EventArgs e)
        {
           
        }

        private void GeneralPurposeBufferView_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Clicks == 2)
            {

            }
        }

        private void WriteBufferButton_Click(object sender, EventArgs e)
        {
            ValidateGeneralPurposeMemoryDisplay();
            MemoryDisplayUpdated = true;
            NodeForOperation = (byte)GPTargetNode.Value;

            NumSegments = 64;
             Thread E = new Thread(new ThreadStart(WriteGPBuffer));
             E.Start();
          
        }

        private void SetMasterSystemModeButton_Click(object sender, EventArgs e)
        {
            switch ((string)MasterSystemModeComboBox.SelectedItem)
            {
                case "GAME_ACTIVE":
                    SetMasterMode(NEOSMessages.SET_MASTER_SYSTEM_MODE_PARAMS.GAME_ACTIVE);
                    break;

                case "USB_FLASH_UPDATE":
                    SetMasterMode(NEOSMessages.SET_MASTER_SYSTEM_MODE_PARAMS.USB_FLASH_UPDATE);
                    break;

                case "SYSTEM_IDLE":
                    SetMasterMode(NEOSMessages.SET_MASTER_SYSTEM_MODE_PARAMS.SYSTEM_IDLE);
                    break;

                case "SYSTEM_SOFTWARE_RESET":
                    SetMasterMode(NEOSMessages.SET_MASTER_SYSTEM_MODE_PARAMS.SYSTEM_SOFTWARE_RESET);
                    break;
                case "SYSTEM_DIAGNOSTICS":
                    SetMasterMode(NEOSMessages.SET_MASTER_SYSTEM_MODE_PARAMS.SYSTEM_DIAGNOSTICS);
                    break;

            }
        }

        private void GeneralPurposeBufferGroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void ReadFlashPageIntoGeneralPurposeBufferButton_Click(object sender, EventArgs e)
        {
            NodeForOperation = (byte)GPTargetNode.Value;
            FlashOperationAddress = (uint)FlashAddressControl.Value;
            ReadFlashPageIntoGeneralPurposeBuffer();
        }

        void ReadFlashPageIntoGeneralPurposeBuffer()
        {
            int FlashOperationTimer = 0;

            Cmd_ReadFlashPageIntoGeneralPurposeBuffer(NodeForOperation, FlashOperationAddress);

            while (MyNodes[NodeForOperation].ReadFlashPageAck == false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    PostStatusMessage("Timeout waiting for READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER for page " + FlashOperationAddress + " on node " + NodeForOperation);
                    ResetCANQueue();
                    LastOperationSuccessful = false;
                    return;
                }
                Thread.Sleep(10);
            }

            PostStatusMessage("Page " + FlashOperationAddress + " on node " + NodeForOperation + " has been read into the general purpose buffer");
            LastOperationSuccessful = true;
            return;

        }

        private void WriteFlashPagefromGPBufferButton_Click(object sender, EventArgs e)
        {
            NodeForOperation = (byte)GPTargetNode.Value;
            FlashOperationAddress = (uint)FlashAddressControl.Value;
            WriteFlashPagefromGPBuffer();
        }

        private void WriteFlashPagefromGPBuffer()
        {
            int FlashOperationTimer = 0;
            Cmd_WriteFlashPageFromGeneralPurposeBuffer(NodeForOperation, FlashOperationAddress);
       
            while (MyNodes[NodeForOperation].WriteFlashPageAck == false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    PostStatusMessage("Timeout waiting for WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER for sector " + FlashOperationAddress + " on node " + NodeForOperation);
                    ResetCANQueue();
                    LastOperationSuccessful = false;
                    return;
                }
                Thread.Sleep(10);
            }

            PostStatusMessage("Page " + FlashOperationAddress + " on node " + NodeForOperation + " has been programmed from the general purpose buffer");
            LastOperationSuccessful = true;
            return;

        }

        private void EraseSectorButton_Click(object sender, EventArgs e)
        {
            NodeForOperation = (byte)GPTargetNode.Value;
            FlashOperationAddress = (uint)FlashAddressControl.Value;
            EraseSector();
        }

        void EraseSector()
        {
            int FlashOperationTimer = 0;
           
            Cmd_EraseFlashSector(NodeForOperation, FlashOperationAddress);
            while (MyNodes[NodeForOperation].EraseFlashSectorAck == false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    PostStatusMessage("Timeout waiting for ERASE_SECTOR for sector " + FlashOperationAddress + " on node " + NodeForOperation);
                    ResetCANQueue();
                    LastOperationSuccessful = false;
                    return;
                }
                Thread.Sleep(10);
            }

            PostStatusMessage("Sector " + FlashOperationAddress + " on node " + NodeForOperation + " has been erased");
            LastOperationSuccessful = true;
            return;

        }
     
        bool WriteFLASHPage(byte Node, UInt32 FlashAddress,  byte [] Data)
        {
           
            for(int i=0;i<256;i++)
            {
                GeneralPurposeBuffer[i] = Data[i];

            }
            MemoryDisplayUpdated = true;

            NumSegments = 32;
            WriteGPBuffer();

            if (LastOperationSuccessful == false)
            {
                return false;
            }

            FlashOperationAddress = FlashAddress;

            WriteFlashPagefromGPBuffer();

            return LastOperationSuccessful;   

        }

        public void DownloadBinaryKeyFile()
        {

            FileStream fs = new FileStream(FileToFlash, FileMode.Open, FileAccess.Read);
            BinaryReader br = new BinaryReader(fs);
            NodeForOperation = (byte)GPTargetNode.Value;
            byte[] PageToFlash = new byte[256];

            int FlashOperationRetries = 0;
            

            uint FileToFlashLength = (uint)fs.Length;
           
            uint FLASHSectorsToErase;
            uint FLASHPagesToProgram;
            uint LastFlashPageSize;
            int i;
            uint FLASHSectorCounter = 0;
            uint FLASHPageCounter = 0;

            QuietMessaging = true;

            for (i = 0; i < 256; i++)
            {
                PageToFlash[i] = (byte)0;
            }

            fs.Position = 0;
           //First, Figure out the number of sectors (64k blocks)
            FLASHSectorsToErase = (FileToFlashLength >> 16) + 1;
            FLASHPagesToProgram = FileToFlashLength >> 8;
            if ((FileToFlashLength & 0xFF) > 0)
            {
                FLASHPagesToProgram++; //Make sure to get the Scragglers
                LastFlashPageSize = FileToFlashLength & 0xFF;
            }
            else
            {
                LastFlashPageSize = 0;
            }



            if (EraseFlashBeforeProgramming == true)
            {
                for (i = 0; i < FLASHSectorsToErase; i++)
                {
                    FlashOperationRetries = 0;

                FlashEraseRetryEntry:

                    PostStatusMessage("Erasing Sector " + (i + 1) + ":" + FLASHSectorsToErase);
                   
                    FlashOperationAddress = FLASHSectorCounter;
                    EraseSector();
                    if (LastOperationSuccessful == false)
                    {
                        PostStatusMessage("No response to erase command of sector [" + FLASHSectorCounter + "].  Going to retry....  (" + FlashOperationRetries + ")");
                        FlashOperationRetries++;

                        if (FlashOperationRetries > MAX_FLASH_OPERATION_RETRIES)
                        {
                            PostStatusMessage("Could not erase Flash Sector [" + FLASHSectorCounter + "] after " + MAX_FLASH_OPERATION_RETRIES + " retries.");
                            return;
                        }
                        else
                        {
                            goto FlashEraseRetryEntry;
                        }
                    }

                    FLASHSectorCounter += 0x10000;
                }
            }
          
            FLASHPageCounter = 0;
  
            for (i = 0; i < FLASHPagesToProgram; i++)
            //for (i = 0; i < 2; i++)
            {
                //Thread.Sleep(250);
                FlashOperationRetries = 0;

                PostStatusMessage("Programming Page " + (i + 1) + ":" + FLASHPagesToProgram);
              
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


               FlashWriteRetry:
                FlashOperationAddress = FLASHSectorCounter;
               if (WriteFLASHPage(NodeForOperation, FLASHPageCounter, PageToFlash) == false)
                {

                    FlashOperationRetries++;

                    if(FlashOperationRetries > MAX_FLASH_OPERATION_RETRIES)
                    {
                        PostStatusMessage("Could not program page [" + FLASHPageCounter + "] after " + MAX_FLASH_OPERATION_RETRIES + " retries. ");
                        return;
                    }
                    else
                    {
                         PostStatusMessage("No response to program command at sector [" + FLASHPageCounter + "].  Going to retry....  (" + FlashOperationRetries + ")");
                         goto FlashWriteRetry;
                    }
                 }

                FLASHPageCounter += 0x100;
            }

            
            PostStatusMessage(" pages (" + FLASHPagesToProgram * 256 + "  bytes) successfully programmed");
       
            br.Close();
            fs.Close();

            QuietMessaging = false;
      }

        private void WriteFileToFlashButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog MyOpenFile = new OpenFileDialog();

            MyOpenFile.Filter = "Binary files (*.bin)|*.bin|All files (*.*)|*.*";
            MyOpenFile.FilterIndex = 0;
            MyOpenFile.RestoreDirectory = true;
            EraseFlashBeforeProgramming = EraseFLASHCB.Checked;

            if (MyOpenFile.ShowDialog() == DialogResult.OK)
            {
                NodeForOperation = (byte)GPTargetNode.Value;
                NodeForOperation = (byte)GPTargetNode.Value;
                FileToFlash = MyOpenFile.FileName;
                FlashThread = new Thread(new ThreadStart(DownloadBinaryKeyFile));
                FlashThread.Priority = ThreadPriority.Highest;
                FlashThread.Start();

            }   
           
        }

        private void FlashStatusUpdate_Tick(object sender, EventArgs e)
        {
            NodeVoltage.Text = "";
            for(int i=0;i<8;i++)
            {
                NodeVoltage.Text += "Node         [" + i + "] :" + MyNodes[i].NodeVoltage.ToString("0.00") + "v\r\n";
            }
            NodeVoltage.Text +=     "Node [Master] :" + MyNodes[247].NodeVoltage.ToString("0.00") + "v\r\n";

            this.StatusMessageBox.Lines = SortedMessageBoxLines;
            if (MemoryDisplayUpdated == true)
            {
                UpdateGeneralPurposeMemoryDisplay();
                MemoryDisplayUpdated = false;
            }

        }

        private void SetBufferAcceptanceFlagButton_Click(object sender, EventArgs e)
        {
            NodeForOperation = (byte)GPTargetNode.Value;
            SetBufferAcceptanceFlag();
        }

        private void ClearBufferAcceptanceFlagButton_Click(object sender, EventArgs e)
        {
            NodeForOperation = (byte)GPTargetNode.Value;
            ClearBufferAcceptanceFlag();
        }

        private void NEOSLowLevelCtrl_Load(object sender, EventArgs e)
        {

        }

        private void SendUIDButton_Click(object sender, EventArgs e)
        {

            ResetNodeInfo();
            PONGResponseDisplay.Text = "";
            NEOSMessages.NODE_OPERATIONS_PARAMS.Node = (byte)this.TargetNodeForUID.Value;
            if (StatisticallyAcceptUIDCB.Checked == true)
            {
                NEOSMessages.NODE_OPERATIONS_PARAMS.Command = NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_STATISCALLY_ACCEPT_UID;
            }
            else
            {
                NEOSMessages.NODE_OPERATIONS_PARAMS.Command = NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_UNCONDITIONALLY_ACCEPT_UID;
            }

            NEOSMessages.NODE_OPERATIONS_PARAMS.UID = (uint)MyRandom.Next();
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_OPERATIONS);
            TxCANMessageQueue.Enqueue(Outgoing);
          
        }

        private void NEOSLowLevelCtrl_FormClosing(object sender, FormClosingEventArgs e)
        {
            
        }

        public void Terminate()
        {
            this.CANProcessingThread.Abort();
        }

        private void MasterSystemModeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void ReadNodeSupplyVoltage_Click(object sender, EventArgs e)
        {
            GetSupplyVoltages();

        }

        void GetSupplyVoltages()
        {
            CANMessage Outgoing;

            for (byte i = 0; i < 8; i++)
            {
                NEOSMessages.NODE_OPERATIONS_PARAMS.Node = i;
                NEOSMessages.NODE_OPERATIONS_PARAMS.Command = NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_READ_SUPPLY_VOLTAGE;

                Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_OPERATIONS);
                TxCANMessageQueue.Enqueue(Outgoing);
            }
            NEOSMessages.NODE_OPERATIONS_PARAMS.Node = 0xF7;
            NEOSMessages.NODE_OPERATIONS_PARAMS.Command = NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_READ_SUPPLY_VOLTAGE;

            Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_OPERATIONS);
            TxCANMessageQueue.Enqueue(Outgoing);

        }

        private void RequestNodeVoltages_Tick(object sender, EventArgs e)
        {
            if (GetNodeVoltagesCB.Checked == true)
            {
                GetSupplyVoltages();
            }
        }

        private void PingButton_Click(object sender, EventArgs e)
        {
            ResetNodeInfo();
            PONGResponseDisplay.Text = "";

            for (int i = 0; i < 8; i++)
            {
                NEOSMessages.NODE_OPERATIONS_PARAMS.Node = (byte)i;
                NEOSMessages.NODE_OPERATIONS_PARAMS.Command = NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_PING;
                CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_OPERATIONS);
                TxCANMessageQueue.Enqueue(Outgoing);
            }
        }
    }





}