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

public class SystemMessageView : Form
{

    public bool PostMessagesOnTop = true;
    private RichTextBox MessageViewOutput = new RichTextBox();
    private System.Windows.Forms.Timer UpdateTimer = new System.Windows.Forms.Timer();
   
    private const int MESSAGE_HISTORY = 64;
    string[] MessageBuffer = new string[MESSAGE_HISTORY];
    int MessagePtr = 0;
    bool FlagForUpdate = false;


    public SystemMessageView()
	{
        InitComponent();
        UpdateTimer.Interval = 250;
        UpdateTimer.Enabled = true; ;
        UpdateTimer.Tick += new EventHandler(UpdateTimer_Tick);

        for (int i = 0; i < MESSAGE_HISTORY; i++)
        {
            MessageBuffer[0] = "";
        }
	}

    void UpdateTimer_Tick(object sender, EventArgs e)
    {
        if (FlagForUpdate == true)
        {

            string[] Sort = new String[MESSAGE_HISTORY];
            int MsgPtrTemp =MessagePtr;

            for (int i = 0; i < MESSAGE_HISTORY; i++)
            {

                if (MsgPtrTemp == 0)
                {
                    MsgPtrTemp = MESSAGE_HISTORY-1;
                }
                else
                {
                    MsgPtrTemp--;
                }
                Sort[i] = MessageBuffer[MsgPtrTemp];
            }

            this.MessageViewOutput.Lines = Sort;
            FlagForUpdate = false;
        }
    }

    void UpdateMessages()
    {
        
    }

    public void PostMessage(String MessageClass, String Message)
        {
               MessageBuffer[MessagePtr] = "[" + MessageClass + "]: " + Message;
               MessagePtr++;
               if (MessagePtr > MESSAGE_HISTORY-1)
               {
                   MessagePtr = 0;
               }
               FlagForUpdate = true;    
        }
    
    void InitComponent()
    {
        //Setup the main Form

        this.Size = new Size(600, 100);
        this.Controls.Add(MessageViewOutput);
        this.ResizeEnd += new EventHandler(SystemMessageView_ResizeEnd);
        this.MaximizedBoundsChanged += new EventHandler(SystemMessageView_MaximizedBoundsChanged);
        this.SizeChanged += new EventHandler(SystemMessageView_SizeChanged);
        this.Text = "System Message View";
        this.MaximizeBox = false;
        this.FormClosing += new FormClosingEventHandler(SystemMessageView_FormClosing);
        this.Font = new Font(FontFamily.GenericMonospace, (float)9.0);
        this.StartPosition = FormStartPosition.CenterParent;
        this.Location = new Point(10, 500);

        
        MessageViewOutput.Size = new Size(this.Size.Width - 8, this.Size.Height -32);
        MessageViewOutput.Location = new Point(0,0);
        MessageViewOutput.Multiline = true;
        MessageViewOutput.ScrollBars = RichTextBoxScrollBars.ForcedVertical;
        MessageViewOutput.Font = new Font(FontFamily.GenericMonospace, (float)9.0); 
        
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
