namespace CAN_SNIFFER
{
    partial class NEOSLightRingSimulator
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
            this.SelectorPanel = new System.Windows.Forms.PictureBox();
            this.UIUpdateTimer = new System.Windows.Forms.Timer(this.components);
            this.ButtonStateTimer = new System.Windows.Forms.Timer(this.components);
            this.BUTTON_LED0 = new LBSoft.IndustrialCtrls.Leds.LBLed();
            this.BUTTON_LED1 = new LBSoft.IndustrialCtrls.Leds.LBLed();
            this.BUTTON_LED2 = new LBSoft.IndustrialCtrls.Leds.LBLed();
            this.BUTTON_LED3 = new LBSoft.IndustrialCtrls.Leds.LBLed();
            this.BUTTON_LED4 = new LBSoft.IndustrialCtrls.Leds.LBLed();
            this.BUTTON_LED5 = new LBSoft.IndustrialCtrls.Leds.LBLed();
            this.ClickLabel = new System.Windows.Forms.Label();
            this.SimButtonLabel = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.SelectorPanel)).BeginInit();
            this.SuspendLayout();
            // 
            // SelectorPanel
            // 
            this.SelectorPanel.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.SelectorPanel.Location = new System.Drawing.Point(12, 12);
            this.SelectorPanel.Name = "SelectorPanel";
            this.SelectorPanel.Size = new System.Drawing.Size(300, 448);
            this.SelectorPanel.TabIndex = 22;
            this.SelectorPanel.TabStop = false;
            this.SelectorPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.SelectorPanel_Paint);
            this.SelectorPanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.SelectorPanel_MouseDown);
            // 
            // UIUpdateTimer
            // 
            this.UIUpdateTimer.Enabled = true;
            this.UIUpdateTimer.Interval = 25;
            this.UIUpdateTimer.Tick += new System.EventHandler(this.UIUpdateTimer_Tick);
            // 
            // ButtonStateTimer
            // 
            this.ButtonStateTimer.Enabled = true;
            this.ButtonStateTimer.Interval = 10;
            this.ButtonStateTimer.Tick += new System.EventHandler(this.ButtonStateTimer_Tick);
            // 
            // BUTTON_LED0
            // 
            this.BUTTON_LED0.BackColor = System.Drawing.Color.Transparent;
            this.BUTTON_LED0.BlinkInterval = 500;
            this.BUTTON_LED0.Label = "";
            this.BUTTON_LED0.LabelPosition = LBSoft.IndustrialCtrls.Leds.LBLed.LedLabelPosition.Top;
            this.BUTTON_LED0.LedColor = System.Drawing.Color.Red;
            this.BUTTON_LED0.LedSize = new System.Drawing.SizeF(25F, 25F);
            this.BUTTON_LED0.Location = new System.Drawing.Point(449, 313);
            this.BUTTON_LED0.Name = "BUTTON_LED0";
            this.BUTTON_LED0.Renderer = null;
            this.BUTTON_LED0.Size = new System.Drawing.Size(35, 30);
            this.BUTTON_LED0.State = LBSoft.IndustrialCtrls.Leds.LBLed.LedState.On;
            this.BUTTON_LED0.TabIndex = 23;
            // 
            // BUTTON_LED1
            // 
            this.BUTTON_LED1.BackColor = System.Drawing.Color.Transparent;
            this.BUTTON_LED1.BlinkInterval = 500;
            this.BUTTON_LED1.Label = "";
            this.BUTTON_LED1.LabelPosition = LBSoft.IndustrialCtrls.Leds.LBLed.LedLabelPosition.Top;
            this.BUTTON_LED1.LedColor = System.Drawing.Color.Red;
            this.BUTTON_LED1.LedSize = new System.Drawing.SizeF(25F, 25F);
            this.BUTTON_LED1.Location = new System.Drawing.Point(371, 262);
            this.BUTTON_LED1.Name = "BUTTON_LED1";
            this.BUTTON_LED1.Renderer = null;
            this.BUTTON_LED1.Size = new System.Drawing.Size(35, 30);
            this.BUTTON_LED1.State = LBSoft.IndustrialCtrls.Leds.LBLed.LedState.On;
            this.BUTTON_LED1.TabIndex = 24;
            // 
            // BUTTON_LED2
            // 
            this.BUTTON_LED2.BackColor = System.Drawing.Color.Transparent;
            this.BUTTON_LED2.BlinkInterval = 500;
            this.BUTTON_LED2.Label = "";
            this.BUTTON_LED2.LabelPosition = LBSoft.IndustrialCtrls.Leds.LBLed.LedLabelPosition.Top;
            this.BUTTON_LED2.LedColor = System.Drawing.Color.Red;
            this.BUTTON_LED2.LedSize = new System.Drawing.SizeF(25F, 25F);
            this.BUTTON_LED2.Location = new System.Drawing.Point(371, 171);
            this.BUTTON_LED2.Name = "BUTTON_LED2";
            this.BUTTON_LED2.Renderer = null;
            this.BUTTON_LED2.Size = new System.Drawing.Size(35, 30);
            this.BUTTON_LED2.State = LBSoft.IndustrialCtrls.Leds.LBLed.LedState.On;
            this.BUTTON_LED2.TabIndex = 25;
            // 
            // BUTTON_LED3
            // 
            this.BUTTON_LED3.BackColor = System.Drawing.Color.Transparent;
            this.BUTTON_LED3.BlinkInterval = 500;
            this.BUTTON_LED3.Label = "";
            this.BUTTON_LED3.LabelPosition = LBSoft.IndustrialCtrls.Leds.LBLed.LedLabelPosition.Top;
            this.BUTTON_LED3.LedColor = System.Drawing.Color.Red;
            this.BUTTON_LED3.LedSize = new System.Drawing.SizeF(25F, 25F);
            this.BUTTON_LED3.Location = new System.Drawing.Point(449, 121);
            this.BUTTON_LED3.Name = "BUTTON_LED3";
            this.BUTTON_LED3.Renderer = null;
            this.BUTTON_LED3.Size = new System.Drawing.Size(35, 30);
            this.BUTTON_LED3.State = LBSoft.IndustrialCtrls.Leds.LBLed.LedState.On;
            this.BUTTON_LED3.TabIndex = 26;
            // 
            // BUTTON_LED4
            // 
            this.BUTTON_LED4.BackColor = System.Drawing.Color.Transparent;
            this.BUTTON_LED4.BlinkInterval = 500;
            this.BUTTON_LED4.Label = "";
            this.BUTTON_LED4.LabelPosition = LBSoft.IndustrialCtrls.Leds.LBLed.LedLabelPosition.Top;
            this.BUTTON_LED4.LedColor = System.Drawing.Color.Red;
            this.BUTTON_LED4.LedSize = new System.Drawing.SizeF(25F, 25F);
            this.BUTTON_LED4.Location = new System.Drawing.Point(531, 171);
            this.BUTTON_LED4.Name = "BUTTON_LED4";
            this.BUTTON_LED4.Renderer = null;
            this.BUTTON_LED4.Size = new System.Drawing.Size(35, 30);
            this.BUTTON_LED4.State = LBSoft.IndustrialCtrls.Leds.LBLed.LedState.On;
            this.BUTTON_LED4.TabIndex = 27;
            // 
            // BUTTON_LED5
            // 
            this.BUTTON_LED5.BackColor = System.Drawing.Color.Transparent;
            this.BUTTON_LED5.BlinkInterval = 500;
            this.BUTTON_LED5.Label = "";
            this.BUTTON_LED5.LabelPosition = LBSoft.IndustrialCtrls.Leds.LBLed.LedLabelPosition.Top;
            this.BUTTON_LED5.LedColor = System.Drawing.Color.Red;
            this.BUTTON_LED5.LedSize = new System.Drawing.SizeF(25F, 25F);
            this.BUTTON_LED5.Location = new System.Drawing.Point(531, 262);
            this.BUTTON_LED5.Name = "BUTTON_LED5";
            this.BUTTON_LED5.Renderer = null;
            this.BUTTON_LED5.Size = new System.Drawing.Size(35, 30);
            this.BUTTON_LED5.State = LBSoft.IndustrialCtrls.Leds.LBLed.LedState.On;
            this.BUTTON_LED5.TabIndex = 28;
            // 
            // ClickLabel
            // 
            this.ClickLabel.AutoSize = true;
            this.ClickLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ClickLabel.Location = new System.Drawing.Point(378, 66);
            this.ClickLabel.Name = "ClickLabel";
            this.ClickLabel.Size = new System.Drawing.Size(172, 24);
            this.ClickLabel.TabIndex = 30;
            this.ClickLabel.Text = "[Click To Interact]";
            // 
            // SimButtonLabel
            // 
            this.SimButtonLabel.AutoSize = true;
            this.SimButtonLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.SimButtonLabel.Location = new System.Drawing.Point(351, 42);
            this.SimButtonLabel.Name = "SimButtonLabel";
            this.SimButtonLabel.Size = new System.Drawing.Size(235, 24);
            this.SimButtonLabel.TabIndex = 29;
            this.SimButtonLabel.Text = "Simulated Buttons/LEDS";
            // 
            // NEOSLightRingSimulator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(604, 476);
            this.Controls.Add(this.ClickLabel);
            this.Controls.Add(this.SimButtonLabel);
            this.Controls.Add(this.BUTTON_LED5);
            this.Controls.Add(this.BUTTON_LED4);
            this.Controls.Add(this.BUTTON_LED3);
            this.Controls.Add(this.BUTTON_LED2);
            this.Controls.Add(this.BUTTON_LED1);
            this.Controls.Add(this.BUTTON_LED0);
            this.Controls.Add(this.SelectorPanel);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(620, 514);
            this.MinimumSize = new System.Drawing.Size(620, 514);
            this.Name = "NEOSLightRingSimulator";
            this.Text = "NEOSLightRingSimulator";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.NEOSLightRingSimulator_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.SelectorPanel)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox SelectorPanel;
        private System.Windows.Forms.Timer UIUpdateTimer;
        private System.Windows.Forms.Timer ButtonStateTimer;
        private LBSoft.IndustrialCtrls.Leds.LBLed BUTTON_LED0;
        private LBSoft.IndustrialCtrls.Leds.LBLed BUTTON_LED1;
        private LBSoft.IndustrialCtrls.Leds.LBLed BUTTON_LED2;
        private LBSoft.IndustrialCtrls.Leds.LBLed BUTTON_LED3;
        private LBSoft.IndustrialCtrls.Leds.LBLed BUTTON_LED4;
        private LBSoft.IndustrialCtrls.Leds.LBLed BUTTON_LED5;
        private System.Windows.Forms.Label ClickLabel;
        private System.Windows.Forms.Label SimButtonLabel;
    }
}