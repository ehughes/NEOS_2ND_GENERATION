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
using HardwareInterfaceManagement;
using System.Diagnostics;
using UrielGuy.SyntaxHighlightingTextBox;

public class CodeEditor : Form
{
    private SyntaxHighlightingTextBox MyCodeEditorText = new SyntaxHighlightingTextBox();
    private System.Windows.Forms.Timer WindowUpdateTimer = new System.Windows.Forms.Timer();


    public bool Changed = false;
    public bool FileLoaded = false;

    public string FileName = "No File Opened";

    private ToolStripMenuItem OpenMenuItem;
    private ToolStripMenuItem NewMenuItem;
    private ToolStripMenuItem SaveMenuItem;
    private ToolStripMenuItem SaveAsMenuItem;
    private ContextMenuStrip LeftClickMenu;

    
	public CodeEditor()
	{
        InitComponent();
	}

    void InitComponent()
    {
        //Setup the main Form

        this.Size = new Size(600, 300);
        this.Controls.Add(MyCodeEditorText);

        this.ResizeEnd += new EventHandler(CodeEditor_ResizeEnd);
        this.MaximizedBoundsChanged += new EventHandler(CodeEditor_MaximizedBoundsChanged);
        this.SizeChanged += new EventHandler(CodeEditor_SizeChanged);
        this.Text = "Sound Package Editor";
        this.MaximizeBox = false;
        this.FormClosing += new FormClosingEventHandler(CodeEditor_FormClosing);
        this.Font = new Font(FontFamily.GenericMonospace, (float)9.0);
        this.StartPosition = FormStartPosition.CenterParent;
        this.Location = new Point(10, 500);

        WindowUpdateTimer.Interval = 150;
        WindowUpdateTimer.Tick += new EventHandler(WindowUpdateTimer_Tick);
        WindowUpdateTimer.Enabled = true;


        MyCodeEditorText.Font = new Font(FontFamily.GenericMonospace, (float)9.0);
        MyCodeEditorText.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
        MyCodeEditorText.Location = new Point(0, 0);
        MyCodeEditorText.Multiline = true;
        MyCodeEditorText.ScrollBars = RichTextBoxScrollBars.ForcedVertical;
        MyCodeEditorText.Font = new Font(FontFamily.GenericMonospace, (float)9.0);


        MyCodeEditorText.Location = new Point(0, 0);
        MyCodeEditorText.Dock = DockStyle.Fill;
        MyCodeEditorText.Seperators.Add(' ');
        MyCodeEditorText.Seperators.Add('\r');
        MyCodeEditorText.Seperators.Add('\n');
        MyCodeEditorText.Seperators.Add(',');
        MyCodeEditorText.Seperators.Add('.');
        MyCodeEditorText.Seperators.Add('-');
        MyCodeEditorText.Seperators.Add('+');

        MyCodeEditorText.Seperators.Add('[');
        MyCodeEditorText.Seperators.Add(']');
        //MyCodeEditorText.Seperators.Add('*');
        //MyCodeEditorText.Seperators.Add('/');
        Controls.Add(MyCodeEditorText);
        MyCodeEditorText.WordWrap = false;
        MyCodeEditorText.ScrollBars = RichTextBoxScrollBars.Both;// & RichTextBoxScrollBars.ForcedVertical;

        MyCodeEditorText.FilterAutoComplete = false;
        MyCodeEditorText.HighlightDescriptors.Add(new HighlightDescriptor("#define", Color.Green, null, DescriptorType.Word, DescriptorRecognition.WholeWord, true));
        MyCodeEditorText.HighlightDescriptors.Add(new HighlightDescriptor("sound", Color.Red, null, DescriptorType.Word, DescriptorRecognition.WholeWord, true));
        MyCodeEditorText.HighlightDescriptors.Add(new HighlightDescriptor("/*", "*/", Color.Gray, null, DescriptorType.ToCloseToken, DescriptorRecognition.StartsWith, false));
        MyCodeEditorText.HighlightDescriptors.Add(new HighlightDescriptor("[", "]", Color.Magenta, null, DescriptorType.ToCloseToken, DescriptorRecognition.StartsWith, false));
        MyCodeEditorText.HighlightDescriptors.Add(new HighlightDescriptor("//", Color.Gray, null, DescriptorType.ToEOL, DescriptorRecognition.StartsWith, false));

        MyCodeEditorText.KeyPress += new KeyPressEventHandler(MyCodeEditorText_KeyPress);


        LeftClickMenu = new ContextMenuStrip();
       
        OpenMenuItem = new ToolStripMenuItem("Open");
        NewMenuItem = new ToolStripMenuItem("New");
        SaveMenuItem = new ToolStripMenuItem("Save");
        SaveAsMenuItem = new ToolStripMenuItem("Save As ....");


        OpenMenuItem.Click +=new EventHandler(OpenMenuItem_Click);
        NewMenuItem .Click +=new EventHandler(NewMenuItem_Click);
        SaveMenuItem.Click += new EventHandler(SaveMenuItem_Click);
        SaveAsMenuItem.Click += new EventHandler(SaveAsMenuItem_Click);
        LeftClickMenu.Items.AddRange(new ToolStripItem[] { OpenMenuItem, NewMenuItem, SaveMenuItem, SaveAsMenuItem });


        this.MouseDown += new MouseEventHandler(CodeEditor_MouseDown);
        this.MouseClick += new MouseEventHandler(CodeEditor_MouseClick);
        MyCodeEditorText.MouseClick += new MouseEventHandler(MyCodeEditorText_MouseClick);
        MyCodeEditorText.MouseDown += new MouseEventHandler(MyCodeEditorText_MouseDown);
       
        
    }

    void MyCodeEditorText_MouseDown(object sender, MouseEventArgs e)
    {
        if (e.Button == MouseButtons.Right)
        {

            LeftClickMenu.Show((Control)sender, e.Location);
        }
    }

    void CodeEditor_MouseDown(object sender, MouseEventArgs e)
    {
        if (e.Button == MouseButtons.Right)
        {

            LeftClickMenu.Show((Control)sender, e.Location);
        }
    }

    void SaveAsMenuItem_Click(object sender, EventArgs e)
    {
        SaveFileAs();
    }

    public void SaveMenuItem_Click(object sender, EventArgs e)
    {
        SaveFile();
    }

    public void SaveFileAs()
    {
         SaveFileDialog MySave = new SaveFileDialog();

            MySave.CheckFileExists = false;
            MySave.DefaultExt = "";
            MySave.Title = "Select file to save to";
            MySave.FileName = "*.*";
            MySave.Filter = "ASCII Text File (*.*)|*.*";
            MySave.FilterIndex = 0;
            MySave.RestoreDirectory = true;

            if(MySave.ShowDialog() == DialogResult.OK);
            {
                FileName = MySave.FileName;
                try
                {
                    StreamWriter FileOut = new StreamWriter(FileName);
                    FileOut.Write(MyCodeEditorText.Text + "\r\n");
                    FileOut.Close();
                    FileLoaded = true;
                    Changed = false;
                }
                catch (Exception Ex)
                {

                    MessageBox.Show(Ex.Message, "Error saving file.");
                }
            }
    }

    public void SaveFile()
    {
        if (FileLoaded == false)
        {
            SaveFileAs();
        }
        else
        {
            try
            {
                StreamWriter FileOut = new StreamWriter(FileName);
                FileOut.Write(MyCodeEditorText.Text + "\r\n");
                FileOut.Close();
                FileLoaded = true;
                Changed = false;
            }
            catch (Exception Ex)
            {

                MessageBox.Show(Ex.Message, "Error saving file.");
            }

        }

    }

    void MyCodeEditorText_MouseClick(object sender, MouseEventArgs e)
    {
        if (e.Button == MouseButtons.Right)
        {

            LeftClickMenu.Show((Control)sender, e.Location);
        }
    }

    void CodeEditor_MouseClick(object sender, MouseEventArgs e)
    {
        if (e.Button == MouseButtons.Right)
        {

            LeftClickMenu.Show((Control)sender, e.Location);
        }
    }

    void NewMenuItem_Click(object sender, EventArgs e)
    {
        NewFile();
    }

   
    void OpenMenuItem_Click(object sender, EventArgs e)
    {
        OpenFile();

    }

    public void OpenFile()
    {
        PromptToChange();

        OpenFileDialog MyOpen = new OpenFileDialog();

        MyOpen.DefaultExt = "*.*";
        MyOpen.FileName = "*.*";
        MyOpen.Title = "Select file to open.";
        MyOpen.Filter = "ASCII Text File (*.*)|*.*";
        MyOpen.FilterIndex = 0;
        MyOpen.RestoreDirectory = true;
        if (MyOpen.ShowDialog() == DialogResult.OK)


        {
            try
            {
                FileName = MyOpen.FileName;
               
                StreamReader MyRead = new StreamReader(FileName);
                MyCodeEditorText.Text = MyRead.ReadToEnd();
                MyRead.Close();
                FileLoaded = true;
                Changed = false;
            }
            catch (Exception Ex)
            {
                MessageBox.Show(Ex.Message, "Error opening file.");

            }

        }
    }


    void MyCodeEditorText_KeyPress(object sender, KeyPressEventArgs e)
    {
        Changed = true; 
    }

    void PromptToChange()
    {
        if (Changed == true)
        {
            if (MessageBox.Show("Do you wish to save the current code?", "Save current code?", MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                SaveFile();
            }
        }
    }
    void WindowUpdateTimer_Tick(object sender, EventArgs e)
    {
        if (FileLoaded == false)
        {
            if (Changed == false)
            {
                this.Text = "Sound Package Editor : No File Loaded";
            }
            else
            {
                this.Text = "Sound Package Editor : [*] No File Loaded";
            }
        }
        else
        {
            if (Changed == false)
            {
                this.Text = "Sound Package Editor : " + FileName;
            }
            else
            {
                this.Text = "Sound Package Editor : [*] " + FileName;
            }

        }
    }

    void CodeEditor_FormClosing(object sender, FormClosingEventArgs e)
    {
        e.Cancel = true;
    }

    void CodeEditor_SizeChanged(object sender, EventArgs e)
    {
        MyCodeEditorText.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
    }

    void CodeEditor_MaximizedBoundsChanged(object sender, EventArgs e)
    {
        MyCodeEditorText.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
    }

    void CodeEditor_ResizeEnd(object sender, EventArgs e)
    {
        MyCodeEditorText.Size = new Size(this.Size.Width - 8, this.Size.Height - 32);
        
    }


    public void NewFile()
    {
        PromptToChange();

        Changed = false;
        FileLoaded = false;
        FileName = "No File Opened";
        MyCodeEditorText.Text = "";
    }


 
}
