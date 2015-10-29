namespace arduino_support
{
    partial class Main
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.SerialportDebug = new System.Windows.Forms.Button();
            this.tcpClient = new System.Windows.Forms.Button();
            this.RealtimeCurve = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // SerialportDebug
            // 
            this.SerialportDebug.Location = new System.Drawing.Point(13, 13);
            this.SerialportDebug.Name = "SerialportDebug";
            this.SerialportDebug.Size = new System.Drawing.Size(120, 23);
            this.SerialportDebug.TabIndex = 0;
            this.SerialportDebug.Text = "SerialportDebug";
            this.SerialportDebug.UseVisualStyleBackColor = true;
            this.SerialportDebug.Click += new System.EventHandler(this.SerialportDebug_Click);
            // 
            // tcpClient
            // 
            this.tcpClient.Location = new System.Drawing.Point(139, 13);
            this.tcpClient.Name = "tcpClient";
            this.tcpClient.Size = new System.Drawing.Size(120, 23);
            this.tcpClient.TabIndex = 0;
            this.tcpClient.Text = "tcpClient";
            this.tcpClient.UseVisualStyleBackColor = true;
            this.tcpClient.Click += new System.EventHandler(this.tcpClient_Click);
            // 
            // RealtimeCurve
            // 
            this.RealtimeCurve.Location = new System.Drawing.Point(265, 13);
            this.RealtimeCurve.Name = "RealtimeCurve";
            this.RealtimeCurve.Size = new System.Drawing.Size(120, 23);
            this.RealtimeCurve.TabIndex = 1;
            this.RealtimeCurve.Text = "RealtimeCurve";
            this.RealtimeCurve.UseVisualStyleBackColor = true;
            this.RealtimeCurve.Click += new System.EventHandler(this.RealtimeCurve_Click);
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(612, 372);
            this.Controls.Add(this.RealtimeCurve);
            this.Controls.Add(this.tcpClient);
            this.Controls.Add(this.SerialportDebug);
            this.Name = "Main";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button SerialportDebug;
        private System.Windows.Forms.Button tcpClient;
        private System.Windows.Forms.Button RealtimeCurve;
    }
}

