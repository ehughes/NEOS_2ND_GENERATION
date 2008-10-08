namespace NEOS_PRODUCTION_PROGRAMMER
{
    partial class OptionsMenu
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(OptionsMenu));
            this.OptionsGroupBox = new System.Windows.Forms.GroupBox();
            this.PrintPreviewCB = new System.Windows.Forms.CheckBox();
            this.PrintLabelCB = new System.Windows.Forms.CheckBox();
            this.DownloadSoundDataCB = new System.Windows.Forms.CheckBox();
            this.DownloadFirmwareCB = new System.Windows.Forms.CheckBox();
            this.ExitOptionsButton = new System.Windows.Forms.Button();
            this.OptionsGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // OptionsGroupBox
            // 
            this.OptionsGroupBox.Controls.Add(this.PrintPreviewCB);
            this.OptionsGroupBox.Controls.Add(this.PrintLabelCB);
            this.OptionsGroupBox.Controls.Add(this.DownloadSoundDataCB);
            this.OptionsGroupBox.Controls.Add(this.DownloadFirmwareCB);
            this.OptionsGroupBox.Location = new System.Drawing.Point(12, 12);
            this.OptionsGroupBox.Name = "OptionsGroupBox";
            this.OptionsGroupBox.Size = new System.Drawing.Size(203, 113);
            this.OptionsGroupBox.TabIndex = 0;
            this.OptionsGroupBox.TabStop = false;
            this.OptionsGroupBox.Text = "Programming Flow";
            // 
            // PrintPreviewCB
            // 
            this.PrintPreviewCB.AutoSize = true;
            this.PrintPreviewCB.Checked = true;
            this.PrintPreviewCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.PrintPreviewCB.Location = new System.Drawing.Point(27, 89);
            this.PrintPreviewCB.Name = "PrintPreviewCB";
            this.PrintPreviewCB.Size = new System.Drawing.Size(88, 17);
            this.PrintPreviewCB.TabIndex = 3;
            this.PrintPreviewCB.Text = "Print Preview";
            this.PrintPreviewCB.UseVisualStyleBackColor = true;
            // 
            // PrintLabelCB
            // 
            this.PrintLabelCB.AutoSize = true;
            this.PrintLabelCB.Checked = true;
            this.PrintLabelCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.PrintLabelCB.Location = new System.Drawing.Point(11, 66);
            this.PrintLabelCB.Name = "PrintLabelCB";
            this.PrintLabelCB.Size = new System.Drawing.Size(76, 17);
            this.PrintLabelCB.TabIndex = 2;
            this.PrintLabelCB.Text = "Print Label";
            this.PrintLabelCB.UseVisualStyleBackColor = true;
            // 
            // DownloadSoundDataCB
            // 
            this.DownloadSoundDataCB.AutoSize = true;
            this.DownloadSoundDataCB.Checked = true;
            this.DownloadSoundDataCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.DownloadSoundDataCB.Location = new System.Drawing.Point(11, 43);
            this.DownloadSoundDataCB.Name = "DownloadSoundDataCB";
            this.DownloadSoundDataCB.Size = new System.Drawing.Size(184, 17);
            this.DownloadSoundDataCB.TabIndex = 1;
            this.DownloadSoundDataCB.Text = "Download Sound Data for Master";
            this.DownloadSoundDataCB.UseVisualStyleBackColor = true;
            // 
            // DownloadFirmwareCB
            // 
            this.DownloadFirmwareCB.AutoSize = true;
            this.DownloadFirmwareCB.Checked = true;
            this.DownloadFirmwareCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.DownloadFirmwareCB.Location = new System.Drawing.Point(11, 20);
            this.DownloadFirmwareCB.Name = "DownloadFirmwareCB";
            this.DownloadFirmwareCB.Size = new System.Drawing.Size(119, 17);
            this.DownloadFirmwareCB.TabIndex = 0;
            this.DownloadFirmwareCB.Text = "Download Firmware";
            this.DownloadFirmwareCB.UseVisualStyleBackColor = true;
            // 
            // ExitOptionsButton
            // 
            this.ExitOptionsButton.Location = new System.Drawing.Point(13, 132);
            this.ExitOptionsButton.Name = "ExitOptionsButton";
            this.ExitOptionsButton.Size = new System.Drawing.Size(75, 23);
            this.ExitOptionsButton.TabIndex = 1;
            this.ExitOptionsButton.Text = "Exit";
            this.ExitOptionsButton.UseVisualStyleBackColor = true;
            this.ExitOptionsButton.Click += new System.EventHandler(this.ExitOptionsButton_Click);
            // 
            // OptionsMenu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(229, 159);
            this.Controls.Add(this.ExitOptionsButton);
            this.Controls.Add(this.OptionsGroupBox);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(237, 193);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(237, 193);
            this.Name = "OptionsMenu";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Options";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.OptionsMenu_FormClosing);
            this.OptionsGroupBox.ResumeLayout(false);
            this.OptionsGroupBox.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

       public System.Windows.Forms.GroupBox OptionsGroupBox;
        public System.Windows.Forms.CheckBox DownloadFirmwareCB;
        public System.Windows.Forms.CheckBox PrintPreviewCB;
        public System.Windows.Forms.CheckBox PrintLabelCB;
        public System.Windows.Forms.CheckBox DownloadSoundDataCB;
        public System.Windows.Forms.Button ExitOptionsButton;
    }
}