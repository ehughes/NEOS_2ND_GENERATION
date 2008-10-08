using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace NEOS_PRODUCTION_PROGRAMMER
{
    public partial class OptionsMenu : Form
    {
        public OptionsMenu()
        {
            InitializeComponent();
        }

        private void ExitOptionsButton_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
            this.Hide();
        }

        private void OptionsMenu_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
        }
    }
}