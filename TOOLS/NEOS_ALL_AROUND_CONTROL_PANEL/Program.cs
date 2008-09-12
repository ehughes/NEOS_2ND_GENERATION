using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO.Ports;
using Playworld;

namespace Playworld
{
    static class Program
    {


        /// <summary>
        /// The main entry point for the application.
        /// </summary>
       [STAThread]
         static void Main()
        {
            
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);


            try
                {

                    Application.Run(new AllAroundControlPanel());
                }
            catch(Exception e)
                {
                MessageBox.Show("Oops!! You got an exception.  This means that " +

                    "Eli Missed Something.  Please send the following" +
                    "message to him: \r\n\r\n" + e.ToString());

                }
          
       }
    }
}