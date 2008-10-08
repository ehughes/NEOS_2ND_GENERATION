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
using HardwareInterfaceManagement;
using NEOSUSBMessages;
using System.Drawing.Printing;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace NEOS_PRODUCTION_PROGRAMMER
{
    public partial class NeosProductionProgrammerMainForm : Form
    {
     
        string ConfigPath;
        string ProfilesPath;

        String BackgroundMessageData = "";
        Thread FlashThread;
        Thread SerialDataProcessingThread;
        Thread ProgramNeosThread;

        String[] NEOSProfiles;

        Process FirmwareProcess;
        Process SoundUpdateProcess;

        String FirmwareFileName;
        String FirmwareVersion;
        String CurrentProfile;
        String PathToCurrentProfile;

        public OptionsMenu MyOptionsMenu = new OptionsMenu();

        PrintDocument MyPrintOutput = new PrintDocument();
        PrintPreviewDialog MyPrintPreview = new PrintPreviewDialog();
        PrintDialog MyPrintDialog = new PrintDialog();
       
        int ProcessTimingCounts = 0;
     

        #region GUI

        public NeosProductionProgrammerMainForm()
        {
            ConfigPath = ".\\config";
            ProfilesPath = ConfigPath + "\\profiles";
            InitializeComponent();
            RefreshProfiles(ProfilesPath);
            ProfileSelectionCB.SelectedIndex = 0;
            
            MyOptionsMenu.StartPosition = FormStartPosition.WindowsDefaultLocation;
            LoadConfig();
            LoadPrinterSettings();
            MyPrintOutput.PrintPage += new PrintPageEventHandler(MyPrintOutput_PrintPage);
            MyPrintPreview.Document = MyPrintOutput;

          //  MyCommunicationsManager.MyPacketDetector.MyPacketProccessor += new PacketProcessor(ProcessIncomingPacket);

        }

        private void FormupdateTimer_Tick(object sender, EventArgs e)
        {
            StatusTextBox.Text = BackgroundMessageData;
            ProcessTimingCounts++;
        }

        private void ProfileSelectionCB_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void configurationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyOptionsMenu.StartPosition = FormStartPosition.Manual;
            MyOptionsMenu.Location = new Point(this.Location.X + 50, this.Location.Y + 50);
            MyOptionsMenu.ShowDialog();
            SaveConfig();
        }

        void SaveConfig()
        {

            if (File.Exists("ProgramConfig") == false)
            {
                File.Create("ProgramConfig");
            }

            StreamWriter MyWriter = new StreamWriter("ProgramConfig", false);

            if (MyOptionsMenu.PrintPreviewCB.Checked == true)
            {
                MyWriter.WriteLine("PrintPreview=true");
            }
            else
            {
                MyWriter.WriteLine("PrintPreview=false");
            }

            if (MyOptionsMenu.PrintLabelCB.Checked == true)
            {
                MyWriter.WriteLine("PrintLabel=true");
            }
            else
            {
                MyWriter.WriteLine("PrintLabel=false");
            }

            if (MyOptionsMenu.DownloadSoundDataCB.Checked == true)
            {
                MyWriter.WriteLine("DownloadSounds=true");
            }
            else
            {
                MyWriter.WriteLine("DownloadSounds=false");
            }
            if (MyOptionsMenu.DownloadFirmwareCB.Checked == true)
            {
                MyWriter.WriteLine("UpdateFirmware=true");
            }
            else
            {
                MyWriter.WriteLine("UpdateFirmware=false");
            }
            
            MyWriter.Close();

        }

        void LoadConfig()
        {
            if (File.Exists("ProgramConfig") == false)
            {

                MyOptionsMenu.PrintLabelCB.Checked = true;
                MyOptionsMenu.PrintPreviewCB.Checked = true;

                MyOptionsMenu.DownloadFirmwareCB.Checked = true;
                MyOptionsMenu.DownloadSoundDataCB.Checked = true;

            
                return;
            }


            StreamReader MyReader = new StreamReader("ProgramConfig");
            String NextLine = "";

            while ((NextLine = MyReader.ReadLine()) != null)
            {
                string[] Splits = NextLine.Split('=');

                if (Splits.Length == 2)
                {

                    bool Setting;
                    string Temp = Splits[1].ToLower();

                    if (Temp.Trim() == "true")
                    {
                        Setting = true;
                    }
                    else
                    {
                        Setting = false;
                    }

                    switch (Splits[0].Trim())
                    {
                        case "UpdateFirmware":
                            MyOptionsMenu.DownloadFirmwareCB.Checked = Setting;
                            break;

                        case "PrintLabel":
                           MyOptionsMenu.PrintLabelCB.Checked = Setting;
                            break;

                        case "DownloadSounds":
                            MyOptionsMenu.DownloadSoundDataCB.Checked = Setting;
                            break;

                        case "PrintPreview":
                            MyOptionsMenu.PrintPreviewCB.Checked = Setting;
                            break;

                    }

                }

            }

            MyReader.Close();
            return;
        }


        private void NeosProductionProgrammerMainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
           // MyCommunicationsManager.Terminate();
            SaveConfig();
            Application.Exit();
        }



        private void configurationToolStripMenuItem_Click_1(object sender, EventArgs e)
        {

            MyOptionsMenu.StartPosition = FormStartPosition.Manual;
            MyOptionsMenu.Location = new Point(this.Location.X + 30, this.Location.Y + 30);
            MyOptionsMenu.ShowDialog();
            SaveConfig();
        }

        private void printerSetupToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            ShowPrintSetup();
        }

        private void previewLabelToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            GetProfileInfo();
            MyPrintPreview.ShowDialog();
        }

        private void LoadPrinterSettings()
        {
            Stream MyStream;
            BinaryFormatter MyBinaryFormatter;

            if (File.Exists("PrintSettings"))
            {
                try
                {
                    MyStream = File.Open("PrintSettings", FileMode.Open);
                    MyBinaryFormatter = new BinaryFormatter();
                    MyPrintOutput.PrinterSettings = (PrinterSettings)MyBinaryFormatter.Deserialize(MyStream);
                    MyPrintDialog.PrinterSettings = MyPrintOutput.PrinterSettings;
                    MyStream.Close();
                }
                catch
                {

                    ShowPrintSetup();
                }
            }
            else
            {

            }

        }

        private void NeosProductionProgrammerMainForm_Load(object sender, EventArgs e)
        {

        }
        void RefreshProfiles(string ConfigPath)
        {

            if (Directory.Exists(ConfigPath) == false)
            {
                MessageBox.Show("Configuration Directory not found!", "Critical Error!");
            }
            else
            {
                NEOSProfiles = Directory.GetDirectories(ConfigPath);

                ProfileSelectionCB.Items.Clear();

                if (NEOSProfiles.Length > 0)
                {

                    for (int i = 0; i < NEOSProfiles.Length; i++)
                    {
                        NEOSProfiles[i] = NEOSProfiles[i].Replace(ProfilesPath + "\\", "");
                        ProfileSelectionCB.Items.Add(NEOSProfiles[i]);

                    }
                    if (ProfileSelectionCB.Items.Count > 0)
                    {
                        ProfileSelectionCB.SelectedIndex = 0;
                    }
                }
            }
        }



        void GetProfileInfo()
        {
            CurrentProfile = ProfileSelectionCB.Items[ProfileSelectionCB.SelectedIndex].ToString();

            PathToCurrentProfile = ProfilesPath + "\\" + CurrentProfile;

            CheckFirmware(CurrentProfile);

        }

        private void ProgramButton_Click(object sender, EventArgs e)
        {

            ProcessTimingCounts = 0;

            CurrentProfile = ProfileSelectionCB.Items[ProfileSelectionCB.SelectedIndex].ToString();

            PathToCurrentProfile = ProfilesPath + "\\" + CurrentProfile;

            if (CheckFirmware(CurrentProfile) == false)
            {
                return;
            }

            ProgramNeosThread = new Thread(new ThreadStart(ProgramNEOS));
            ProgramNeosThread.Priority = ThreadPriority.Highest;
            ProgramNeosThread.Start();
        }

        #endregion

        #region ModuleProgramming

        void ProgramNEOS()
        {
            #region FirmwareUpdate
            if (MyOptionsMenu.DownloadFirmwareCB.Checked == true)
            {
                if (DownloadFirmware() == false)
                {
                    return;
                }
            }
            #endregion

            Thread.Sleep(1000);
            #region SoundDataUpdate

            if (MyOptionsMenu.DownloadSoundDataCB.Checked == true)
            {


                String FlashImageFile = ProfilesPath + "\\" + CurrentProfile + "\\" + "FlashImage.bin";
                if (File.Exists(FlashImageFile) == true)
                {
                    if (DownloadSounds() == false)
                    {
                        return;
                    }
                }

            }



            int Minutes = (int)(ProcessTimingCounts / 10.0) / 60;
            int Seconds = (int)(ProcessTimingCounts / 10.0) % 60;


            BackgroundMessageData = "Programming Complete!  " + Minutes + " Minutes " + Seconds + " Seconds.";
            #endregion


            #region PrintLabel

            if (MyOptionsMenu.PrintLabelCB.Checked == true)
            {
                if (MyOptionsMenu.PrintPreviewCB.Checked == true)
                {

                    MyPrintPreview.ShowDialog();
                }
                else
                {
                    MyPrintOutput.Print();
                }

            }

            #endregion

        }

        #endregion

        #region Firmware
        bool CheckFirmware(string ProfileName)
        {
            bool BSLFound = false;
            int NumFirmwareFiles = 0;
            string LastRevisionFound = "";
            string LastFirmwareFileName = "";



            string[] Files = Directory.GetFiles(ProfilesPath + "\\" + ProfileName);

            
            for (int i = 0; i < Files.Length; i++)
            {
                Files[i] = Files[i].Replace(ProfilesPath + "\\" + ProfileName + "\\", "");
            }

            //Make sure there are files present
            if (Files.Length == 0 || Files == null)
            {
                MessageBox.Show("No files found in profile directory.", "Error!");
                return false;
            }
                      
            
            for (int i = 0; i < Files.Length; i++)
            {
                String[] Splits = Files[i].Split('.');

                if (Splits.Length == 3 && Splits[0] == ProfileName)
                {
                    NumFirmwareFiles++;
                    LastFirmwareFileName = Files[i];
                    LastRevisionFound = Splits[1] + "." + Splits[2];
                }
            }


            if (NumFirmwareFiles == 0)
            {
                MessageBox.Show("No valid firmware files found.   File name must be in the format " + ProfileName +".XXX.XXX where XXX.XXX is the version code", "Error!");
                return false;
            }

            if (NumFirmwareFiles > 1)
            {
                MessageBox.Show("More than one firmware file found.   Only one firmware file allowed in the profile directory.", "Error!");
                return false;
            }

            FirmwareFileName = LastFirmwareFileName;
            FirmwareVersion = LastRevisionFound;

            return true;
        }


        bool CheckForProgrammer()
        {
            if (File.Exists(ConfigPath + "\\programmer\\PK2CMD.exe") == false)
            {

                MessageBox.Show("Could not find the programming executbale" + ConfigPath + "\\programmer\\PK2CMD.exe", "Error");
                return false;
            }
            else
            {
                return true;
            }

        }

        private void BSL()
        {
            FirmwareProcess = new Process();
            
            FirmwareProcess.StartInfo.FileName = "config\\programmer\\PK2CMD";
            FirmwareProcess.StartInfo.Arguments = "/PdsPIC30F6014A /F" + ProfilesPath + "\\" + CurrentProfile + "\\" + FirmwareFileName + " /M /Y /R";
            FirmwareProcess.StartInfo.UseShellExecute = false;
            FirmwareProcess.StartInfo.RedirectStandardOutput = true;
            FirmwareProcess.StartInfo.CreateNoWindow = true;
            FirmwareProcess.OutputDataReceived += new DataReceivedEventHandler(FirmwareProcess_OutputDataReceived);

            FirmwareProcess.Start();
            FirmwareProcess.BeginOutputReadLine();

        }

        void FirmwareProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (!String.IsNullOrEmpty(e.Data))
            {
                BackgroundMessageData = e.Data;
            }
        }


        bool DownloadFirmware()
        {
            byte Animation = 0;
            int counts = 0;

            BSL();

            while (FirmwareProcess.HasExited == false)
            {
                Thread.Sleep(250);

             
                counts++;
                switch (Animation)
                {
                    case 0:
                        BackgroundMessageData = "Programming " + FirmwareFileName + " .";
                        Animation++;
                        break;

                    case 1:
                        BackgroundMessageData = "Programming " + FirmwareFileName + " ..";
                        Animation++;
                        break;

                    case 2:
                        BackgroundMessageData = "Programming " + FirmwareFileName + " ...";
                        Animation++;
                        break;

                    case 3:
                        BackgroundMessageData = "Programming " + FirmwareFileName + " ..";
                        Animation = 0;
                        break;

                    default:
                        Animation = 0;
                        break;
                }

            }

            if (FirmwareProcess.ExitCode != 0)
            {
                MessageBox.Show(FirmwareFileName + " Download Failed! Make sure programming cable is attached and try again");
                BackgroundMessageData = "";
                return false;
            }

            return true;

        }


        #endregion 
    
        #region SoundDownload

        bool DownloadSounds()
        {
        
            StartSoundDownload();

            while (SoundUpdateProcess.HasExited == false)
            {
                Thread.Sleep(250);
            }

            if (SoundUpdateProcess.ExitCode != 0)
            {
                MessageBox.Show(FirmwareFileName + " Download Failed! Make sure programming cable is attached and try again");
                BackgroundMessageData = "";
                return false;
            }

            return true;

        }

        private void StartSoundDownload()
        {
            SoundUpdateProcess = new Process();

            SoundUpdateProcess.StartInfo.FileName = "config\\programmer\\NFLASH ";
            SoundUpdateProcess.StartInfo.Arguments = ProfilesPath + "\\" + CurrentProfile + "\\" + "FlashImage.bin AUTO";
            SoundUpdateProcess.StartInfo.UseShellExecute = false;
            SoundUpdateProcess.StartInfo.RedirectStandardOutput = true;
            SoundUpdateProcess.StartInfo.CreateNoWindow = true;
            SoundUpdateProcess.OutputDataReceived += new DataReceivedEventHandler(SoundUpdateProcess_OutputDataReceived);

            SoundUpdateProcess.Start();
            SoundUpdateProcess.BeginOutputReadLine();

        }

        void SoundUpdateProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (!String.IsNullOrEmpty(e.Data))
            {
                BackgroundMessageData = e.Data;
            }
        }

        #endregion

    
        #region Printing

        void ShowPrintSetup()
        {
            Stream MyStream;
            BinaryFormatter MyBinaryFormatter;

            MyPrintDialog.ShowDialog();
            MyPrintOutput.PrinterSettings = MyPrintDialog.PrinterSettings;

            //Save The new print Settings
            MyStream = File.Open("PrintSettings", FileMode.Create);
            MyBinaryFormatter = new BinaryFormatter();
            MyBinaryFormatter.Serialize(MyStream, MyPrintOutput.PrinterSettings);
            MyStream.Close();
        }

        void MyPrintOutput_PrintPage(object sender, PrintPageEventArgs e)
        {
            PointF UpperLeftCorner = new PointF(e.PageSettings.PrintableArea.X, e.PageSettings.PrintableArea.Y);
            Image TopImage = NEOS_PRODUCTION_PROGRAMMER.Properties.Resources.Playworld;
            //Calculate Width so it is 3.6" wide

            float TopImageHieghtMills = (float)(((float)TopImage.Height / (float)e.PageSettings.PrinterResolution.Y) * 100.0);
            float TopImageWidthMills = (float)(((float)TopImage.Width / (float)e.PageSettings.PrinterResolution.X) * 100.0);
            float ScaleFactor = 360 / TopImageWidthMills;

            RectangleF TopImageRectDest = new RectangleF(UpperLeftCorner.X, UpperLeftCorner.Y
                                                        , TopImageWidthMills * ScaleFactor, TopImageHieghtMills * ScaleFactor);

            e.Graphics.DrawImage(TopImage, TopImageRectDest);
            e.Graphics.DrawLine(new Pen(Brushes.Black, 3), 0, TopImageHieghtMills * ScaleFactor + 10, 400, TopImageHieghtMills * ScaleFactor + 10);


            float NeosImageSizeX = 100;
            float NeosImageSizeY = 50;

            float NeosLogoImageOffset = UpperLeftCorner.Y + (TopImageHieghtMills * ScaleFactor) +20;
       
            Image NEOSLogo = NEOS_PRODUCTION_PROGRAMMER.Properties.Resources.NEOS_LOGO;
            
            e.Graphics.DrawImage(NEOSLogo, new RectangleF(UpperLeftCorner.X, NeosLogoImageOffset, NeosImageSizeX, NeosImageSizeY));

            float ConfigTextHieght = 12;
            Font ConfigFont = new Font(new FontFamily("Verdana"), (float)(ConfigTextHieght / 100.0), FontStyle.Bold, GraphicsUnit.Inch);
            e.Graphics.DrawString(CurrentProfile, ConfigFont, Brushes.Black, 
                new PointF(UpperLeftCorner.X + NeosImageSizeX  + 10, NeosLogoImageOffset  +  (NeosImageSizeY/2) - e.Graphics.MeasureString(CurrentProfile,ConfigFont).Height/2));
         
         
            //Now Render the Text
            float TextStartLocation = NeosLogoImageOffset + (NeosImageSizeY);
            float TextHieght = 20;
            float TextBuffer = 5;
            Font TextFont = new Font(new FontFamily("Arial"), (float)(TextHieght / 100.0), GraphicsUnit.Inch);
                     
            e.Graphics.DrawString("Firmware Version:     " + FirmwareVersion, TextFont, Brushes.Black, new PointF(UpperLeftCorner.X, TextStartLocation));
            e.Graphics.DrawString("Configuration Date:   " + System.DateTime.Now.Month + "/" + System.DateTime.Now.Day + "/" + System.DateTime.Now.Year, TextFont, Brushes.Black, new PointF(UpperLeftCorner.X, TextStartLocation + ((TextHieght + TextBuffer) * 1)));
      
        }
      
        #endregion

      



    }



}

