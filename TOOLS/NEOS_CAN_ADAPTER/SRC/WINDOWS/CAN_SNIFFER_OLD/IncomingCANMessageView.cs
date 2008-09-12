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
using System.Diagnostics;
using CANInterfaceManagement;

public class IncomingCANMessageView : Form
{
    
    private RichTextBox MessageViewOutput = new RichTextBox();
    private System.Windows.Forms.Timer UpdateTimer = new System.Windows.Forms.Timer();

    private const int MESSAGE_HISTORY = 64;
    string[] MessageBuffer = new string[MESSAGE_HISTORY];
    int MessagePtr = 0;
   

    public Queue<CANMessage> RxCANMessageQueue = new Queue<CANMessage>();


    public IncomingCANMessageView()
    {
        InitComponent();
        UpdateTimer.Interval = 100;
        UpdateTimer.Enabled = true; ;
        UpdateTimer.Tick += new EventHandler(UpdateTimer_Tick);

    }

    void UpdateTimer_Tick(object sender, EventArgs e)
    {
        string[] Sort = new String[MESSAGE_HISTORY];
        int MsgPtrTemp = MessagePtr;

        lock (RxCANMessageQueue)
        {
            if (RxCANMessageQueue != null)
            {
                for (int q = 0; q < RxCANMessageQueue.Count;q++)
                {
                    CANMessage NextCANMessage = RxCANMessageQueue.Dequeue();

                    if (NextCANMessage != null)
                    {

                        MessageBuffer[MessagePtr] = "0x" + NextCANMessage.CANId.ToString("X4") + " " +
                                                    "0x" + NextCANMessage.CANData[0].ToString("X2") + " " +
                                                    "0x" + NextCANMessage.CANData[1].ToString("X2") + " " +
                                                    "0x" + NextCANMessage.CANData[2].ToString("X2") + " " +
                                                    "0x" + NextCANMessage.CANData[3].ToString("X2") + " " +
                                                    "0x" + NextCANMessage.CANData[4].ToString("X2") + " " +
                                                    "0x" + NextCANMessage.CANData[5].ToString("X2") + " " +
                                                    "0x" + NextCANMessage.CANData[6].ToString("X2") + " " +
                                                    "0x" + NextCANMessage.CANData[7].ToString("X2");


                        MessagePtr++;
                        if (MessagePtr > MESSAGE_HISTORY - 1)
                        {
                            MessagePtr = 0;
                        }

                        for (int i = 0; i < MESSAGE_HISTORY; i++)
                        {

                            if (MsgPtrTemp == 0)
                            {
                                MsgPtrTemp = MESSAGE_HISTORY - 1;
                            }
                            else
                            {
                                MsgPtrTemp--;
                            }
                            Sort[i] = MessageBuffer[MsgPtrTemp];
                        }

                        this.MessageViewOutput.Lines = Sort;
                    }
                }
            }
        }
    }

    
   

    void InitComponent()
    {
        //Setup the main Form

        this.Size = new Size(600, 300);
        this.Controls.Add(MessageViewOutput);
        this.ResizeEnd += new EventHandler(SystemMessageView_ResizeEnd);
        this.MaximizedBoundsChanged += new EventHandler(SystemMessageView_MaximizedBoundsChanged);
        this.SizeChanged += new EventHandler(SystemMessageView_SizeChanged);
        this.Text = "Incoming CAN Message View";
        this.MaximizeBox = false;
        this.FormClosing += new FormClosingEventHandler(SystemMessageView_FormClosing);
        this.Font = new Font(FontFamily.GenericMonospace, (float)9.0);
        this.StartPosition = FormStartPosition.CenterParent;
        this.Location = new Point(10, 500);

        MessageViewOutput.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
        MessageViewOutput.Location = new Point(0, 0);
        MessageViewOutput.Multiline = true;
        MessageViewOutput.ScrollBars = RichTextBoxScrollBars.ForcedVertical;
        MessageViewOutput.Font = new Font(FontFamily.GenericMonospace, (float)8.0);

    }

    void SystemMessageView_FormClosing(object sender, FormClosingEventArgs e)
    {
        e.Cancel = true;
    }

    void SystemMessageView_SizeChanged(object sender, EventArgs e)
    {
        MessageViewOutput.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
    }

    void SystemMessageView_MaximizedBoundsChanged(object sender, EventArgs e)
    {

        MessageViewOutput.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
    }

    void SystemMessageView_ResizeEnd(object sender, EventArgs e)
    {
        MessageViewOutput.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
    }
}
