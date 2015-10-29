using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace arduino_support
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();
        }

        private void SerialportDebug_Click(object sender, EventArgs e)
        {
            arduino_support.SerialportSampleForm SerialportDebug = new arduino_support.SerialportSampleForm();
            SerialportDebug.Show();
        }

        private void tcpClient_Click(object sender, EventArgs e)
        {
            tcpClient tcpClient = new tcpClient();
            tcpClient.Show();
        }

        private void RealtimeCurve_Click(object sender, EventArgs e)
        {
            RealtimeCurve RealtimeCurve = new RealtimeCurve();
            RealtimeCurve.Show();

        }
    }
}
