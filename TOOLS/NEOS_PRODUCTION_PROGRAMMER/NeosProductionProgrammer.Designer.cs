namespace NEOS_PRODUCTION_PROGRAMMER
{
    partial class NeosProductionProgrammerMainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NeosProductionProgrammerMainForm));
            this.ProfileSelectionCB = new System.Windows.Forms.ComboBox();
            this.ProgramButton = new System.Windows.Forms.Button();
            this.StatusTextBox = new System.Windows.Forms.TextBox();
            this.FormupdateTimer = new System.Windows.Forms.Timer(this.components);
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.optionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.printerSetupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.previewLabelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // ProfileSelectionCB
            // 
            this.ProfileSelectionCB.Font = new System.Drawing.Font("Microsoft Sans Serif", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ProfileSelectionCB.FormattingEnabled = true;
            this.ProfileSelectionCB.Items.AddRange(new object[] {
            "NEOS_ORIGINAL_MASTER_SLAVE_DISPLAY",
            "NEOS_360_LIGHTRING_SLAVE",
            "NEOS_360_MASTER",
            "NEOS_LIGHTRING_MASTER"});
            this.ProfileSelectionCB.Location = new System.Drawing.Point(12, 27);
            this.ProfileSelectionCB.MaxDropDownItems = 32;
            this.ProfileSelectionCB.Name = "ProfileSelectionCB";
            this.ProfileSelectionCB.Size = new System.Drawing.Size(710, 39);
            this.ProfileSelectionCB.TabIndex = 0;
            // 
            // ProgramButton
            // 
            this.ProgramButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.ProgramButton.Font = new System.Drawing.Font("Arial Black", 72F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ProgramButton.Location = new System.Drawing.Point(12, 72);
            this.ProgramButton.Name = "ProgramButton";
            this.ProgramButton.Size = new System.Drawing.Size(710, 318);
            this.ProgramButton.TabIndex = 1;
            this.ProgramButton.Text = "Program";
            this.ProgramButton.UseVisualStyleBackColor = true;
            this.ProgramButton.Click += new System.EventHandler(this.ProgramButton_Click);
            // 
            // StatusTextBox
            // 
            this.StatusTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.StatusTextBox.Location = new System.Drawing.Point(12, 396);
            this.StatusTextBox.Multiline = true;
            this.StatusTextBox.Name = "StatusTextBox";
            this.StatusTextBox.Size = new System.Drawing.Size(710, 93);
            this.StatusTextBox.TabIndex = 2;
            // 
            // FormupdateTimer
            // 
            this.FormupdateTimer.Enabled = true;
            this.FormupdateTimer.Tick += new System.EventHandler(this.FormupdateTimer_Tick);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.optionsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(734, 24);
            this.menuStrip1.TabIndex = 3;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // optionsToolStripMenuItem
            // 
            this.optionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.configurationToolStripMenuItem,
            this.printerSetupToolStripMenuItem,
            this.previewLabelToolStripMenuItem});
            this.optionsToolStripMenuItem.Name = "optionsToolStripMenuItem";
            this.optionsToolStripMenuItem.Size = new System.Drawing.Size(56, 20);
            this.optionsToolStripMenuItem.Text = "Options";
            // 
            // configurationToolStripMenuItem
            // 
            this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
            this.configurationToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.configurationToolStripMenuItem.Text = "Configuration";
            this.configurationToolStripMenuItem.Click += new System.EventHandler(this.configurationToolStripMenuItem_Click_1);
            // 
            // printerSetupToolStripMenuItem
            // 
            this.printerSetupToolStripMenuItem.Name = "printerSetupToolStripMenuItem";
            this.printerSetupToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.printerSetupToolStripMenuItem.Text = "Printer Setup";
            this.printerSetupToolStripMenuItem.Click += new System.EventHandler(this.printerSetupToolStripMenuItem_Click_1);
            // 
            // previewLabelToolStripMenuItem
            // 
            this.previewLabelToolStripMenuItem.Name = "previewLabelToolStripMenuItem";
            this.previewLabelToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.previewLabelToolStripMenuItem.Text = "Preview Label";
            this.previewLabelToolStripMenuItem.Click += new System.EventHandler(this.previewLabelToolStripMenuItem_Click_1);
            // 
            // NeosProductionProgrammerMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(734, 497);
            this.Controls.Add(this.StatusTextBox);
            this.Controls.Add(this.ProgramButton);
            this.Controls.Add(this.ProfileSelectionCB);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(742, 531);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(742, 531);
            this.Name = "NeosProductionProgrammerMainForm";
            this.RightToLeftLayout = true;
            this.Text = "Neos Production Programmer";
            this.Load += new System.EventHandler(this.NeosProductionProgrammerMainForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.NeosProductionProgrammerMainForm_FormClosing);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox ProfileSelectionCB;
        private System.Windows.Forms.Button ProgramButton;
        private System.Windows.Forms.TextBox StatusTextBox;
        private System.Windows.Forms.Timer FormupdateTimer;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem optionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem printerSetupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem previewLabelToolStripMenuItem;
     
    }
}

