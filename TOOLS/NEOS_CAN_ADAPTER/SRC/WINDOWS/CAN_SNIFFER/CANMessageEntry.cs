using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using CANInterfaceManagement;

public class CANMessageEntryForm : Form
{
    Size FormSize = new Size(350,54);
    TextBox CANMessageTextBox = new TextBox();

    public CANMessageQueue TxCANMessageQueue = new CANMessageQueue(256);

    CANMessage UserEnteredCANMessage = new CANMessage();

    AutoCompleteSource MyCANMessageHistory = new AutoCompleteSource();
    AutoCompleteStringCollection MyCANStringCollection = new AutoCompleteStringCollection();

    void InitForm()
    {


        this.Size = FormSize;
        this.MaximumSize = FormSize;
        this.MinimumSize = FormSize;

        this.FormClosing += new FormClosingEventHandler(CANMessageEntryForm_FormClosing);
        this.Text = "Outgoing CAN Message Entry";
        this.MaximizeBox = false;

        CANMessageTextBox.Size = new Size(350, 40);
        CANMessageTextBox.Multiline = false;
        CANMessageTextBox.Font = new Font(FontFamily.GenericMonospace, 8.0f);
        CANMessageTextBox.DoubleClick += new EventHandler(CANMessageTextBox_DoubleClick);

        CANMessageTextBox.AutoCompleteMode = AutoCompleteMode.Suggest;
        CANMessageTextBox.AutoCompleteSource = AutoCompleteSource.CustomSource;
        CANMessageTextBox.AutoCompleteCustomSource = MyCANStringCollection;

        

         
        this.Controls.Add(CANMessageTextBox);

    
    }

    void Transmit()
    {

        ValidateInput();
        if (TxCANMessageQueue != null)
        {
            TxCANMessageQueue.Enqueue(UserEnteredCANMessage);
        }
    }

    void CANMessageTextBox_DoubleClick(object sender, EventArgs e)
    {
        
    }

    void CANMessageEntryForm_FormClosing(object sender, FormClosingEventArgs e)
    {
        e.Cancel = true;
    }

    public CANMessageEntryForm()
    {
        InitForm();   

    }

    void ValidateInput()
    {

        String StringToCheck = CANMessageTextBox.Text;
        String RegexString= @"(\s*0x\w+\s*)";

        MatchCollection MyMatches = Regex.Matches(StringToCheck,RegexString);
    
        Match[] EMatch = new Match[MyMatches.Count];

        MyMatches.CopyTo(EMatch, 0);

     
        String Cleanup = "";

        UInt32 [] MessageElements  = new UInt32[MyMatches.Count];

        for (int i = 0; i < EMatch.Length; i++)
        {
            string [] splits = EMatch[i].Value.Split('x');
            
            try
            {
                MessageElements[i] = UInt32.Parse(splits[1], System.Globalization.NumberStyles.HexNumber);
                
            }
            catch
            {
                MessageElements[i] = 0;                
            }
          
        }


        UserEnteredCANMessage.CANId = 0;
        for(int i=0;i<8;i++)
        {
            UserEnteredCANMessage.CANData[i]=0;
        }

        if(MessageElements.Length>0)
        {
            if (MessageElements.Length < 8)
            {
                UserEnteredCANMessage.CANId = MessageElements[0];

                for (int i = 1; i < MessageElements.Length; i++)
                {
                    if (MessageElements[i] > 0xff) 
                        MessageElements[i] = 0xff;

                    UserEnteredCANMessage.CANData[i-1] = (byte)MessageElements[i];
                }
 
            }
            else
            {
                UserEnteredCANMessage.CANId = MessageElements[0];
 
                for(int i=1;i<9;i++)
                {
                    if (MessageElements[i] > 0xff)
                        MessageElements[i] = 0xff;

                    UserEnteredCANMessage.CANData[i-1] = (byte)MessageElements[i];
                }

            }


        }

        Cleanup ="0x" +  UserEnteredCANMessage.CANId.ToString("X4")+" ";
 
        for(int i=0;i<8;i++)
        {
            Cleanup+= "0x" + UserEnteredCANMessage.CANData[i].ToString("X2") +" ";

        }

        MyCANStringCollection.Add(Cleanup);
        CANMessageTextBox.Text = Cleanup;

     
    }

    public void Terminate()
    {

    }
    // This method intercepts the Enter Key
    // signal before the containing Form does
    protected override bool ProcessCmdKey(ref 
              System.Windows.Forms.Message m,
              System.Windows.Forms.Keys k)
    {
        // detect the pushing (Msg) of Enter Key (k)
        if (m.Msg == 256 && k ==
               System.Windows.Forms.Keys.Tab)
        {
            // Execute an alternative action: here we
            // tabulate in order to focus
            // on the next control in the formular
            ValidateInput();
            // return true to stop any further
            // interpretation of this key action
            return true;
        }

        if (m.Msg == 256 && k ==
              System.Windows.Forms.Keys.Enter)
        {
            // Execute an alternative action: here we
            // tabulate in order to focus
            // on the next control in the formular
            Transmit();
            // return true to stop any further
            // interpretation of this key action
            
        }
               
        // if not pushing Enter Key,
        // then process the signal as usual
        return base.ProcessCmdKey(ref m, k);
    }


}
