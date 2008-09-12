namespace CAN_SNIFFER
{
    partial class NEOSAllaroundSimulator
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
            this.BUTTON_LED0 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED4 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED2 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED6 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED3 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED7 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED1 = new NationalInstruments.UI.WindowsForms.Led();
            this.BUTTON_LED5 = new NationalInstruments.UI.WindowsForms.Led();
            this.ButtonStateTimer = new System.Windows.Forms.Timer(this.components);
            this.UIUpdateTimer = new System.Windows.Forms.Timer(this.components);
            this.SelectorPanel = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED0)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED4)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED6)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED3)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED7)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED5)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SelectorPanel)).BeginInit();
            this.SuspendLayout();
            // 
            // BUTTON_LED0
            // 
            this.BUTTON_LED0.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED0.Location = new System.Drawing.Point(458, 237);
            this.BUTTON_LED0.Name = "BUTTON_LED0";
            this.BUTTON_LED0.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED0.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED0.TabIndex = 5;
            // 
            // BUTTON_LED4
            // 
            this.BUTTON_LED4.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED4.Location = new System.Drawing.Point(458, 13);
            this.BUTTON_LED4.Name = "BUTTON_LED4";
            this.BUTTON_LED4.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED4.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED4.TabIndex = 6;
            this.BUTTON_LED4.StateChanged += new NationalInstruments.UI.ActionEventHandler(this.led2_StateChanged);
            // 
            // BUTTON_LED2
            // 
            this.BUTTON_LED2.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED2.Location = new System.Drawing.Point(343, 122);
            this.BUTTON_LED2.Name = "BUTTON_LED2";
            this.BUTTON_LED2.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED2.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED2.TabIndex = 7;
            // 
            // BUTTON_LED6
            // 
            this.BUTTON_LED6.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED6.Location = new System.Drawing.Point(559, 122);
            this.BUTTON_LED6.Name = "BUTTON_LED6";
            this.BUTTON_LED6.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED6.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED6.TabIndex = 8;
            // 
            // BUTTON_LED3
            // 
            this.BUTTON_LED3.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED3.Location = new System.Drawing.Point(378, 45);
            this.BUTTON_LED3.Name = "BUTTON_LED3";
            this.BUTTON_LED3.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED3.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED3.TabIndex = 9;
            this.BUTTON_LED3.StateChanged += new NationalInstruments.UI.ActionEventHandler(this.led5_StateChanged);
            // 
            // BUTTON_LED7
            // 
            this.BUTTON_LED7.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED7.Location = new System.Drawing.Point(527, 201);
            this.BUTTON_LED7.Name = "BUTTON_LED7";
            this.BUTTON_LED7.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED7.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED7.TabIndex = 10;
            this.BUTTON_LED7.StateChanged += new NationalInstruments.UI.ActionEventHandler(this.led6_StateChanged);
            // 
            // BUTTON_LED1
            // 
            this.BUTTON_LED1.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED1.Location = new System.Drawing.Point(378, 201);
            this.BUTTON_LED1.Name = "BUTTON_LED1";
            this.BUTTON_LED1.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED1.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED1.TabIndex = 11;
            // 
            // BUTTON_LED5
            // 
            this.BUTTON_LED5.LedStyle = NationalInstruments.UI.LedStyle.Round3D;
            this.BUTTON_LED5.Location = new System.Drawing.Point(527, 45);
            this.BUTTON_LED5.Name = "BUTTON_LED5";
            this.BUTTON_LED5.OffColor = System.Drawing.Color.Black;
            this.BUTTON_LED5.Size = new System.Drawing.Size(37, 37);
            this.BUTTON_LED5.TabIndex = 12;
            // 
            // ButtonStateTimer
            // 
            this.ButtonStateTimer.Enabled = true;
            this.ButtonStateTimer.Interval = 10;
            this.ButtonStateTimer.Tick += new System.EventHandler(this.ButtonStateTimer_Tick_1);
            // 
            // UIUpdateTimer
            // 
            this.UIUpdateTimer.Enabled = true;
            this.UIUpdateTimer.Interval = 25;
            this.UIUpdateTimer.Tick += new System.EventHandler(this.UIUpdateTimer_Tick);
            // 
            // SelectorPanel
            // 
            this.SelectorPanel.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.SelectorPanel.Location = new System.Drawing.Point(12, 12);
            this.SelectorPanel.Name = "SelectorPanel";
            this.SelectorPanel.Size = new System.Drawing.Size(300, 448);
            this.SelectorPanel.TabIndex = 13;
            this.SelectorPanel.TabStop = false;
            this.SelectorPanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.SelectorPanel_MouseDown);
            // 
            // NEOSAllaroundSimulator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(596, 466);
            this.Controls.Add(this.SelectorPanel);
            this.Controls.Add(this.BUTTON_LED5);
            this.Controls.Add(this.BUTTON_LED1);
            this.Controls.Add(this.BUTTON_LED7);
            this.Controls.Add(this.BUTTON_LED3);
            this.Controls.Add(this.BUTTON_LED6);
            this.Controls.Add(this.BUTTON_LED2);
            this.Controls.Add(this.BUTTON_LED4);
            this.Controls.Add(this.BUTTON_LED0);
            this.MaximumSize = new System.Drawing.Size(604, 500);
            this.MinimumSize = new System.Drawing.Size(604, 500);
            this.Name = "NEOSAllaroundSimulator";
            this.Text = "NEOS All Around Simulator";
            this.Load += new System.EventHandler(this.NEOSCtrlPanel_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.NEOSCtrlPanel_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED0)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED4)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED6)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED3)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED7)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.BUTTON_LED5)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SelectorPanel)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED0;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED4;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED2;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED6;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED3;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED7;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED1;
        private NationalInstruments.UI.WindowsForms.Led BUTTON_LED5;
        private System.Windows.Forms.Timer ButtonStateTimer;
        private System.Windows.Forms.Timer UIUpdateTimer;
        private System.Windows.Forms.PictureBox SelectorPanel;
    }
}