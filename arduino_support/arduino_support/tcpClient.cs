using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading;

namespace arduino_support
{
    public partial class tcpClient : Form
    {
        TcpClient Client;
        Thread thread;
        private const int bufferSize = 10000;
        NetworkStream ntwStream;

        public tcpClient()
        {
            InitializeComponent();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {            
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            //强制关闭程序（强行终止Listener）
            ntwStream.Close();
            Client.Close();
            Environment.Exit(0);
        }

        //发送数据
        private void btnSend_Click(object sender, EventArgs e)
        {
            if (Client != null)
            {
                //要发送的信息
                if (txtSendMsg.Text.Trim() == string.Empty)
                    return;
                string msg = txtSendMsg.Text.Trim();
                //将信息存入缓存中
                byte[] buffer = Encoding.Default.GetBytes(msg);
                //lock (sendStream)
                //{
                ntwStream.Write(buffer, 0, buffer.Length);
                //}
                txtRecvMsg.AppendText("发送给服务端的数据:" + msg + "\n");
                //txtSendMsg.Text = string.Empty;
            }
        }

        public delegate void showData(string msg);//委托,防止跨线程的访问控件，引起的安全异常
        
        private void connect_Click(object sender, EventArgs e)
        {
            if (IP.Text.Trim() == string.Empty)
            {
                return;
            }
            if (Port.Text.Trim() == string.Empty)
            {
                return;
            }
            IPAddress ip = IPAddress.Parse(IP.Text);
            Client = new TcpClient();
            Client.Connect(ip, int.Parse(Port.Text));
            txtRecvMsg.AppendText("开始连接服务端....\n");
            txtRecvMsg.AppendText("已经连接服务端\n");
            //获取用于发送数据的传输流
            ntwStream = Client.GetStream();

            ////打开Listener开始监听
            thread = new Thread(ListenerServer);
            thread.IsBackground = true;//设置为后台线程，不然主程序退出无法结束线程
            thread.Start();
        }

        private void disconnect_Click(object sender, EventArgs e)
        {
            thread.Abort();
            ntwStream.Close();
            Client.Close();
        }

        private void ListenerServer()
        {
            int readSize;
            byte[] buffer = new byte[bufferSize];
            while(thread.IsAlive)
            {
                try
                {
                    lock (ntwStream)
                    {
                        readSize = ntwStream.Read(buffer, 0, bufferSize);
                    }
                    if (readSize == 0)
                        return;
                    txtRecvMsg.Invoke(new showData(txtRecvMsg.AppendText), Encoding.Default.GetString(buffer, 0, readSize));

                }
                catch
                {
                    txtRecvMsg.Invoke(new showData(txtRecvMsg.AppendText), "error to read\n");
                }
                //将缓存中的数据写入传输流
            }
        }
    }
}
