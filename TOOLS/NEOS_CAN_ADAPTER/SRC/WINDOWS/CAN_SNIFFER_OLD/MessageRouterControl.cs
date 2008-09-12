using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CAN_SNIFFER
{
    public partial class MessageRouterControl : Form
    {
        public MessageRouterControl()
        {
            InitializeComponent();
        }

        private void MessageRouterControl_Load(object sender, EventArgs e)
        {

        }

        private void MessageRouterControl_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
        }

        public bool SuscriberEnable(string Suscriber)
        {
            bool enabled;

            switch (Suscriber)
            {
                case "RAW CAN Message View":
                    if (RawCANMessageViewCB.Checked == true)
                    {
                        enabled = true;
                    }
                    else
                    {
                        enabled = false;
                    }
                    break;

                case "NEOS All Around Simulator":
                    if (NEOSAllAroundSimulatorCB.Checked == true)
                    {
                        enabled = true;
                    }
                     else
                    {
                        enabled = false;
                    }
                    break;

                case "Low Level NEOS Control":
                    if (LowLevelNEOSCtrlCB.Checked == true)
                    {
                        enabled = true;
                    }
                    else
                    {
                        enabled = false;
                    }
                    break;


                default:

                    enabled = false;
                    break;
               
            }

            return enabled;
          }
    }
}