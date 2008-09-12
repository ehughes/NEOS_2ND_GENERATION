namespace CAN_SNIFFER
{
    partial class MessageRouterControl
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
            this.RawCANMessageViewCB = new System.Windows.Forms.CheckBox();
            this.NEOSAllAroundSimulatorCB = new System.Windows.Forms.CheckBox();
            this.LowLevelNEOSCtrlCB = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // RawCANMessageViewCB
            // 
            this.RawCANMessageViewCB.AutoSize = true;
            this.RawCANMessageViewCB.Checked = true;
            this.RawCANMessageViewCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.RawCANMessageViewCB.Location = new System.Drawing.Point(13, 13);
            this.RawCANMessageViewCB.Name = "RawCANMessageViewCB";
            this.RawCANMessageViewCB.Size = new System.Drawing.Size(145, 17);
            this.RawCANMessageViewCB.TabIndex = 0;
            this.RawCANMessageViewCB.Text = "Raw CAN Message View";
            this.RawCANMessageViewCB.UseVisualStyleBackColor = true;
            // 
            // NEOSAllAroundSimulatorCB
            // 
            this.NEOSAllAroundSimulatorCB.AutoSize = true;
            this.NEOSAllAroundSimulatorCB.Location = new System.Drawing.Point(13, 37);
            this.NEOSAllAroundSimulatorCB.Name = "NEOSAllAroundSimulatorCB";
            this.NEOSAllAroundSimulatorCB.Size = new System.Drawing.Size(153, 17);
            this.NEOSAllAroundSimulatorCB.TabIndex = 1;
            this.NEOSAllAroundSimulatorCB.Text = "NEOS All Around Simulator";
            this.NEOSAllAroundSimulatorCB.UseVisualStyleBackColor = true;
            // 
            // LowLevelNEOSCtrlCB
            // 
            this.LowLevelNEOSCtrlCB.AutoSize = true;
            this.LowLevelNEOSCtrlCB.Checked = true;
            this.LowLevelNEOSCtrlCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.LowLevelNEOSCtrlCB.Location = new System.Drawing.Point(13, 60);
            this.LowLevelNEOSCtrlCB.Name = "LowLevelNEOSCtrlCB";
            this.LowLevelNEOSCtrlCB.Size = new System.Drawing.Size(144, 17);
            this.LowLevelNEOSCtrlCB.TabIndex = 2;
            this.LowLevelNEOSCtrlCB.Text = "Low Level NEOS Control";
            this.LowLevelNEOSCtrlCB.UseVisualStyleBackColor = true;
            // 
            // MessageRouterControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(180, 94);
            this.Controls.Add(this.LowLevelNEOSCtrlCB);
            this.Controls.Add(this.NEOSAllAroundSimulatorCB);
            this.Controls.Add(this.RawCANMessageViewCB);
            this.MaximumSize = new System.Drawing.Size(188, 128);
            this.MinimumSize = new System.Drawing.Size(188, 98);
            this.Name = "MessageRouterControl";
            this.Text = "Message Router Control";
            this.Load += new System.EventHandler(this.MessageRouterControl_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MessageRouterControl_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.CheckBox RawCANMessageViewCB;
        public System.Windows.Forms.CheckBox NEOSAllAroundSimulatorCB;
        public System.Windows.Forms.CheckBox LowLevelNEOSCtrlCB;

    }
}