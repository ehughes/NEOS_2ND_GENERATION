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
            this.BUTTON_LED5 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED1 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED3 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED2 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED4 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED0 = new NationalInstruments.UI.WindowsForms.Led();
            this.UIUpdateTimer = new System.Windows.Forms.Timer(this.components);
            this.ButtonStateTimer = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.SelectorPanel)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED5)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED3)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED4)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED0)).BeginInit();
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
            this.SelectorPanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.SelectorPanel_MouseDown);
            this.SelectorPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.SelectorPanel_Paint);
            // 
            // BUTTON_LED5
            // 
            this.BUTTON_LED5.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED5.Location = new System.Drawing.Point(502, 156);
            this.BUTTON_LED5.Name = "BUTTON_LED5";
            this.BUTTON_LED5.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED5.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED5.TabIndex = 21;
            this.BUTTON_LED5.Click += new System.EventHandler(this.NEOSButtonHit_5);
            // 
            // BUTTON_LED1
            // 
            this.BUTTON_LED1.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED1.Location = new System.Drawing.Point(349, 156);
            this.BUTTON_LED1.Name = "BUTTON_LED1";
            this.BUTTON_LED1.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED1.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED1.TabIndex = 20;
            this.BUTTON_LED1.Click += new System.EventHandler(this.NEOSButtonHit_1);
            // 
            // BUTTON_LED3
            // 
            this.BUTTON_LED3.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED3.Location = new System.Drawing.Point(429, 32);
            this.BUTTON_LED3.Name = "BUTTON_LED3";
            this.BUTTON_LED3.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED3.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED3.TabIndex = 18;
            this.BUTTON_LED3.Click += new System.EventHandler(this.NEOSButtonHit_3);
            // 
            // BUTTON_LED2
            // 
            this.BUTTON_LED2.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED2.Location = new System.Drawing.Point(349, 77);
            this.BUTTON_LED2.Name = "BUTTON_LED2";
            this.BUTTON_LED2.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED2.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED2.TabIndex = 16;
            this.BUTTON_LED2.Click += new System.EventHandler(this.NEOSButtonHit_2);
            // 
            // BUTTON_LED4
            // 
            this.BUTTON_LED4.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED4.Location = new System.Drawing.Point(502, 77);
            this.BUTTON_LED4.Name = "BUTTON_LED4";
            this.BUTTON_LED4.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED4.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED4.TabIndex = 15;
            this.BUTTON_LED4.Click += new System.EventHandler(this.NEOSButtonHit_4);
            // 
            // BUTTON_LED0
            // 
            this.BUTTON_LED0.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED0.Location = new System.Drawing.Point(429, 192);
            this.BUTTON_LED0.Name = "BUTTON_LED0";
            this.BUTTON_LED0.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED0.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED0.TabIndex = 14;
            this.BUTTON_LED0.Value = true;
            this.BUTTON_LED0.Click += new System.EventHandler(this.NEOSButtonHit_0);
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
            // NEOSLightRingSimulator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(604, 476);
            this.Controls.Add(this.SelectorPanel);
            this.Controls.Add(this.BUTTON_LED5);
            this.Controls.Add(this.BUTTON_LED1);
            this.Controls.Add(this.BUTTON_LED3);
            this.Controls.Add(this.BUTTON_LED2);
            this.Controls.Add(this.BUTTON_LED4);
            this.Controls.Add(this.BUTTON_LED0);
            this.Name = "NEOSLightRingSimulator";
            this.Text = "NEOSLightRingSimulator";
            ((System.ComponentModel.ISupportInitialize)(this.SelectorPanel)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED5)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED3)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED4)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED0)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox SelectorPanel;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED5;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED1;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED3;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED2;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED4;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED0;
        private System.Windows.Forms.Timer UIUpdateTimer;
        private System.Windows.Forms.Timer ButtonStateTimer;
    }
}