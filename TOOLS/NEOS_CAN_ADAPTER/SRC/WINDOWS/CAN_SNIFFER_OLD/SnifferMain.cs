using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CANInterfaceManagement;
using System.Text.RegularExpressions;
using System.Threading;

namespace CAN_SNIFFER
{
    public partial class CANSnifferMainForm : Form
    {
        public CANCommunicationsManager MyCANCommunicationsManager = new CANCommunicationsManager();
        public SystemMessageView MySystemMessageView = new SystemMessageView();
        public CANMessageEntryForm MyCANMessageEntryForm = new CANMessageEntryForm();
        public IncomingCANMessageView MyIncomingCANMessageView = new IncomingCANMessageView();
        public NEOSAllaroundSimulator MyNEOSControlPanel = new NEOSAllaroundSimulator();
        public bool CurrentCANTerminationState;
        public Thread MessageRouterThread;
        public MessageRouterControl MyMessageRouterControl = new MessageRouterControl();
        public NEOSLowLevelCtrl MyNEOSLowLevelCtrl = new NEOSLowLevelCtrl();


        public CANSnifferMainForm()
        {
            InitializeComponent();
            MySystemMessageView.MdiParent = this;
            MySystemMessageView.Show();
            MyCANMessageEntryForm.MdiParent = this;
            MyCANMessageEntryForm.Show();
            MyIncomingCANMessageView.MdiParent = this;
            MyIncomingCANMessageView.Show();

            MyNEOSControlPanel.MdiParent = this;
            MyNEOSControlPanel.Show();
            MyMessageRouterControl.MdiParent = this;
            MyMessageRouterControl.Show();



            MyNEOSLowLevelCtrl.MdiParent = this;
            MyNEOSLowLevelCtrl.Show();
            MessageRouterThread = new Thread(new ThreadStart(CANMessageRouter));
            MessageRouterThread.Priority = ThreadPriority.Highest;
            MessageRouterThread.Start();
           
        }

        private void CANSnifferMainFormUpdateTimer_Tick(object sender, EventArgs e)
        {
            if (MyCANCommunicationsManager.ConnectionActive == true)
            {
                this.Text = "CAN Adapter Status [" + MyCANCommunicationsManager.ConnectionStatus + "{" + MyCANCommunicationsManager.Port + "} ]" + " [Bytes In : " + MyCANCommunicationsManager.BytesRead + "]  [Packets In : " + MyCANCommunicationsManager.PacketsIn + "]";
                CANConnectButton.ToolTipText = "Click to disconnect from CAN Interface";
            }
            else
            {
                this.Text = MyCANCommunicationsManager.ConnectionStatus;
                CANConnectButton.ToolTipText = "Click to connect to CAN Interface";
            }
        }

        private void CANConnectButton_Click(object sender, EventArgs e)
        {
            if (MyCANCommunicationsManager.ConnectionActive == false)
            {
                MyCANCommunicationsManager.Connect();
            }
            else
            {

                MyCANCommunicationsManager.Close();
            }
       }

        private void SetExcludeRangeButton_Click(object sender, EventArgs e)
        {
            SetExcludeRange();
        }

        void SetExcludeRange()
        {

            String StringToCheck = ExcludeRangeEntryBox.Text;
            String RegexString = @"(\s*0x\w+\s*):(\s*0x\w+\s*)";

            Match MyMatch = Regex.Match(StringToCheck, RegexString);


            if (MyMatch.Success == true)
            {

             String Range = MyMatch.Captures[0].Value;
             String [] Splits = Range.Split(':');

             if (Splits.Length != 2)
             {
                 MessageBox.Show("Please enter range in the format '0x0000:0xFFFF'");
                 return;
             }

             String RangeLowString  = Splits[0];
             String RangeHighString = Splits[1];

             Splits = RangeLowString.Split('x');

             UInt32 RangeLow;
             UInt32 RangeHigh;

             try
             {
                 RangeLow = UInt32.Parse(Splits[1], System.Globalization.NumberStyles.HexNumber);
             }
             catch
             {
                 RangeLow = 0x100;
             }

             Splits = RangeHighString.Split('x');

             try
             {
                 RangeHigh = UInt32.Parse(Splits[1], System.Globalization.NumberStyles.HexNumber);
             }
             catch
             {
                 RangeHigh = 0x100;
             }

             if (RangeLow > RangeHigh)
             {
                 UInt32 Temp1 = RangeLow;
                 UInt32 Temp2 = RangeHigh;

                 RangeLow = Temp2;
                 RangeHigh = Temp1;
             }

             ExcludeRangeEntryBox.Text = "0x" + RangeLow.ToString("X") + ":" + "0x" + RangeHigh.ToString("X");

             MyCANCommunicationsManager.SetExcludeRange(RangeLow, RangeHigh);
             return; 
                
            }
            else
            {
                MessageBox.Show("Please enter range in the format '0x0000:0xFFFF'");
                return;
            }

         
        }

        private void SetCANTermination_Click(object sender, EventArgs e)
        {
            SetCANTermination();
        }

        void SetCANTermination()
        {
            if (CurrentCANTerminationState == false)
            {
                CurrentCANTerminationState = true;

                MyCANCommunicationsManager.SetCANTerminationState(true);
                SetCANTerminationButton.Text = "CAN Termination Off";
                SetCANTerminationButton.ToolTipText = "Turn off internal CAN Termination";

            }

            else
            {
                CurrentCANTerminationState = false;
                MyCANCommunicationsManager.SetCANTerminationState(false);
                SetCANTerminationButton.Text = "CAN Termination On";
                SetCANTerminationButton.ToolTipText = "Turn on internal CAN Termination";

            }

        }

        private void ResetInterfaceButton_Click(object sender, EventArgs e)
        {
            MyCANCommunicationsManager.ResetCANInterface();
        }


        public void CANMessageRouter()
        {
            while (true)
            {
                Thread.Sleep(1);
                
                                lock (MyCANCommunicationsManager.OutgoingCANMessageQueue)
                                {
                                    while (MyCANMessageEntryForm.TxCANMessageQueue.Count > 0)
                                    {
                                       MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(MyCANMessageEntryForm.TxCANMessageQueue.Dequeue());
                                    }
                                    while (MyNEOSControlPanel.TxCANMessageQueue.Count > 0)
                                    {
                                        MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(MyNEOSControlPanel.TxCANMessageQueue.Dequeue());
                                    }

                                    while (MyNEOSLowLevelCtrl.TxCANMessageQueue.Count > 0)
                                    {
                                        MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(MyNEOSLowLevelCtrl.TxCANMessageQueue.Dequeue());
                                    }
                               }

                                lock (MyCANCommunicationsManager.IncomingCANMessageQueue)
                                {
                                    while (MyCANCommunicationsManager.IncomingCANMessageQueue.Count > 0)
                                    {
                                        CANMessage MasterClone = MyCANCommunicationsManager.IncomingCANMessageQueue.Dequeue();

                                        if (MyMessageRouterControl.SuscriberEnable("RAW CAN Message View") == true)
                                        {
                                            CANMessage Clone1 = CANMessage.Clone(MasterClone);
                                            MyIncomingCANMessageView.RxCANMessageQueue.Enqueue(Clone1);
                                        }
                                        if (MyMessageRouterControl.SuscriberEnable("NEOS All Around Simulator") == true)
                                        {
                                           CANMessage Clone2 = CANMessage.Clone(MasterClone);
                                            MyNEOSControlPanel.RxCANMessageQueue.Enqueue(Clone2);
                                        }

                                        if (MyMessageRouterControl.SuscriberEnable("Low Level NEOS Control") == true)
                                        {
                                            CANMessage Clone3 = CANMessage.Clone(MasterClone);
                                            MyNEOSLowLevelCtrl.RxCANMessageQueue.Enqueue(Clone3);  
                                            lock (MyNEOSLowLevelCtrl.RxCANMessageQueue)
                                            {
                                                MyNEOSLowLevelCtrl.RxCANMessageQueue.Enqueue(MasterClone);
                                            }
                                        }
                                   }
                            }
                }
               
        }

        private void CANSnifferMainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            MessageRouterThread.Abort();
            MyCANCommunicationsManager.Terminate();

            MyNEOSControlPanel.Terminate();
        }

        private void CANSnifferMainForm_Load(object sender, EventArgs e)
        {

        }

    }
}