namespace Playworld
{
    partial class NEOSUSBControlPanel
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.Timer WindowUpdateManager;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NEOSUSBControlPanel));
            this.ConnectionManangerToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.PlayButtonTip = new System.Windows.Forms.ToolTip(this.components);
            this.MainFormTip = new System.Windows.Forms.ToolTip(this.components);
            this.BottomToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.TopToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.RightToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.LeftToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.ContentPanel = new System.Windows.Forms.ToolStripContentPanel();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveSoundPackagerFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openSoundPackagerFilerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveSoundPackagerFileAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.uSBToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.disconnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.AllAroundControlPanelMenuStrip = new System.Windows.Forms.MenuStrip();
            this.soundPackagerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.generateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.FlashProgressBar = new System.Windows.Forms.ToolStripProgressBar();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.AllAroundUtilitiesControlStrip = new System.Windows.Forms.ToolStrip();
            this.ConnectButton = new System.Windows.Forms.ToolStripButton();
            this.SoundFileDownloadToolButton = new System.Windows.Forms.ToolStripButton();
            this.MSPResetToolButton = new System.Windows.Forms.ToolStripButton();
            this.GetFirmwareToolButton = new System.Windows.Forms.ToolStripButton();
            this.SoundPlayButton = new System.Windows.Forms.ToolStripButton();
            this.SoundNumberTextBox = new System.Windows.Forms.ToolStripTextBox();
            this.SoundLengthTextBox = new System.Windows.Forms.ToolStripTextBox();
            this.SoundStreamTextBox = new System.Windows.Forms.ToolStripTextBox();
            WindowUpdateManager = new System.Windows.Forms.Timer(this.components);
            this.AllAroundControlPanelMenuStrip.SuspendLayout();
            this.AllAroundUtilitiesControlStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // WindowUpdateManager
            // 
            WindowUpdateManager.Enabled = true;
            WindowUpdateManager.Interval = 10;
            WindowUpdateManager.Tick += new System.EventHandler(this.WindowUpdateManager_Tick);
            // 
            // ConnectionManangerToolTip
            // 
            this.ConnectionManangerToolTip.Popup += new System.Windows.Forms.PopupEventHandler(this.ConnectionManangerToolTip_Popup);
            // 
            // BottomToolStripPanel
            // 
            this.BottomToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.BottomToolStripPanel.Name = "BottomToolStripPanel";
            this.BottomToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.BottomToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.BottomToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // TopToolStripPanel
            // 
            this.TopToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.TopToolStripPanel.Name = "TopToolStripPanel";
            this.TopToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.TopToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.TopToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // RightToolStripPanel
            // 
            this.RightToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.RightToolStripPanel.Name = "RightToolStripPanel";
            this.RightToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.RightToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.RightToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // LeftToolStripPanel
            // 
            this.LeftToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.LeftToolStripPanel.Name = "LeftToolStripPanel";
            this.LeftToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.LeftToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.LeftToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // ContentPanel
            // 
            this.ContentPanel.Size = new System.Drawing.Size(838, 175);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.saveSoundPackagerFileToolStripMenuItem,
            this.openSoundPackagerFilerToolStripMenuItem,
            this.saveSoundPackagerFileAsToolStripMenuItem,
            this.toolStripSeparator4});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(239, 22);
            this.newToolStripMenuItem.Text = "New";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // saveSoundPackagerFileToolStripMenuItem
            // 
            this.saveSoundPackagerFileToolStripMenuItem.Name = "saveSoundPackagerFileToolStripMenuItem";
            this.saveSoundPackagerFileToolStripMenuItem.Size = new System.Drawing.Size(239, 22);
            this.saveSoundPackagerFileToolStripMenuItem.Text = "Save Sound Packager File";
            this.saveSoundPackagerFileToolStripMenuItem.Click += new System.EventHandler(this.saveSoundPackagerFileToolStripMenuItem_Click);
            // 
            // openSoundPackagerFilerToolStripMenuItem
            // 
            this.openSoundPackagerFilerToolStripMenuItem.Name = "openSoundPackagerFilerToolStripMenuItem";
            this.openSoundPackagerFilerToolStripMenuItem.Size = new System.Drawing.Size(239, 22);
            this.openSoundPackagerFilerToolStripMenuItem.Text = "Open Sound Packager File";
            this.openSoundPackagerFilerToolStripMenuItem.Click += new System.EventHandler(this.openSoundPackagerFilerToolStripMenuItem_Click);
            // 
            // saveSoundPackagerFileAsToolStripMenuItem
            // 
            this.saveSoundPackagerFileAsToolStripMenuItem.Name = "saveSoundPackagerFileAsToolStripMenuItem";
            this.saveSoundPackagerFileAsToolStripMenuItem.Size = new System.Drawing.Size(239, 22);
            this.saveSoundPackagerFileAsToolStripMenuItem.Text = "Save Sound Packager File As....";
            this.saveSoundPackagerFileAsToolStripMenuItem.Click += new System.EventHandler(this.saveSoundPackagerFileAsToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(236, 6);
            // 
            // uSBToolStripMenuItem
            // 
            this.uSBToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectToolStripMenuItem,
            this.disconnectToolStripMenuItem});
            this.uSBToolStripMenuItem.Name = "uSBToolStripMenuItem";
            this.uSBToolStripMenuItem.Size = new System.Drawing.Size(38, 20);
            this.uSBToolStripMenuItem.Text = "USB";
            // 
            // connectToolStripMenuItem
            // 
            this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
            this.connectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.connectToolStripMenuItem.Text = "Connect";
            // 
            // disconnectToolStripMenuItem
            // 
            this.disconnectToolStripMenuItem.Name = "disconnectToolStripMenuItem";
            this.disconnectToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.disconnectToolStripMenuItem.Text = "Disconnect";
            // 
            // AllAroundControlPanelMenuStrip
            // 
            this.AllAroundControlPanelMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.uSBToolStripMenuItem,
            this.soundPackagerToolStripMenuItem});
            this.AllAroundControlPanelMenuStrip.Location = new System.Drawing.Point(0, 0);
            this.AllAroundControlPanelMenuStrip.Name = "AllAroundControlPanelMenuStrip";
            this.AllAroundControlPanelMenuStrip.Size = new System.Drawing.Size(792, 24);
            this.AllAroundControlPanelMenuStrip.TabIndex = 27;
            this.AllAroundControlPanelMenuStrip.Text = "AllAroundControlPanelMenuStrip";
            // 
            // soundPackagerToolStripMenuItem
            // 
            this.soundPackagerToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.generateToolStripMenuItem});
            this.soundPackagerToolStripMenuItem.Name = "soundPackagerToolStripMenuItem";
            this.soundPackagerToolStripMenuItem.Size = new System.Drawing.Size(96, 20);
            this.soundPackagerToolStripMenuItem.Text = "Sound Packager";
            // 
            // generateToolStripMenuItem
            // 
            this.generateToolStripMenuItem.Name = "generateToolStripMenuItem";
            this.generateToolStripMenuItem.Size = new System.Drawing.Size(196, 22);
            this.generateToolStripMenuItem.Text = "Generate Binary Image";
            this.generateToolStripMenuItem.Click += new System.EventHandler(this.generateToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 40);
            // 
            // FlashProgressBar
            // 
            this.FlashProgressBar.AutoSize = false;
            this.FlashProgressBar.Name = "FlashProgressBar";
            this.FlashProgressBar.Size = new System.Drawing.Size(150, 24);
            this.FlashProgressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 40);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 40);
            // 
            // AllAroundUtilitiesControlStrip
            // 
            this.AllAroundUtilitiesControlStrip.AutoSize = false;
            this.AllAroundUtilitiesControlStrip.ImageScalingSize = new System.Drawing.Size(32, 32);
            this.AllAroundUtilitiesControlStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ConnectButton,
            this.toolStripSeparator3,
            this.SoundFileDownloadToolButton,
            this.FlashProgressBar,
            this.MSPResetToolButton,
            this.toolStripSeparator1,
            this.GetFirmwareToolButton,
            this.toolStripSeparator2,
            this.SoundPlayButton,
            this.SoundNumberTextBox,
            this.SoundLengthTextBox,
            this.SoundStreamTextBox});
            this.AllAroundUtilitiesControlStrip.Location = new System.Drawing.Point(0, 24);
            this.AllAroundUtilitiesControlStrip.Name = "AllAroundUtilitiesControlStrip";
            this.AllAroundUtilitiesControlStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.AllAroundUtilitiesControlStrip.Size = new System.Drawing.Size(792, 40);
            this.AllAroundUtilitiesControlStrip.TabIndex = 25;
            this.AllAroundUtilitiesControlStrip.Text = "AllAroundUtilitiesControlStrip";
            // 
            // ConnectButton
            // 
            this.ConnectButton.AutoSize = false;
            this.ConnectButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.ConnectButton.Image = global::Playworld.Properties.Resources.USBConnected;
            this.ConnectButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.ConnectButton.Name = "ConnectButton";
            this.ConnectButton.Size = new System.Drawing.Size(48, 32);
            this.ConnectButton.Text = "Connect to Talking Bob";
            this.ConnectButton.ToolTipText = "Connect via USB";
            this.ConnectButton.Click += new System.EventHandler(this.ConnectToNEOS_Click);
            // 
            // SoundFileDownloadToolButton
            // 
            this.SoundFileDownloadToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.SoundFileDownloadToolButton.Image = global::Playworld.Properties.Resources.key_download;
            this.SoundFileDownloadToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SoundFileDownloadToolButton.Name = "SoundFileDownloadToolButton";
            this.SoundFileDownloadToolButton.Size = new System.Drawing.Size(36, 37);
            this.SoundFileDownloadToolButton.Text = "Download Sounds Via USB";
            this.SoundFileDownloadToolButton.ToolTipText = "Download Sounds Via USB";
            this.SoundFileDownloadToolButton.Click += new System.EventHandler(this.KeyFileDownloadToolButton_Click);
            // 
            // MSPResetToolButton
            // 
            this.MSPResetToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.MSPResetToolButton.Image = global::Playworld.Properties.Resources.MSP_RESET;
            this.MSPResetToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.MSPResetToolButton.Name = "MSPResetToolButton";
            this.MSPResetToolButton.Size = new System.Drawing.Size(36, 37);
            this.MSPResetToolButton.Text = "Reset Sound Module";
            this.MSPResetToolButton.Click += new System.EventHandler(this.ResetToolButton_Click);
            // 
            // GetFirmwareToolButton
            // 
            this.GetFirmwareToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.GetFirmwareToolButton.Image = global::Playworld.Properties.Resources.firmware;
            this.GetFirmwareToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.GetFirmwareToolButton.Name = "GetFirmwareToolButton";
            this.GetFirmwareToolButton.Size = new System.Drawing.Size(36, 37);
            this.GetFirmwareToolButton.Text = "Query Firmware Version";
            this.GetFirmwareToolButton.Click += new System.EventHandler(this.GetFirmwareToolButton_Click);
            // 
            // SoundPlayButton
            // 
            this.SoundPlayButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.SoundPlayButton.Image = global::Playworld.Properties.Resources.play;
            this.SoundPlayButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SoundPlayButton.Name = "SoundPlayButton";
            this.SoundPlayButton.Size = new System.Drawing.Size(36, 37);
            this.SoundPlayButton.Text = "Sound Play";
            this.SoundPlayButton.ToolTipText = "Play Sound";
            this.SoundPlayButton.Click += new System.EventHandler(this.SoundPlayButton_Click);
            // 
            // SoundNumberTextBox
            // 
            this.SoundNumberTextBox.Name = "SoundNumberTextBox";
            this.SoundNumberTextBox.Size = new System.Drawing.Size(100, 40);
            this.SoundNumberTextBox.Text = "0";
            this.SoundNumberTextBox.ToolTipText = "Sound Number to Play";
            // 
            // SoundLengthTextBox
            // 
            this.SoundLengthTextBox.Name = "SoundLengthTextBox";
            this.SoundLengthTextBox.Size = new System.Drawing.Size(100, 40);
            this.SoundLengthTextBox.Text = "65535";
            this.SoundLengthTextBox.ToolTipText = "Sound Length";
            // 
            // SoundStreamTextBox
            // 
            this.SoundStreamTextBox.Name = "SoundStreamTextBox";
            this.SoundStreamTextBox.Size = new System.Drawing.Size(100, 40);
            this.SoundStreamTextBox.Text = "0";
            this.SoundStreamTextBox.ToolTipText = "Sound Stream";
            // 
            // NEOSUSBControlPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(792, 466);
            this.Controls.Add(this.AllAroundUtilitiesControlStrip);
            this.Controls.Add(this.AllAroundControlPanelMenuStrip);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.AllAroundControlPanelMenuStrip;
            this.MaximumSize = new System.Drawing.Size(1600, 1200);
            this.MinimumSize = new System.Drawing.Size(800, 100);
            this.Name = "NEOSUSBControlPanel";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Tag = "";
            this.Text = "NEOS USB CONTROL PANEL";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.NEOSUSBControlPanel_FormClosing);
            this.AllAroundControlPanelMenuStrip.ResumeLayout(false);
            this.AllAroundControlPanelMenuStrip.PerformLayout();
            this.AllAroundUtilitiesControlStrip.ResumeLayout(false);
            this.AllAroundUtilitiesControlStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolTip ConnectionManangerToolTip;
        private System.Windows.Forms.ToolTip PlayButtonTip;
        private System.Windows.Forms.ToolTip MainFormTip;
        private System.Windows.Forms.ToolStripPanel BottomToolStripPanel;
        private System.Windows.Forms.ToolStripPanel TopToolStripPanel;
        private System.Windows.Forms.ToolStripPanel RightToolStripPanel;
        private System.Windows.Forms.ToolStripPanel LeftToolStripPanel;
        private System.Windows.Forms.ToolStripContentPanel ContentPanel;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveSoundPackagerFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openSoundPackagerFilerToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveSoundPackagerFileAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem uSBToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem disconnectToolStripMenuItem;
        private System.Windows.Forms.MenuStrip AllAroundControlPanelMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem soundPackagerToolStripMenuItem;
        private System.Windows.Forms.ToolStripButton ConnectButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton SoundFileDownloadToolButton;
        private System.Windows.Forms.ToolStripProgressBar FlashProgressBar;
        private System.Windows.Forms.ToolStripButton MSPResetToolButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton GetFirmwareToolButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStrip AllAroundUtilitiesControlStrip;
        private System.Windows.Forms.ToolStripMenuItem generateToolStripMenuItem;
        private System.Windows.Forms.ToolStripButton SoundPlayButton;
        private System.Windows.Forms.ToolStripTextBox SoundNumberTextBox;
        private System.Windows.Forms.ToolStripTextBox SoundLengthTextBox;
        private System.Windows.Forms.ToolStripTextBox SoundStreamTextBox;
    }
}

