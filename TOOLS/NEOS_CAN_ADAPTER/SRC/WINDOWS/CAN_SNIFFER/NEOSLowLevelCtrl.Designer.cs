namespace CAN_SNIFFER
{
    partial class NEOSLowLevelCtrl
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
            this.button1 = new System.Windows.Forms.Button();
            this.PONGResponseDisplay = new System.Windows.Forms.TextBox();
            this.PongResponseTimer = new System.Windows.Forms.Timer(this.components);
            this.NodeOperationsGroupBox = new System.Windows.Forms.GroupBox();
            this.GeneralPurposeBufferView = new System.Windows.Forms.TextBox();
            this.GeneralPurposeBufferGroupBox = new System.Windows.Forms.GroupBox();
            this.ClearBufferAcceptanceFlagButton = new System.Windows.Forms.Button();
            this.SetBufferAcceptanceFlagButton = new System.Windows.Forms.Button();
            this.EraseSectorButton = new System.Windows.Forms.Button();
            this.FlashAddressLabel = new System.Windows.Forms.Label();
            this.FlashAddressControl = new System.Windows.Forms.NumericUpDown();
            this.WriteFlashPagefromGPBufferButton = new System.Windows.Forms.Button();
            this.ReadFlashPageIntoGeneralPurposeBufferButton = new System.Windows.Forms.Button();
            this.GPTargetNode = new System.Windows.Forms.NumericUpDown();
            this.GPTargetNodeLabel = new System.Windows.Forms.Label();
            this.WriteBufferButton = new System.Windows.Forms.Button();
            this.ReadBufferButton = new System.Windows.Forms.Button();
            this.ClearBufferButton = new System.Windows.Forms.Button();
            this.SystemModeGroupBox = new System.Windows.Forms.GroupBox();
            this.SetMasterSystemModeButton = new System.Windows.Forms.Button();
            this.MasterSystemModeComboBox = new System.Windows.Forms.ComboBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.EraseFLASHCB = new System.Windows.Forms.CheckBox();
            this.WriteFileToFlashButton = new System.Windows.Forms.Button();
            this.StatusMessageBox = new System.Windows.Forms.TextBox();
            this.StatusMessageUpdate = new System.Windows.Forms.Timer(this.components);
            this.UIDGroupBox = new System.Windows.Forms.GroupBox();
            this.StatisticallyAcceptUIDCB = new System.Windows.Forms.CheckBox();
            this.TargetNodeForUID = new System.Windows.Forms.NumericUpDown();
            this.SendUIDButton = new System.Windows.Forms.Button();
            this.NodeVoltage = new System.Windows.Forms.TextBox();
            this.NodeVoltageGroupBox = new System.Windows.Forms.GroupBox();
            this.RequestNodeVoltages = new System.Windows.Forms.Timer(this.components);
            this.GetNodeVoltagesCB = new System.Windows.Forms.CheckBox();
            this.NodeOperationsGroupBox.SuspendLayout();
            this.GeneralPurposeBufferGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.FlashAddressControl)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GPTargetNode)).BeginInit();
            this.SystemModeGroupBox.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.UIDGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.TargetNodeForUID)).BeginInit();
            this.NodeVoltageGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(6, 19);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(204, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "Ping";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // PONGResponseDisplay
            // 
            this.PONGResponseDisplay.Location = new System.Drawing.Point(6, 48);
            this.PONGResponseDisplay.Multiline = true;
            this.PONGResponseDisplay.Name = "PONGResponseDisplay";
            this.PONGResponseDisplay.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.PONGResponseDisplay.Size = new System.Drawing.Size(204, 103);
            this.PONGResponseDisplay.TabIndex = 1;
            // 
            // PongResponseTimer
            // 
            this.PongResponseTimer.Enabled = true;
            this.PongResponseTimer.Interval = 250;
            this.PongResponseTimer.Tick += new System.EventHandler(this.PongResponseTimer_Tick);
            // 
            // NodeOperationsGroupBox
            // 
            this.NodeOperationsGroupBox.Controls.Add(this.PONGResponseDisplay);
            this.NodeOperationsGroupBox.Controls.Add(this.button1);
            this.NodeOperationsGroupBox.Location = new System.Drawing.Point(12, 12);
            this.NodeOperationsGroupBox.Name = "NodeOperationsGroupBox";
            this.NodeOperationsGroupBox.Size = new System.Drawing.Size(216, 162);
            this.NodeOperationsGroupBox.TabIndex = 2;
            this.NodeOperationsGroupBox.TabStop = false;
            this.NodeOperationsGroupBox.Text = "Node Operations";
            // 
            // GeneralPurposeBufferView
            // 
            this.GeneralPurposeBufferView.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.GeneralPurposeBufferView.Location = new System.Drawing.Point(6, 19);
            this.GeneralPurposeBufferView.Multiline = true;
            this.GeneralPurposeBufferView.Name = "GeneralPurposeBufferView";
            this.GeneralPurposeBufferView.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.GeneralPurposeBufferView.Size = new System.Drawing.Size(302, 143);
            this.GeneralPurposeBufferView.TabIndex = 3;
            this.GeneralPurposeBufferView.TextChanged += new System.EventHandler(this.GeneralPurposeBufferView_TextChanged);
            this.GeneralPurposeBufferView.MouseDown += new System.Windows.Forms.MouseEventHandler(this.GeneralPurposeBufferView_MouseDown);
            // 
            // GeneralPurposeBufferGroupBox
            // 
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.ClearBufferAcceptanceFlagButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.SetBufferAcceptanceFlagButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.EraseSectorButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.FlashAddressLabel);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.FlashAddressControl);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.WriteFlashPagefromGPBufferButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.ReadFlashPageIntoGeneralPurposeBufferButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.GPTargetNode);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.GPTargetNodeLabel);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.WriteBufferButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.ReadBufferButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.ClearBufferButton);
            this.GeneralPurposeBufferGroupBox.Controls.Add(this.GeneralPurposeBufferView);
            this.GeneralPurposeBufferGroupBox.Location = new System.Drawing.Point(234, 12);
            this.GeneralPurposeBufferGroupBox.Name = "GeneralPurposeBufferGroupBox";
            this.GeneralPurposeBufferGroupBox.Size = new System.Drawing.Size(584, 250);
            this.GeneralPurposeBufferGroupBox.TabIndex = 4;
            this.GeneralPurposeBufferGroupBox.TabStop = false;
            this.GeneralPurposeBufferGroupBox.Text = "General Purpose Buffer";
            this.GeneralPurposeBufferGroupBox.Enter += new System.EventHandler(this.GeneralPurposeBufferGroupBox_Enter);
            // 
            // ClearBufferAcceptanceFlagButton
            // 
            this.ClearBufferAcceptanceFlagButton.Location = new System.Drawing.Point(316, 175);
            this.ClearBufferAcceptanceFlagButton.Name = "ClearBufferAcceptanceFlagButton";
            this.ClearBufferAcceptanceFlagButton.Size = new System.Drawing.Size(262, 23);
            this.ClearBufferAcceptanceFlagButton.TabIndex = 13;
            this.ClearBufferAcceptanceFlagButton.Text = "Clear Button Acceptance Flag";
            this.ClearBufferAcceptanceFlagButton.UseVisualStyleBackColor = true;
            this.ClearBufferAcceptanceFlagButton.Click += new System.EventHandler(this.ClearBufferAcceptanceFlagButton_Click);
            // 
            // SetBufferAcceptanceFlagButton
            // 
            this.SetBufferAcceptanceFlagButton.Location = new System.Drawing.Point(316, 146);
            this.SetBufferAcceptanceFlagButton.Name = "SetBufferAcceptanceFlagButton";
            this.SetBufferAcceptanceFlagButton.Size = new System.Drawing.Size(262, 23);
            this.SetBufferAcceptanceFlagButton.TabIndex = 12;
            this.SetBufferAcceptanceFlagButton.Text = "Set Buffer Acceptance Flag";
            this.SetBufferAcceptanceFlagButton.UseVisualStyleBackColor = true;
            this.SetBufferAcceptanceFlagButton.Click += new System.EventHandler(this.SetBufferAcceptanceFlagButton_Click);
            // 
            // EraseSectorButton
            // 
            this.EraseSectorButton.Location = new System.Drawing.Point(397, 57);
            this.EraseSectorButton.Name = "EraseSectorButton";
            this.EraseSectorButton.Size = new System.Drawing.Size(181, 23);
            this.EraseSectorButton.TabIndex = 11;
            this.EraseSectorButton.Text = "EraseSector";
            this.EraseSectorButton.UseVisualStyleBackColor = true;
            this.EraseSectorButton.Click += new System.EventHandler(this.EraseSectorButton_Click);
            // 
            // FlashAddressLabel
            // 
            this.FlashAddressLabel.AutoSize = true;
            this.FlashAddressLabel.Location = new System.Drawing.Point(394, 14);
            this.FlashAddressLabel.Name = "FlashAddressLabel";
            this.FlashAddressLabel.Size = new System.Drawing.Size(73, 13);
            this.FlashAddressLabel.TabIndex = 9;
            this.FlashAddressLabel.Text = "Flash Address";
            // 
            // FlashAddressControl
            // 
            this.FlashAddressControl.Location = new System.Drawing.Point(397, 31);
            this.FlashAddressControl.Maximum = new decimal(new int[] {
            8388607,
            0,
            0,
            0});
            this.FlashAddressControl.Name = "FlashAddressControl";
            this.FlashAddressControl.Size = new System.Drawing.Size(181, 20);
            this.FlashAddressControl.TabIndex = 10;
            // 
            // WriteFlashPagefromGPBufferButton
            // 
            this.WriteFlashPagefromGPBufferButton.Location = new System.Drawing.Point(397, 117);
            this.WriteFlashPagefromGPBufferButton.Name = "WriteFlashPagefromGPBufferButton";
            this.WriteFlashPagefromGPBufferButton.Size = new System.Drawing.Size(181, 23);
            this.WriteFlashPagefromGPBufferButton.TabIndex = 11;
            this.WriteFlashPagefromGPBufferButton.Text = "Write Flash Page Into GP Buffer";
            this.WriteFlashPagefromGPBufferButton.UseVisualStyleBackColor = true;
            this.WriteFlashPagefromGPBufferButton.Click += new System.EventHandler(this.WriteFlashPagefromGPBufferButton_Click);
            // 
            // ReadFlashPageIntoGeneralPurposeBufferButton
            // 
            this.ReadFlashPageIntoGeneralPurposeBufferButton.Location = new System.Drawing.Point(397, 88);
            this.ReadFlashPageIntoGeneralPurposeBufferButton.Name = "ReadFlashPageIntoGeneralPurposeBufferButton";
            this.ReadFlashPageIntoGeneralPurposeBufferButton.Size = new System.Drawing.Size(181, 23);
            this.ReadFlashPageIntoGeneralPurposeBufferButton.TabIndex = 10;
            this.ReadFlashPageIntoGeneralPurposeBufferButton.Text = "Read Flash Page Into GP Buffer";
            this.ReadFlashPageIntoGeneralPurposeBufferButton.UseVisualStyleBackColor = true;
            this.ReadFlashPageIntoGeneralPurposeBufferButton.Click += new System.EventHandler(this.ReadFlashPageIntoGeneralPurposeBufferButton_Click);
            // 
            // GPTargetNode
            // 
            this.GPTargetNode.Location = new System.Drawing.Point(319, 31);
            this.GPTargetNode.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.GPTargetNode.Name = "GPTargetNode";
            this.GPTargetNode.Size = new System.Drawing.Size(72, 20);
            this.GPTargetNode.TabIndex = 9;
            this.GPTargetNode.Value = new decimal(new int[] {
            247,
            0,
            0,
            0});
            // 
            // GPTargetNodeLabel
            // 
            this.GPTargetNodeLabel.AutoSize = true;
            this.GPTargetNodeLabel.Location = new System.Drawing.Point(316, 14);
            this.GPTargetNodeLabel.Name = "GPTargetNodeLabel";
            this.GPTargetNodeLabel.Size = new System.Drawing.Size(67, 13);
            this.GPTargetNodeLabel.TabIndex = 8;
            this.GPTargetNodeLabel.Text = "Target Node";
            // 
            // WriteBufferButton
            // 
            this.WriteBufferButton.Location = new System.Drawing.Point(316, 117);
            this.WriteBufferButton.Name = "WriteBufferButton";
            this.WriteBufferButton.Size = new System.Drawing.Size(75, 23);
            this.WriteBufferButton.TabIndex = 6;
            this.WriteBufferButton.Text = "Write Buffer";
            this.WriteBufferButton.UseVisualStyleBackColor = true;
            this.WriteBufferButton.Click += new System.EventHandler(this.WriteBufferButton_Click);
            // 
            // ReadBufferButton
            // 
            this.ReadBufferButton.Location = new System.Drawing.Point(316, 88);
            this.ReadBufferButton.Name = "ReadBufferButton";
            this.ReadBufferButton.Size = new System.Drawing.Size(75, 23);
            this.ReadBufferButton.TabIndex = 5;
            this.ReadBufferButton.Text = "Read Buffer";
            this.ReadBufferButton.UseVisualStyleBackColor = true;
            this.ReadBufferButton.Click += new System.EventHandler(this.ReadBufferButton_Click);
            // 
            // ClearBufferButton
            // 
            this.ClearBufferButton.Location = new System.Drawing.Point(316, 59);
            this.ClearBufferButton.Name = "ClearBufferButton";
            this.ClearBufferButton.Size = new System.Drawing.Size(75, 23);
            this.ClearBufferButton.TabIndex = 4;
            this.ClearBufferButton.Text = "Clear Buffer";
            this.ClearBufferButton.UseVisualStyleBackColor = true;
            this.ClearBufferButton.Click += new System.EventHandler(this.ClearBufferButton_Click);
            // 
            // SystemModeGroupBox
            // 
            this.SystemModeGroupBox.Controls.Add(this.SetMasterSystemModeButton);
            this.SystemModeGroupBox.Controls.Add(this.MasterSystemModeComboBox);
            this.SystemModeGroupBox.Location = new System.Drawing.Point(18, 300);
            this.SystemModeGroupBox.Name = "SystemModeGroupBox";
            this.SystemModeGroupBox.Size = new System.Drawing.Size(160, 76);
            this.SystemModeGroupBox.TabIndex = 6;
            this.SystemModeGroupBox.TabStop = false;
            this.SystemModeGroupBox.Text = "Master System Mode";
            // 
            // SetMasterSystemModeButton
            // 
            this.SetMasterSystemModeButton.Location = new System.Drawing.Point(6, 46);
            this.SetMasterSystemModeButton.Name = "SetMasterSystemModeButton";
            this.SetMasterSystemModeButton.Size = new System.Drawing.Size(141, 23);
            this.SetMasterSystemModeButton.TabIndex = 7;
            this.SetMasterSystemModeButton.Text = "Set Mode";
            this.SetMasterSystemModeButton.UseVisualStyleBackColor = true;
            this.SetMasterSystemModeButton.Click += new System.EventHandler(this.SetMasterSystemModeButton_Click);
            // 
            // MasterSystemModeComboBox
            // 
            this.MasterSystemModeComboBox.FormattingEnabled = true;
            this.MasterSystemModeComboBox.Items.AddRange(new object[] {
            "GAME_ACTIVE ",
            "USB_FLASH_UPDATE",
            "SYSTEM_IDLE",
            "SYSTEM_SOFTWARE_RESET",
            "SYSTEM_DIAGNOSTICS"});
            this.MasterSystemModeComboBox.Location = new System.Drawing.Point(6, 19);
            this.MasterSystemModeComboBox.Name = "MasterSystemModeComboBox";
            this.MasterSystemModeComboBox.Size = new System.Drawing.Size(141, 21);
            this.MasterSystemModeComboBox.TabIndex = 5;
            this.MasterSystemModeComboBox.SelectedIndexChanged += new System.EventHandler(this.MasterSystemModeComboBox_SelectedIndexChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.EraseFLASHCB);
            this.groupBox1.Controls.Add(this.WriteFileToFlashButton);
            this.groupBox1.Location = new System.Drawing.Point(234, 268);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(584, 56);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Flash File Programming";
            // 
            // EraseFLASHCB
            // 
            this.EraseFLASHCB.AutoSize = true;
            this.EraseFLASHCB.Checked = true;
            this.EraseFLASHCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.EraseFLASHCB.Location = new System.Drawing.Point(397, 24);
            this.EraseFLASHCB.Name = "EraseFLASHCB";
            this.EraseFLASHCB.Size = new System.Drawing.Size(90, 17);
            this.EraseFLASHCB.TabIndex = 13;
            this.EraseFLASHCB.Text = "Erase FLASH";
            this.EraseFLASHCB.UseVisualStyleBackColor = true;
            // 
            // WriteFileToFlashButton
            // 
            this.WriteFileToFlashButton.Location = new System.Drawing.Point(6, 20);
            this.WriteFileToFlashButton.Name = "WriteFileToFlashButton";
            this.WriteFileToFlashButton.Size = new System.Drawing.Size(385, 23);
            this.WriteFileToFlashButton.TabIndex = 12;
            this.WriteFileToFlashButton.Text = "WriteFile to FLASH";
            this.WriteFileToFlashButton.UseVisualStyleBackColor = true;
            this.WriteFileToFlashButton.Click += new System.EventHandler(this.WriteFileToFlashButton_Click);
            // 
            // StatusMessageBox
            // 
            this.StatusMessageBox.Location = new System.Drawing.Point(240, 330);
            this.StatusMessageBox.Multiline = true;
            this.StatusMessageBox.Name = "StatusMessageBox";
            this.StatusMessageBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.StatusMessageBox.Size = new System.Drawing.Size(578, 167);
            this.StatusMessageBox.TabIndex = 0;
            // 
            // StatusMessageUpdate
            // 
            this.StatusMessageUpdate.Enabled = true;
            this.StatusMessageUpdate.Tick += new System.EventHandler(this.FlashStatusUpdate_Tick);
            // 
            // UIDGroupBox
            // 
            this.UIDGroupBox.Controls.Add(this.StatisticallyAcceptUIDCB);
            this.UIDGroupBox.Controls.Add(this.TargetNodeForUID);
            this.UIDGroupBox.Controls.Add(this.SendUIDButton);
            this.UIDGroupBox.Location = new System.Drawing.Point(18, 186);
            this.UIDGroupBox.Name = "UIDGroupBox";
            this.UIDGroupBox.Size = new System.Drawing.Size(160, 108);
            this.UIDGroupBox.TabIndex = 10;
            this.UIDGroupBox.TabStop = false;
            this.UIDGroupBox.Text = "UID";
            // 
            // StatisticallyAcceptUIDCB
            // 
            this.StatisticallyAcceptUIDCB.AutoSize = true;
            this.StatisticallyAcceptUIDCB.Location = new System.Drawing.Point(6, 45);
            this.StatisticallyAcceptUIDCB.Name = "StatisticallyAcceptUIDCB";
            this.StatisticallyAcceptUIDCB.Size = new System.Drawing.Size(137, 17);
            this.StatisticallyAcceptUIDCB.TabIndex = 12;
            this.StatisticallyAcceptUIDCB.Text = "Statistically Accept UID";
            this.StatisticallyAcceptUIDCB.UseVisualStyleBackColor = true;
            // 
            // TargetNodeForUID
            // 
            this.TargetNodeForUID.Location = new System.Drawing.Point(6, 19);
            this.TargetNodeForUID.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.TargetNodeForUID.Name = "TargetNodeForUID";
            this.TargetNodeForUID.Size = new System.Drawing.Size(72, 20);
            this.TargetNodeForUID.TabIndex = 11;
            this.TargetNodeForUID.Value = new decimal(new int[] {
            247,
            0,
            0,
            0});
            // 
            // SendUIDButton
            // 
            this.SendUIDButton.Location = new System.Drawing.Point(6, 68);
            this.SendUIDButton.Name = "SendUIDButton";
            this.SendUIDButton.Size = new System.Drawing.Size(141, 23);
            this.SendUIDButton.TabIndex = 7;
            this.SendUIDButton.Text = "Send UID";
            this.SendUIDButton.UseVisualStyleBackColor = true;
            this.SendUIDButton.Click += new System.EventHandler(this.SendUIDButton_Click);
            // 
            // NodeVoltage
            // 
            this.NodeVoltage.Location = new System.Drawing.Point(6, 19);
            this.NodeVoltage.Multiline = true;
            this.NodeVoltage.Name = "NodeVoltage";
            this.NodeVoltage.Size = new System.Drawing.Size(142, 209);
            this.NodeVoltage.TabIndex = 13;
            // 
            // NodeVoltageGroupBox
            // 
            this.NodeVoltageGroupBox.Controls.Add(this.GetNodeVoltagesCB);
            this.NodeVoltageGroupBox.Controls.Add(this.NodeVoltage);
            this.NodeVoltageGroupBox.Location = new System.Drawing.Point(824, 20);
            this.NodeVoltageGroupBox.Name = "NodeVoltageGroupBox";
            this.NodeVoltageGroupBox.Size = new System.Drawing.Size(154, 289);
            this.NodeVoltageGroupBox.TabIndex = 15;
            this.NodeVoltageGroupBox.TabStop = false;
            this.NodeVoltageGroupBox.Text = "Node Voltage";
            // 
            // RequestNodeVoltages
            // 
            this.RequestNodeVoltages.Enabled = true;
            this.RequestNodeVoltages.Interval = 333;
            this.RequestNodeVoltages.Tick += new System.EventHandler(this.RequestNodeVoltages_Tick);
            // 
            // GetNodeVoltagesCB
            // 
            this.GetNodeVoltagesCB.AutoSize = true;
            this.GetNodeVoltagesCB.Location = new System.Drawing.Point(7, 235);
            this.GetNodeVoltagesCB.Name = "GetNodeVoltagesCB";
            this.GetNodeVoltagesCB.Size = new System.Drawing.Size(87, 17);
            this.GetNodeVoltagesCB.TabIndex = 14;
            this.GetNodeVoltagesCB.Text = "Get Voltages";
            this.GetNodeVoltagesCB.UseVisualStyleBackColor = true;
            // 
            // NEOSLowLevelCtrl
            // 
            this.ClientSize = new System.Drawing.Size(1091, 516);
            this.Controls.Add(this.NodeVoltageGroupBox);
            this.Controls.Add(this.UIDGroupBox);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.StatusMessageBox);
            this.Controls.Add(this.SystemModeGroupBox);
            this.Controls.Add(this.GeneralPurposeBufferGroupBox);
            this.Controls.Add(this.NodeOperationsGroupBox);
            this.Name = "NEOSLowLevelCtrl";
            this.Text = "Low Level NEOS Control";
            this.Load += new System.EventHandler(this.NEOSLowLevelCtrl_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.NEOSLowLevelCtrl_FormClosing);
            this.NodeOperationsGroupBox.ResumeLayout(false);
            this.NodeOperationsGroupBox.PerformLayout();
            this.GeneralPurposeBufferGroupBox.ResumeLayout(false);
            this.GeneralPurposeBufferGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.FlashAddressControl)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GPTargetNode)).EndInit();
            this.SystemModeGroupBox.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.UIDGroupBox.ResumeLayout(false);
            this.UIDGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.TargetNodeForUID)).EndInit();
            this.NodeVoltageGroupBox.ResumeLayout(false);
            this.NodeVoltageGroupBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox PONGResponseDisplay;
        private System.Windows.Forms.Timer PongResponseTimer;
        private System.Windows.Forms.GroupBox NodeOperationsGroupBox;
        private System.Windows.Forms.TextBox GeneralPurposeBufferView;
        private System.Windows.Forms.GroupBox GeneralPurposeBufferGroupBox;
        private System.Windows.Forms.Button ReadBufferButton;
        private System.Windows.Forms.Button ClearBufferButton;
        private System.Windows.Forms.Label GPTargetNodeLabel;
        private System.Windows.Forms.Button WriteBufferButton;
        private System.Windows.Forms.NumericUpDown GPTargetNode;
        private System.Windows.Forms.GroupBox SystemModeGroupBox;
        private System.Windows.Forms.Button SetMasterSystemModeButton;
        private System.Windows.Forms.Button ReadFlashPageIntoGeneralPurposeBufferButton;
        private System.Windows.Forms.Button EraseSectorButton;
        private System.Windows.Forms.Label FlashAddressLabel;
        private System.Windows.Forms.NumericUpDown FlashAddressControl;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox StatusMessageBox;
        private System.Windows.Forms.Button WriteFlashPagefromGPBufferButton;
        private System.Windows.Forms.Button WriteFileToFlashButton;
        private System.Windows.Forms.Timer StatusMessageUpdate;
        private System.Windows.Forms.Button ClearBufferAcceptanceFlagButton;
        private System.Windows.Forms.Button SetBufferAcceptanceFlagButton;
        private System.Windows.Forms.CheckBox EraseFLASHCB;
        private System.Windows.Forms.ComboBox MasterSystemModeComboBox;
        private System.Windows.Forms.GroupBox UIDGroupBox;
        private System.Windows.Forms.NumericUpDown TargetNodeForUID;
        private System.Windows.Forms.Button SendUIDButton;
        private System.Windows.Forms.CheckBox StatisticallyAcceptUIDCB;
        private System.Windows.Forms.TextBox NodeVoltage;
        private System.Windows.Forms.GroupBox NodeVoltageGroupBox;
        private System.Windows.Forms.Timer RequestNodeVoltages;
        private System.Windows.Forms.CheckBox GetNodeVoltagesCB;
    }
}