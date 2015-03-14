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
        public NEOS360Simulator MyNEOSAllAroundSimulator = new NEOS360Simulator();
        public NEOSLightRingSimulator MyNEOSLightRingSimulator = new NEOSLightRingSimulator();
        public bool CurrentCANTerminationState;
        public Thread MessageRouterThread;
        public NEOSLowLevelCtrl MyNEOSLowLevelCtrl = new NEOSLowLevelCtrl();
        public NEOSDisplayTestForm  MyNEOSDisplayTest = new NEOSDisplayTestForm();

        CANMessage NextOutgoingMessage = new CANMessage();
        CANMessage NextIncomingMessage = new CANMessage();
        CANMessage NextIncomingCANMessage = new CANMessage();

        public CANSnifferMainForm()
        {
            InitializeComponent();
            MySystemMessageView.MdiParent = this;
            MySystemMessageView.Show();
            MyCANMessageEntryForm.MdiParent = this;
            MyCANMessageEntryForm.Show();
            MyIncomingCANMessageView.MdiParent = this;
            MyIncomingCANMessageView.Show();
            MyNEOSLightRingSimulator.MdiParent = this;

            MyNEOSDisplayTest.MdiParent = this;
            MyNEOSDisplayTest.Show();

            MyNEOSLightRingSimulator.Show();

            MyNEOSAllAroundSimulator.MdiParent = this;
            MyNEOSAllAroundSimulator.Show();
       
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
                if (MyCANCommunicationsManager.IncomingCANMessageQueue.GetCount() > 0)
                {


                    uint temp = MyCANCommunicationsManager.IncomingCANMessageQueue.GetCount();
                    for (int i = 0; i < temp; i++)
                    {
                        lock (MyCANCommunicationsManager.IncomingCANMessageQueue)
                        {
                            MyCANCommunicationsManager.IncomingCANMessageQueue.Dequeue(ref NextIncomingCANMessage);
                        }
                        lock (MyIncomingCANMessageView.RxCANMessageQueue)
                        {
                            MyIncomingCANMessageView.RxCANMessageQueue.Enqueue(NextIncomingCANMessage);
                        }
                        lock (MyNEOSLowLevelCtrl.RxCANMessageQueue)
                        {
                            MyNEOSLowLevelCtrl.RxCANMessageQueue.Enqueue(NextIncomingCANMessage);
                        }
                        lock (MyNEOSAllAroundSimulator.RxCANMessageQueue)
                        {
                            MyNEOSAllAroundSimulator.RxCANMessageQueue.Enqueue(NextIncomingCANMessage);
                        }
                        lock (MyNEOSLightRingSimulator.RxCANMessageQueue)
                        {
                            MyNEOSLightRingSimulator.RxCANMessageQueue.Enqueue(NextIncomingCANMessage);
                        }
                        lock (MyNEOSDisplayTest.RxCANMessageQueue)
                        {
                            MyNEOSDisplayTest.RxCANMessageQueue.Enqueue(NextIncomingCANMessage);
                        }
                    }
                }


                if (MyNEOSDisplayTest.TxCANMessageQueue.GetCount() > 0)
                {
                    uint temp = MyNEOSDisplayTest.TxCANMessageQueue.GetCount();
                    for (int i = 0; i < temp; i++)
                    {
                        MyNEOSDisplayTest.TxCANMessageQueue.Dequeue(ref NextOutgoingMessage);
                        MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(NextOutgoingMessage);
                    }
                }

                if (MyNEOSLightRingSimulator.TxCANMessageQueue.GetCount() > 0)
                {
                    uint temp = MyNEOSLightRingSimulator.TxCANMessageQueue.GetCount();
                    for (int i = 0; i < temp; i++)
                    {
                        MyNEOSLightRingSimulator.TxCANMessageQueue.Dequeue(ref NextOutgoingMessage);
                        MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(NextOutgoingMessage);
                    }
                }


                if (MyCANMessageEntryForm.TxCANMessageQueue.GetCount() > 0)
                {
                    uint temp = MyCANMessageEntryForm.TxCANMessageQueue.GetCount();
                    for (int i = 0; i < temp; i++)
                    {
                        MyCANMessageEntryForm.TxCANMessageQueue.Dequeue(ref NextOutgoingMessage);
                        MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(NextOutgoingMessage);
                    }
                }

                if (MyNEOSAllAroundSimulator.TxCANMessageQueue.GetCount() > 0)
                {
                    uint temp = MyNEOSAllAroundSimulator.TxCANMessageQueue.GetCount();
                    for (int i = 0; i < temp; i++)
                    {
                        MyNEOSAllAroundSimulator.TxCANMessageQueue.Dequeue(ref NextOutgoingMessage);
                        MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(NextOutgoingMessage);
                    }
                }

                if (MyNEOSLowLevelCtrl.TxCANMessageQueue.GetCount() > 0)
                {
                    uint temp = MyNEOSLowLevelCtrl.TxCANMessageQueue.GetCount();
                    for (int i = 0; i < temp; i++)
                    {
                        MyNEOSLowLevelCtrl.TxCANMessageQueue.Dequeue(ref NextOutgoingMessage);
                        MyCANCommunicationsManager.OutgoingCANMessageQueue.Enqueue(NextOutgoingMessage);
                    }
                }
            }
               
        }

        private void CANSnifferMainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            MyCANCommunicationsManager.Terminate();
            MyNEOSAllAroundSimulator.Terminate();
            MyNEOSLightRingSimulator.Terminate();
            MessageRouterThread.Abort();
            MyNEOSLowLevelCtrl.Terminate();
            Application.ExitThread();
            Application.Exit();
        }

        private void CANSnifferMainForm_Load(object sender, EventArgs e)
        {

        }

    }
}