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

namespace Playworld
{

    public partial class AllAroundControlPanel : Form
    {

        public CommunicationsManager MyCommunicationsManager = new CommunicationsManager();
        public CodeEditor MySoundPackageCodeEditor = new CodeEditor();
        public CodeEditor MySounderHeaderViewer = new CodeEditor();
        
        public SystemMessageView MySystemMessageView = new SystemMessageView();



        public const uint SoundModuleFLASHSize = 4194304 * 2;
        public const uint TalkingBobExtMemSize = 65536 * 2;

        public const int WAITING_FOR_SOUND_MODULE = 0;
        public const int FLASH_OPERATION_COMPLETE = 1;
        public const int FLASH_OPERATION_FAILED = 2;
        static volatile int FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;
        public volatile bool FLASHOperationFailed = false;
        public const int FLASHEraseOperationTimeOut = 500;
        public const int FLASHWriteOperationTimeOut = 200;

        public const int MAX_RETRIES_AFTER_TIMEOUT = 5;

        public const int RAMReadOperationTimeOut = 100;
        public const int RAMReadOperationRetries = 4;
        public volatile bool RAMReadComplete = false;

        static bool DownloadingKeyFile = false;

        static int FlashStatusMin = 0;
        static int FlashStatusMax = 0;
        static int FlashStatusCurrent = 0;
        static String BinaryKeyFilename;
        static String FlashStatusString;

        Thread FlashThread;
        Thread SerialDataProcessingThread;

        StreamWriter AccelDataFile;

        bool AccelFileStreamActive = false;
        public int TimeOutCnt = 0;
        public bool AccelStreamStarted = false;


        byte[] PageToFlash = new byte[256];

        public AllAroundControlPanel()
        {
            InitializeComponent();

            MyCommunicationsManager.SetParentReference(this);

            MySystemMessageView.MdiParent = this;
            MySystemMessageView.Show();

            MySoundPackageCodeEditor.MdiParent = this;
            MySoundPackageCodeEditor.Show();

            MainFormTip.SetToolTip(this, "Neos All Around Control Panel");

            //SerialDataProcessingThread = new Thread(new ThreadStart(ProcessIncomingSerialData));
            //SerialDataProcessingThread.Priority = ThreadPriority.Highest;
            //SerialDataProcessingThread.Start();

            DownloadingKeyFile = false;

        }


        public void ProcessPacket()
        {

            switch (IncomingPacket.Payload[0])
            {

                case MessageTypes.UNKNOWN_COMMAND:
                    if (IncomingPacket.Length == 1)
                    {
                    }
                    else
                    {
                    }
                    break;

                case MessageTypes.GET_FIRMWARE_VERSION:

                    if (IncomingPacket.Length == 3)
                    {
                        MessageBox.Show("Sound Module Firmware Version: " + IncomingPacket.Payload[1] + "." + IncomingPacket.Payload[2]);

                    }
                    else
                    {

                        MessageBox.Show("Malformed GET_FIRMWARE_VERSION packet recieved");
                    }

                    break;

             



                case MessageTypes.READ_FLASH_PAGE:
                    if (IncomingPacket.Length == 257)
                    {

                    }
                    else
                    {
                    }

                    break;



                case MessageTypes.FLASH_WRITING_COMPLETE:
                    if (IncomingPacket.Length == 1)
                    {

                        FLASHOperationStatus = FLASH_OPERATION_COMPLETE;

                    }
                    else
                    {
                    }
                    break;

                case MessageTypes.FLASH_WRITING_ERROR:
                    if (IncomingPacket.Length == 1)
                    {

                        FLASHOperationStatus = FLASH_OPERATION_FAILED;
                    }
                    else
                    {
                    }
                    break;


                case MessageTypes.WRITE_FLASH_PAGE:
                    if (IncomingPacket.Length == 1)
                    {
                    }
                    else
                    {
                    }
                    break;



                default:

                    break;



            }

           
        }


        byte[] WriteOutPacket = new byte[266];

        private void GetFirmwareVersion_Click(object sender, EventArgs e)
        {
            Byte[] OutPacket;
            OutPacket = MessageTypes.AssembleMessage(MessageTypes.GET_FIRMWARE_VERSION);
            MyCommunicationsManager.Write(OutPacket);

        }




        public void DownloadBinaryKeyFile()
        {

            FileStream fs = new FileStream(BinaryKeyFilename, FileMode.Open, FileAccess.Read);
            BinaryReader br = new BinaryReader(fs);

            uint BinaryKeyFileLength = (uint)fs.Length;
            uint KeyCheck;
            uint FLASHSectorsToErase;
            uint FLASHPagesToProgram;
            uint LastFlashPageSize;
            int i;
            uint FLASHSectorCounter = 0;
            uint FLASHPageCounter = 0;

            DownloadingKeyFile = true;

            for (i = 0; i < 256; i++)
            {
                PageToFlash[i] = (byte)0;
            }

            if (MyCommunicationsManager.ConnectionActive == false)
            {

                MessageBox.Show("There is no active connection to a NEOS. Press the connect button.", "Error downloading Binary Key File");

                DownloadingKeyFile = false;
                return;
            }


            fs.Position = 0;
            KeyCheck = br.ReadUInt32();

            /*    if (KeyCheck != 0xFF01AAFF)
                    {
                    MessageBox.Show("This File is not a valid Spring Rider key file!");
                    DownloadingKeyFile = false;
                    return ;

                    }*/


            PutNEOSInFLASHMode();
            PutNEOSInFLASHMode();
            PutNEOSInFLASHMode();

            //Wait for unit to take the new mode.
            Thread.Sleep(500);

            fs.Position = 0;

            //First, Figure out the number of sectors (64k blocks)

            FLASHSectorsToErase = (BinaryKeyFileLength >> 16) + 1;

            FlashStatusMin = 0;
            FlashStatusMax = (int)FLASHSectorsToErase;


            FLASHPagesToProgram = BinaryKeyFileLength >> 8;

            if ((BinaryKeyFileLength & 0xFF) > 0)
            {

                FLASHPagesToProgram++; //Make sure to get the Scragglers

                LastFlashPageSize = BinaryKeyFileLength & 0xFF;
            }
            else
            {
                LastFlashPageSize = 0;

            }

            for (i = 0; i < FLASHSectorsToErase; i++)
            {
                int TimeOutRetries = 0;
                GC.Collect();
               FlashEraseRetryEntry:

                FlashStatusString = "Erasing Sector " + (i + 1) + ":" + FLASHSectorsToErase;
                MySystemMessageView.PostMessage("FlashProc", FlashStatusString);


                if (EraseFLASHSector(FLASHSectorCounter) == false)
                {
                    MessageBox.Show("Error sending erase sector command at " + FLASHSectorCounter);
                    MySystemMessageView.PostMessage("FlashProc", "Error sending erase sector command at " + FLASHSectorCounter);

                    DownloadingKeyFile = false;
                    return;
                }
                else
                {

                    TimeOutCnt = 0;
                    FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;

                    while (FLASHOperationStatus == WAITING_FOR_SOUND_MODULE)
                    {
                        FlashStatusCurrent = i;
                        Thread.Sleep(1);
                        TimeOutCnt++;

                        if (TimeOutCnt > FLASHEraseOperationTimeOut)
                        {
                            TimeOutRetries++;

                            if (TimeOutRetries > MAX_RETRIES_AFTER_TIMEOUT)
                            {
                                MySystemMessageView.PostMessage("FlashProc", "Could not erase Flash Sector [" + FLASHSectorCounter + "] after " + MAX_RETRIES_AFTER_TIMEOUT + " retries.  Timeout error.");
                                MessageBox.Show("Could not erase Flash Sector [" + FLASHSectorCounter + "] after " + MAX_RETRIES_AFTER_TIMEOUT + " retries.  Timeout error.");
                                DownloadingKeyFile = false;
                                return;

                            }
                            else
                            {
                                MySystemMessageView.PostMessage("FlashProc", "No response to erase command of sector [" + FLASHSectorCounter + "].  Going to retry....  (" + TimeOutRetries + ")");
                                PutNEOSInFLASHMode();
                                PutNEOSInFLASHMode();
                                PutNEOSInFLASHMode();
                                Thread.Sleep(250);
                                goto FlashEraseRetryEntry;
                            }
                        }

                    }

                    FLASHSectorCounter += 0x10000;
                }
            }




            FLASHPageCounter = 0;
            FlashStatusMin = 0;
            FlashStatusMax = (int)FLASHPagesToProgram;





            for (i = 0; i < FLASHPagesToProgram; i++)
            {
               
                int TimeOutRetries = 0;

                FlashStatusString = "Programming Page " + (i + 1) + ":" + FLASHPagesToProgram;
                MySystemMessageView.PostMessage("FlashProc", FlashStatusString);

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


                FlashStatusCurrent = i;

            TimeOutRetry:

                if (WriteFLASH(PageToFlash, FLASHPageCounter) == false)
                {
                    MessageBox.Show("Error sending page program command at page [" + FLASHPageCounter + "]");
                    MySystemMessageView.PostMessage("FlashProc", "Error sending page program command at page [" + FLASHPageCounter + "]");
                    DownloadingKeyFile = false;
                    return;
                }

                TimeOutCnt = 0;

                FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;

                while (FLASHOperationStatus == WAITING_FOR_SOUND_MODULE)
                {

                    
                    Thread.Sleep(1);

                    TimeOutCnt++;

                    if (TimeOutCnt > FLASHWriteOperationTimeOut)
                    {
                        TimeOutRetries++;
                        if (TimeOutRetries > MAX_RETRIES_AFTER_TIMEOUT)
                        {
                            MessageBox.Show("FlashProc", "Could not program page [" + FLASHPageCounter + "] after " + MAX_RETRIES_AFTER_TIMEOUT + " retries.  Timeout error.");
                            MySystemMessageView.PostMessage("FlashProc", "Could not program page [" + FLASHPageCounter + "] after " + MAX_RETRIES_AFTER_TIMEOUT + " retries.  Timeout error.");

                            DownloadingKeyFile = false;
                            return;

                        }
                        else
                        {
                            MySystemMessageView.PostMessage("FlashProc", "No response to program command at sector [" + FLASHPageCounter + "].  Going to retry....  (" + TimeOutRetries + ")");
                            PutNEOSInFLASHMode();
                            PutNEOSInFLASHMode();
                            PutNEOSInFLASHMode();
                            Thread.Sleep(250);
                            goto TimeOutRetry;
                        }
                    }

                }

                if (FLASHOperationStatus == FLASH_OPERATION_FAILED)
                {
                    MessageBox.Show("Flash program/verify error at address " + FLASHPageCounter);
                    MySystemMessageView.PostMessage("FlashProc", "Flash program/verify error at address " + FLASHPageCounter);
                    DownloadingKeyFile = false;
                    return;
                }

                FLASHPageCounter += 0x100;
            }

            FlashStatusCurrent = 0;
            PutNEOSInGAMEMode();

            MessageBox.Show(FLASHPagesToProgram + " pages (" + FLASHPagesToProgram * 256 + "  bytes) successfully programmed", "Programming Complete");
            MySystemMessageView.PostMessage("FlashProc", FLASHPagesToProgram + " pages (" + FLASHPagesToProgram * 256 + "  bytes) successfully programmed");


            br.Close();
            fs.Close();

            DownloadingKeyFile = false;

        }

        public bool WriteFLASH(byte[] Page, uint PageAddressOut)
        {
            
            uint PageAddress;

            PageAddress = PageAddressOut;

            if (MyCommunicationsManager.ConnectionActive)
            {

                if (Page.Length != 256)
                {
                    return false;
                }
                else
                {

                    ushort MyCRC;

                 
                
                        PageAddress &= 0xFFFFFF00;

                        WriteOutPacket[0] = 0xAA;
                        WriteOutPacket[1] = 0xAA;
                        WriteOutPacket[2] = 0x04;
                        WriteOutPacket[3] = 0x01;
                        WriteOutPacket[4] = MessageTypes.WRITE_FLASH_PAGE;
                        WriteOutPacket[5] = (byte)(PageAddress >> 8);
                        WriteOutPacket[6] = (byte)(PageAddress >> 16);
                        WriteOutPacket[7] = (byte)(PageAddress >> 24);


                        int i;
                        for (i = 0; i < 256; i++)
                        {
                            WriteOutPacket[i + 8] = Page[i];
                        }

                        MyCRC = CRC.Calculate(WriteOutPacket, 256 + 8);

                        WriteOutPacket[256 + 8] = (Byte)(MyCRC & 0xff);
                        WriteOutPacket[256 + 9] = (Byte)(MyCRC >> 8);


                        MyCommunicationsManager.Write(WriteOutPacket);
                        FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;
                        return true;
                  

                }
            }
            else
            {
                return false;
            }


        }

        public bool EraseFLASHSector(uint SectorAddressOut)
        {
            if (MyCommunicationsManager.ConnectionActive)
            {
                Byte[] PacketOut = new Byte[9];
                ushort MyCRC;
                uint SectorAddress;


                SectorAddress = SectorAddressOut;

                SectorAddress &= 0xFFFF0000;

                PacketOut[0] = 0xAA;
                PacketOut[1] = 0xAA;
                PacketOut[2] = 0x03;
                PacketOut[3] = 0x00;
                PacketOut[4] = MessageTypes.SECTOR_ERASE;
                PacketOut[5] = (byte)(SectorAddress >> 16);
                PacketOut[6] = (byte)(SectorAddress >> 24);
                MyCRC = CRC.Calculate(PacketOut, 7);

                PacketOut[7] = (Byte)(MyCRC & 0xff);
                PacketOut[8] = (Byte)(MyCRC >> 8);


                MyCommunicationsManager.Write(PacketOut);
                FLASHOperationStatus = WAITING_FOR_SOUND_MODULE;
                return true;
            }
            else
            {

                return false;

            }


        }

     

        private void resetSoundModuleToolStripMenuItem_Click(object sender, EventArgs e)
        {

            if (MyCommunicationsManager.ConnectionActive == true)
            {
                Byte[] OutPacket;
                OutPacket = MessageTypes.AssembleMessage(MessageTypes.SOFTWARE_RESET);
                MyCommunicationsManager.Write(OutPacket);
            }
            else
            {
                MessageBox.Show("There is no active connection to a Talking Bob Module. Press the Connect button.", "Error!");
            }

        }


        private void ProcessIncomingSerialData()
        {

            int i;
            while (true)
            {
                Thread.Sleep(10);
             /*   if (MyCommunicationsManager.ConnectionActive == true)
                {
                    if (MyCommunicationsManager.MyPacketDetector.PacketQueue.Count > 0)
                    {

                        lock (MyCommunicationsManager.MyPacketDetector.PacketQueue)
                        {
                            for (i = 0; i < MyCommunicationsManager.MyPacketDetector.PacketQueue.Count; i++)
                            {
                                byte[] NewPacket = MyCommunicationsManager.MyPacketDetector.PacketQueue.Dequeue();
                                lock (NewPacket)
                                {
                                    ProcessPacket(NewPacket);
                                }
                                NewPacket = null;
                                GC.Collect();
                            }
                        }
                    }
                }*/
            }
        }

        private void WindowUpdateManager_Tick(object sender, EventArgs e)
        {

            if (MyCommunicationsManager.ConnectionActive == true)
            {
                ConnectButton.ToolTipText = "Click to disconnect from USB port";


            }
            else
            {
                ConnectButton.ToolTipText = "Click to connect to USB port";
            }



            if (DownloadingKeyFile == true)
            {
                FlashProgressBar.Minimum = FlashStatusMin;
                FlashProgressBar.Maximum = FlashStatusMax;
                FlashProgressBar.Value = FlashStatusCurrent;


                this.Text = MyCommunicationsManager.ConnectionStatus + " -> " + FlashStatusString;


            }
            else
            {
                FlashProgressBar.Value = 0;
                this.Text = MyCommunicationsManager.ConnectionStatus + " Bytes In: " + MyCommunicationsManager.BytesRead + "  Packets In:" + MyCommunicationsManager.MyPacketDetector.PacketsIn;

            }
        }


        private void TBControlPanel_Load(object sender, EventArgs e)
        {

        }

        private void TBControlPanel_FormClosing(object sender, FormClosingEventArgs e)
        {


        }




        private void KeyFileDownloadToolButton_Click(object sender, EventArgs e)
        {

            if (DownloadingKeyFile == false)
            {
                
                OpenFileDialog MyOpenFile = new OpenFileDialog();

                MyOpenFile.Filter = "Binary Profile files (*.bin)|*.bin|All files (*.*)|*.*";
                MyOpenFile.FilterIndex = 0;
                MyOpenFile.RestoreDirectory = true;

                if (MyOpenFile.ShowDialog() == DialogResult.OK)
                {
                    BinaryKeyFilename = MyOpenFile.FileName;
                    try
                    {

                        FlashThread = new Thread(new ThreadStart(DownloadBinaryKeyFile));
                        FlashThread.Start();

                    }
                    catch
                    {
                        DownloadingKeyFile = false;
                    }

                }
                DownloadingKeyFile = false;
            }



        }





        private void GetFirmwareToolButton_Click(object sender, EventArgs e)
        {
            if (MyCommunicationsManager.ConnectionActive)
            {
                Byte[] OutPacket;
                OutPacket = MessageTypes.AssembleMessage(MessageTypes.GET_FIRMWARE_VERSION);
                MyCommunicationsManager.Write(OutPacket);
            }
            else
            {
                MessageBox.Show("There is no active connection to a Talking Bob Module. Press the Connect button.", "Error!");

            }
        }





        private void ConnectionManangerToolTip_Popup(object sender, PopupEventArgs e)
        {

        }

        private void ConnectToTalkingBob_Click(object sender, EventArgs e)
        {



            if (MyCommunicationsManager.ConnectionActive == false)
            {
                MyCommunicationsManager.ConnectToTalkingBob();
            }

            else
            {
                MyCommunicationsManager.Close();
            }

        }



        private void MSPResetToolButton_Click(object sender, EventArgs e)
        {

            if (MyCommunicationsManager.ConnectionActive == true)
            {
                Byte[] OutPacket;
                OutPacket = MessageTypes.AssembleMessage(MessageTypes.SOFTWARE_RESET);
                MyCommunicationsManager.Write(OutPacket);
            }
        }

      
        private void AllAroundControlPanel_FormClosing(object sender, FormClosingEventArgs e)
        {
            MyCommunicationsManager.Terminate();
        //    SerialDataProcessingThread.Abort();
            Application.ExitThread();
            Application.Exit();
        }




        void PutNEOSInFLASHMode()
        {
            MessageTypes.NewSystemMode = SystemModes.FLASH_UPDATE;
            byte[] OutPacket = MessageTypes.AssembleMessage(MessageTypes.SWITCH_SYSTEM_MODE);
            MyCommunicationsManager.Write(OutPacket);
        }

        void PutNEOSInGAMEMode()
        {
            MessageTypes.NewSystemMode = SystemModes.GAME_ACTIVE;
            byte[] OutPacket = MessageTypes.AssembleMessage(MessageTypes.SWITCH_SYSTEM_MODE);
            MyCommunicationsManager.Write(OutPacket);
        }

        private void openSoundPackagerFilerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MySoundPackageCodeEditor.OpenFile();
        }

        private void saveSoundPackagerFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MySoundPackageCodeEditor.SaveFile();
        }

        private void saveSoundPackagerFileAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MySoundPackageCodeEditor.SaveFileAs();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MySoundPackageCodeEditor.NewFile();
        }

        private void generateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RunSoundPackager();
        }

        private void RunSoundPackager()
        {
            if ((MySoundPackageCodeEditor.FileLoaded == false))
            {
                MessageBox.Show("Please save you packager source code.", "File not saved");
                MySoundPackageCodeEditor.SaveFile();

            }

            if (MySoundPackageCodeEditor.Changed == true)
            {
                MySoundPackageCodeEditor.SaveFile();
            }


            MySoundPackageCodeEditor.FileName  = MySoundPackageCodeEditor.FileName.Trim();

         

          /*  if (MySoundPackageCodeEditor.FileName.IndexOf(" ") != -1)
            {
                MessageBox.Show("The path to the sound package file contains spaces.  Please fix path and/or file name", "Spaces in path.");
                MySystemMessageView.PostMessage("npackProc", "The path to the sound package file contains spaces.");
                return;
            }*/
            
            try
            {
                Process npackProcess = new Process();

                npackProcess.StartInfo.FileName = "npack";
                npackProcess.StartInfo.Arguments = MySoundPackageCodeEditor.FileName; 
                npackProcess.StartInfo.UseShellExecute = false;
                npackProcess.StartInfo.RedirectStandardOutput = true;
                npackProcess.StartInfo.CreateNoWindow = true;
                npackProcess.OutputDataReceived += new DataReceivedEventHandler(npackProcess_OutputDataReceived);
                                             
                npackProcess.Start();
                npackProcess.BeginOutputReadLine();


                npackProcess.WaitForExit(10);
                
                               
            }
            catch (Exception Ex)
            {
                MessageBox.Show(Ex.Message, "Error running sound packager.");
            }



        }

        void npackProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if(!String.IsNullOrEmpty(e.Data))
            {
                MySystemMessageView.PostMessage("npackProc", e.Data);
            }
        }

        private void AllAroundControlPanel_FormClosed(object sender, FormClosedEventArgs e)
        {

           
        }

        private void SoundPlayButton_Click(object sender, EventArgs e)
        {
            UInt16 Soundnum;
            UInt16 SoundLen;
            Byte SoundStream;

            try
            {
                Soundnum = Convert.ToUInt16(SoundNumberTextBox.Text);
            }
            catch
            {
                Soundnum = 0;
                SoundNumberTextBox.Text = "0";
            }

            try
            {
                SoundLen = Convert.ToUInt16(SoundLengthTextBox.Text);
            }
            catch
            {
                SoundLen = 0;
                SoundLengthTextBox.Text = "0";
                
            }

            try
            {
                SoundStream = Convert.ToByte(SoundStreamTextBox.Text);
            }
            catch
            {
                SoundStream = 0;
                SoundStreamTextBox.Text = "3";
            }

            if (SoundStream > 3)
            {
                SoundStream = 3;
                SoundStreamTextBox.Text = "3";
            }

            MessageTypes.SoundLength=SoundLen;
            MessageTypes.SoundStream = SoundStream;
            MessageTypes.SoundToPlay = Soundnum;

            byte[] OutPacket = MessageTypes.AssembleMessage(MessageTypes.PLAY_SOUND);
            MyCommunicationsManager.Write(OutPacket);
        }


    }
}