namespace CAN_SNIFFER
{
    partial class CANSnifferMainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CANSnifferMainForm));
            this.MainCANSnifferToolStrip = new System.Windows.Forms.ToolStrip();
            this.CANConnectButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.SetCANTerminationButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.SetExcludeRangeButton = new System.Windows.Forms.ToolStripButton();
            this.ExcludeRangeEntryBox = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.ResetInterfaceButton = new System.Windows.Forms.ToolStripButton();
            this.CANSnifferMainFormUpdateTimer = new System.Windows.Forms.Timer(this.components);
            this.MainCANSnifferToolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // MainCANSnifferToolStrip
            // 
            this.MainCANSnifferToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.CANConnectButton,
            this.toolStripSeparator1,
            this.SetCANTerminationButton,
            this.toolStripSeparator2,
            this.SetExcludeRangeButton,
            this.ExcludeRangeEntryBox,
            this.toolStripSeparator3,
            this.ResetInterfaceButton});
            this.MainCANSnifferToolStrip.Location = new System.Drawing.Point(0, 0);
            this.MainCANSnifferToolStrip.Name = "MainCANSnifferToolStrip";
            this.MainCANSnifferToolStrip.Size = new System.Drawing.Size(1022, 25);
            this.MainCANSnifferToolStrip.TabIndex = 0;
            this.MainCANSnifferToolStrip.Text = "toolStrip1";
            // 
            // CANConnectButton
            // 
            this.CANConnectButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.CANConnectButton.Image = ((System.Drawing.Image)(resources.GetObject("CANConnectButton.Image")));
            this.CANConnectButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.CANConnectButton.Name = "CANConnectButton";
            this.CANConnectButton.Size = new System.Drawing.Size(23, 22);
            this.CANConnectButton.Text = "toolStripButton1";
            this.CANConnectButton.ToolTipText = "Connect to CAN Adapter";
            this.CANConnectButton.Click += new System.EventHandler(this.CANConnectButton_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // SetCANTerminationButton
            // 
            this.SetCANTerminationButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.SetCANTerminationButton.Image = ((System.Drawing.Image)(resources.GetObject("SetCANTerminationButton.Image")));
            this.SetCANTerminationButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SetCANTerminationButton.Name = "SetCANTerminationButton";
            this.SetCANTerminationButton.Size = new System.Drawing.Size(84, 22);
            this.SetCANTerminationButton.Text = "Termination On";
            this.SetCANTerminationButton.ToolTipText = "Turn on internal CAN Termination";
            this.SetCANTerminationButton.Click += new System.EventHandler(this.SetCANTermination_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // SetExcludeRangeButton
            // 
            this.SetExcludeRangeButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.SetExcludeRangeButton.Image = ((System.Drawing.Image)(resources.GetObject("SetExcludeRangeButton.Image")));
            this.SetExcludeRangeButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SetExcludeRangeButton.Name = "SetExcludeRangeButton";
            this.SetExcludeRangeButton.Size = new System.Drawing.Size(101, 22);
            this.SetExcludeRangeButton.Text = "Set Exclude Range";
            this.SetExcludeRangeButton.Click += new System.EventHandler(this.SetExcludeRangeButton_Click);
            // 
            // ExcludeRangeEntryBox
            // 
            this.ExcludeRangeEntryBox.Name = "ExcludeRangeEntryBox";
            this.ExcludeRangeEntryBox.Size = new System.Drawing.Size(100, 25);
            this.ExcludeRangeEntryBox.Text = "0x100:0x1FF";
            this.ExcludeRangeEntryBox.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // ResetInterfaceButton
            // 
            this.ResetInterfaceButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.ResetInterfaceButton.Image = ((System.Drawing.Image)(resources.GetObject("ResetInterfaceButton.Image")));
            this.ResetInterfaceButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.ResetInterfaceButton.Name = "ResetInterfaceButton";
            this.ResetInterfaceButton.Size = new System.Drawing.Size(39, 22);
            this.ResetInterfaceButton.Text = "Reset";
            this.ResetInterfaceButton.Click += new System.EventHandler(this.ResetInterfaceButton_Click);
            // 
            // CANSnifferMainFormUpdateTimer
            // 
            this.CANSnifferMainFormUpdateTimer.Enabled = true;
            this.CANSnifferMainFormUpdateTimer.Interval = 250;
            this.CANSnifferMainFormUpdateTimer.Tick += new System.EventHandler(this.CANSnifferMainFormUpdateTimer_Tick);
            // 
            // CANSnifferMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1022, 624);
            this.Controls.Add(this.MainCANSnifferToolStrip);
            this.IsMdiContainer = true;
            this.Name = "CANSnifferMainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "CAN Adapter - No Active Connection";
            this.Load += new System.EventHandler(this.CANSnifferMainForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.CANSnifferMainForm_FormClosing);
            this.MainCANSnifferToolStrip.ResumeLayout(false);
            this.MainCANSnifferToolStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip MainCANSnifferToolStrip;
        private System.Windows.Forms.ToolStripButton CANConnectButton;
        private System.Windows.Forms.Timer CANSnifferMainFormUpdateTimer;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton SetCANTerminationButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton SetExcludeRangeButton;
        private System.Windows.Forms.ToolStripTextBox ExcludeRangeEntryBox;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton ResetInterfaceButton;
    }
}

