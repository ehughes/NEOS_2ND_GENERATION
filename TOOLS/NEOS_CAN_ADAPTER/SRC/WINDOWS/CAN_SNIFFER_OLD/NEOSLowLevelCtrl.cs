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

        public Queue<CANMessage> RxCANMessageQueue = new Queue<CANMessage>();
        public Queue<CANMessage> TxCANMessageQueue = new Queue<CANMessage>();

        Thread CANProcessingThread;
        Thread FlashThread;
        CANMessage NextMessage;

        byte[] GeneralPurposeBuffer = new byte[512];

        const int READ_BUFFER_SEGMENT_TIMEOUT = 50;
        const int WRITE_BUFFER_SEGMENT_TIMEOUT = 50;
        byte NodeForBufferOperation = 0;
        byte NodeForFlashOperation = 0;

        UInt32 FlashOperationAddress;

        const int FLASH_OPERATION_TIMEOUT = 50;
        int MAX_FLASH_OPERATION_RETRIES = 50;
        public String FlashStatusString;
        public String FileToFlash;

        private const int MESSAGE_HISTORY = 64;
        string[] StatusMessageBuffer = new string[MESSAGE_HISTORY];
        int StatusMessagePtr = 0;
        string[] SortedMessageBoxLines = new String[MESSAGE_HISTORY];


        public void PostStatusMessage(String Message)
        {
            int StatusMsgPtrTemp = StatusMessagePtr;

            StatusMessageBuffer[StatusMessagePtr] = Message;
            
            StatusMessagePtr++;
            if (StatusMessagePtr > MESSAGE_HISTORY - 1)
            {
                StatusMessagePtr = 0;
            }

            for (int i = 0; i < MESSAGE_HISTORY; i++)
            {

                if (StatusMsgPtrTemp == 0)
                {
                    StatusMsgPtrTemp = MESSAGE_HISTORY - 1;
                }
                else
                {
                    StatusMsgPtrTemp--;
                }
                SortedMessageBoxLines[i] = StatusMessageBuffer[StatusMsgPtrTemp];
            }

          

        }
        byte NumSegments;
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
               Thread.Sleep(10);
               lock (this.RxCANMessageQueue)
               {
                   for (int q = 0; q < this.RxCANMessageQueue.Count; q++)
                   {
                       NextMessage = RxCANMessageQueue.Dequeue();


                       if ((NextMessage.CANId >= NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.GP_BUFFER_READ_LOWER_CAN_ID_BOUND) && (NextMessage.CANId <= NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.GP_BUFFER_READ_UPPER_CAN_ID_BOUND))
                       {
                           lock (MyNodes)
                           {
                               MyNodes[NodeForBufferOperation].ReadSegmentAck[(NextMessage.CANId - NEOSMessages.GENERAL_PURPOSE_BUFFER_OPERATION_PARAMS.GP_BUFFER_READ_LOWER_CAN_ID_BOUND)] = true;

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
                                   int eli = 5;
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

                                       MyNodes[NextMessage.CANData[1]].Pongs++;
                                       MyNodes[NextMessage.CANData[1]].Ponged = true;
                                       uint UIDCount = MyNodes[NextMessage.CANData[1]].Pongs - 1;
                                       MyNodes[NextMessage.CANData[1]].UID[UIDCount] = (UInt32)(NextMessage.CANData[2]) +
                                                                                       (UInt32)(NextMessage.CANData[3] << 8) +
                                                                                       (UInt32)(NextMessage.CANData[4] << 16) +
                                                                                       (UInt32)(NextMessage.CANData[5] << 24);

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


        private void button1_Click(object sender, EventArgs e)
        {
            ResetNodeInfo();
            PONGResponseDisplay.Text = "";
            NEOSMessages.NODE_OPERATIONS_PARAMS.Node = 247;
            NEOSMessages.NODE_OPERATIONS_PARAMS.Command = NEOSMessages.NODE_OPERATIONS_PARAMS.NODE_PING;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.NODE_OPERATIONS);
            TxCANMessageQueue.Enqueue(Outgoing);
            

        }

        private void PongResponseTimer_Tick(object sender, EventArgs e)
        {
            

            for (int i = 0; i < 256; i++)
            {
                if (MyNodes[i].Ponged == true)
                {
                    for(int j=0;j<MyNodes[i].Pongs;j++)
                    {
                        PONGResponseDisplay.Text = "Node:" + i + " UID:" + MyNodes[i].UID[j] + "\r\n";
                    }

                    MyNodes[i].Ponged = false;
                }
            }
        }

        private void ClearBufferButton_Click(object sender, EventArgs e)
        {
            Cmd_ClearGeneralPurposeBuffer((byte)GPTargetNode.Value);
        }

        private void ReadBufferButton_Click(object sender, EventArgs e)
        {
            String Status="";

            NodeForBufferOperation = (byte)GPTargetNode.Value;

            if (ReadGPBuffer(ref Status) == true)
            {
                UpdateGeneralPurposeMemoryDisplay();
            }
            else
            {
                MessageBox.Show(Status);
            }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            Thread TThread = new Thread(new ThreadStart(TestThread));
            TThread.Start();
        } 
        void TestThread()
        {
            int OperationTimeout;

            NodeForBufferOperation = 247;
            Cmd_SetGeneralPurposeBufferAcceptanceFlag(NodeForBufferOperation);
            OperationTimeout = 0;
            Thread.Sleep(10);
            while (MyNodes[NodeForBufferOperation].SetBufferAcceptanceFlagAck == false)
            {
                OperationTimeout++;
                if (OperationTimeout > READ_BUFFER_SEGMENT_TIMEOUT)
                {
                   
                    MessageBox.Show("Timeout while waiting for acknowledge of SET_BUFFER_ACCEPTANCE_FLAG command on node " + NodeForBufferOperation);

                    return;
                }
                Thread.Sleep(10);
            }
            MessageBox.Show("Good To Go!"); 
            return;
        }

        public bool ReadGPBuffer(ref string StatusString)
        {
            int OperationTimeout;

            lock(StatusString)
            {
             StatusString = "Sending SET_BUFFER_ACCEPTANCE_FLAG message to node " + NodeForBufferOperation;
            }
           
            Cmd_SetGeneralPurposeBufferAcceptanceFlag(NodeForBufferOperation);
            Thread.Sleep(10000);
            OperationTimeout = 0;

            while (MyNodes[NodeForBufferOperation].SetBufferAcceptanceFlagAck == false)
            {
                OperationTimeout++;
                if (OperationTimeout > READ_BUFFER_SEGMENT_TIMEOUT)
                {
                    lock (StatusString)
                    {
                        StatusString = "Timeout while waiting for acknowledge of SET_BUFFER_ACCEPTANCE_FLAG command on node " + NodeForBufferOperation;
                    }
                    
                    return false;
                 }
                 Thread.Sleep(10);
            }


            for (byte i = 0; i < 64; i++)
            {
                
                StatusString = "Sending READ_BUFFER_SEGMENT [" + i + "] command on node " + NodeForBufferOperation ;


                Cmd_ReadGeneralPurposeBufferSegement(NodeForBufferOperation, i);
                
                OperationTimeout = 0;

                Thread.Sleep(200);
                while (MyNodes[NodeForBufferOperation].ReadSegmentAck[i] == false)
                {
                    OperationTimeout++;
                    if (OperationTimeout > READ_BUFFER_SEGMENT_TIMEOUT)
                    {
                        lock (StatusString)
                        {
                            StatusString = "Timeout while waiting for acknowledge of READ_BUFFER_SEGMENT [" + "63" + "] command on node " + NodeForBufferOperation;
                        }
                        return false;
                    }
                    Thread.Sleep(10);
                }


            }


            
            lock (StatusString)
            {
                StatusString = "Sending CLEAR_BUFFER_ACCEPTANCE_FLAG message to node " + NodeForBufferOperation;
            }

           
            Cmd_ClearGeneralPurposeBufferAcceptanceFlag(NodeForBufferOperation);
            OperationTimeout = 0;

            while (MyNodes[NodeForBufferOperation].ClearBufferAcceptanceFlagAck == false)
            {
                OperationTimeout++;
                if (OperationTimeout > READ_BUFFER_SEGMENT_TIMEOUT)
                {
                    lock (StatusString)
                    {
                        StatusString = "Timeout while waiting for acknowledge of CLEAR_BUFFER_ACCEPTANCE_FLAG command on node " + NodeForBufferOperation;
                    }

                    return false;
                }
                Thread.Sleep(10);
            }


            StatusString = "General purpose buffer read successfully from node " + NodeForBufferOperation;
            return true;
        }

        void WriteGPBuffer()
        {
            int OperationTimeout;

            Cmd_SetGeneralPurposeBufferAcceptanceFlag(NodeForBufferOperation);
            OperationTimeout = 0;
            
            while (MyNodes[NodeForBufferOperation].SetBufferAcceptanceFlagAck == false)
            {
                OperationTimeout++;
                if (OperationTimeout > WRITE_BUFFER_SEGMENT_TIMEOUT)
                {
                    PostStatusMessage("Timeout while waiting for acknowledge of SET_BUFFER_ACCEPTANCE_FLAG command on node " + NodeForBufferOperation);

                    return;
                }
                Thread.Sleep(10);
             
            }

            
            for (byte i = 0; i < NumSegments; i++)
            {

                //StatusString = "Sending WRITE_BUFFER_SEGMENT [" + i + "] command on node " + NodeForBufferOperation;
                MyNodes[NodeForBufferOperation].WriteSegmentAck[i] = false;

                NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.Node = NodeForBufferOperation;
                NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.BufferSegment = (byte)i;

                for (int j = 0; j < 8; j++)
                {
                    NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER_PARAMS.WriteData[j] = GeneralPurposeBuffer[(i * 8) + j];
                }

                CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.WRITE_GENERAL_PURPOSE_BUFFER);
                TxCANMessageQueue.Enqueue(Outgoing);

                OperationTimeout = 0;
            }
            
            
            while (MyNodes[NodeForBufferOperation].WriteSegmentAck[NumSegments-1] ==false )
                {
                    OperationTimeout++;
                    if (OperationTimeout > WRITE_BUFFER_SEGMENT_TIMEOUT)
                    {
                            PostStatusMessage("Timeout while waiting for acknowledge of WRITE_BUFFER_SEGMENT [" + "63" + "] command on node " + NodeForBufferOperation);
                            return; 
                    }
                    Thread.Sleep(10);
                   
                }
            

            Cmd_ClearGeneralPurposeBufferAcceptanceFlag(NodeForBufferOperation);
            OperationTimeout = 0;

            while (MyNodes[NodeForBufferOperation].ClearBufferAcceptanceFlagAck == false)
            {
                OperationTimeout++;
                if (OperationTimeout > WRITE_BUFFER_SEGMENT_TIMEOUT)
                {
                    
                        PostStatusMessage("Timeout while waiting for acknowledge of CLEAR_BUFFER_ACCEPTANCE_FLAG command on node " + NodeForBufferOperation);

                        return;
                  
                }
                Thread.Sleep(10);
            }


            PostStatusMessage("General purpose buffer written successfully to node " + NodeForBufferOperation);
            return;
          
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
            NodeForBufferOperation = (byte)GPTargetNode.Value;

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

            }
        }

     

        private void GeneralPurposeBufferGroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void ReadFlashPageIntoGeneralPurposeBufferButton_Click(object sender, EventArgs e)
        {
            int FlashOperationTimer = 0;
            NodeForFlashOperation = (byte)GPTargetNode.Value;

            Cmd_ReadFlashPageIntoGeneralPurposeBuffer(NodeForFlashOperation,(UInt32)FlashAddressControl.Value);

            while (MyNodes[NodeForFlashOperation].ReadFlashPageAck == false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    MessageBox.Show("Timeout waiting for READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER for page " + FlashAddressControl.Value + " on node " + NodeForFlashOperation);
                    return;
                }
                Thread.Sleep(10);
            }

          MessageBox.Show("Page " + FlashAddressControl.Value + " on node " + NodeForFlashOperation + " has been read into the general purpose buffer" );
                    
            return ;
        }

        private void EraseSectorButton_Click(object sender, EventArgs e)
        {
            int FlashOperationTimer = 0;
            NodeForFlashOperation = (byte)GPTargetNode.Value;
            Cmd_EraseFlashSector(NodeForFlashOperation, (UInt32)FlashAddressControl.Value);
            while (MyNodes[NodeForFlashOperation].EraseFlashSectorAck== false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    MessageBox.Show("Timeout waiting for ERASE_SECTOR for sector " + FlashAddressControl.Value + " on node " + NodeForFlashOperation);
                    return;
                }
                Thread.Sleep(10);
            }

            MessageBox.Show("Sector " + FlashAddressControl.Value + " on node " + NodeForFlashOperation + " has been erased");

            return;
        }

        bool EraseFLASHSector(byte Node, UInt32 FlashAddress)
        {
            int FlashOperationTimer = 0;

            Cmd_EraseFlashSector(Node, FlashAddress);
            Thread.Sleep(10);
            while (MyNodes[Node].EraseFlashSectorAck == false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    return false;
                }
                Thread.Sleep(10);
            }
            
            PostStatusMessage("Sector " + FlashAddress + " on node " + Node + "Successfully Erased");
            return true;
        }

        bool WriteFLASHPage(byte Node, UInt32 FlashAddress, ref byte [] Data)
        {
          

            int FlashOperationTimer = 0;
           
            for(int i=0;i<256;i++)
            {
                Data[i] = GeneralPurposeBuffer[i];
            }

            NumSegments = 32;
          /*  if (WriteGPBuffer() == false)
            {
                return false;
            }*/

            Cmd_WriteFlashPageFromGeneralPurposeBuffer(Node, FlashAddress);
            Thread.Sleep(10);
            while (MyNodes[Node].WriteFlashPageAck == false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    PostStatusMessage("No Acknowledge from WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER	at address "+FlashAddress+ " on node " + Node);
                    return false;
                }
                Thread.Sleep(10);
               
            }
            return true;
        }

        private void WriteFlashPagefromGPBufferButton_Click(object sender, EventArgs e)
        {
            int FlashOperationTimer = 0;
            NodeForFlashOperation = (byte)GPTargetNode.Value;
            Cmd_WriteFlashPageFromGeneralPurposeBuffer(NodeForFlashOperation, (UInt32)FlashAddressControl.Value);
            while (MyNodes[NodeForFlashOperation].WriteFlashPageAck == false)
            {
                FlashOperationTimer++;
                if (FlashOperationTimer > FLASH_OPERATION_TIMEOUT)
                {
                    MessageBox.Show("Timeout waiting for WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER for sector " + FlashAddressControl.Value + " on node " + NodeForFlashOperation);
                    return;
                }
                Thread.Sleep(10);
            }

            MessageBox.Show("Page " + FlashAddressControl.Value + " on node " + NodeForFlashOperation + " has been programmed from the general purpose buffer");

            return;
        }


        public void DownloadBinaryKeyFile()
        {

            FileStream fs = new FileStream(FileToFlash, FileMode.Open, FileAccess.Read);
            BinaryReader br = new BinaryReader(fs);
            NodeForFlashOperation = (byte)GPTargetNode.Value;
            byte[] PageToFlash = new byte[256];

            int FlashOperationRetries = 0;
            

            uint FileToFlashLength = (uint)fs.Length;
           
            uint FLASHSectorsToErase;
            uint FLASHPagesToProgram;
            uint LastFlashPageSize;
            int i;
            uint FLASHSectorCounter = 0;
            uint FLASHPageCounter = 0;

            
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
           /*
            for (i = 0; i < FLASHSectorsToErase; i++)
            {
                FlashOperationRetries = 0;
                
                FlashEraseRetryEntry:

                PostStatusMessage("Erasing Sector " + (i + 1) + ":" + FLASHSectorsToErase);
                
                if (EraseFLASHSector(NodeForFlashOperation, FLASHSectorCounter) == false)
                {
                    PostStatusMessage("No response to erase command of sector [" + FLASHSectorCounter + "].  Going to retry....  (" + FlashOperationRetries + ")");
                    FlashOperationRetries++;
                    
                    if(FlashOperationRetries > MAX_FLASH_OPERATION_RETRIES)
                    {
                        PostStatusMessage("Could not erase Flash Sector [" + FLASHSectorCounter + "] after " + MAX_FLASH_OPERATION_RETRIES + " retries.  Timeout error.");
                        return;
                    }   
                    else
                    {
                        goto FlashEraseRetryEntry;
                    }
                  }
                
                FLASHSectorCounter += 0x10000;
            }
            
            */
            FLASHPageCounter = 0;
  
            for (i = 0; i < FLASHPagesToProgram; i++)
            {

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

                if (WriteFLASHPage(NodeForFlashOperation, FLASHPageCounter, ref PageToFlash) == false)
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
      }

        private void WriteFileToFlashButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog MyOpenFile = new OpenFileDialog();

            MyOpenFile.Filter = "Binary files (*.bin)|*.bin|All files (*.*)|*.*";
            MyOpenFile.FilterIndex = 0;
            MyOpenFile.RestoreDirectory = true;

            if (MyOpenFile.ShowDialog() == DialogResult.OK)
            {
                NodeForFlashOperation = (byte)GPTargetNode.Value;
                NodeForBufferOperation = (byte)GPTargetNode.Value;
                FileToFlash = MyOpenFile.FileName;
                FlashThread = new Thread(new ThreadStart(DownloadBinaryKeyFile));
                FlashThread.Priority = ThreadPriority.Highest;
                FlashThread.Start();

            }   
           
        }

        private void FlashStatusUpdate_Tick(object sender, EventArgs e)
        {
            this.Text = "" + RxCANMessageQueue.Count;
            this.StatusMessageBox.Lines = SortedMessageBoxLines;
        }

       
    
    }





}