using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace arduino_support
{
    public class RealTimeImageMaker
    {
        private int width;//要生成的曲线图的宽度  
        private int height;//要生成的曲线图的高度  
        private Point[] pointList;//用来绘制曲线图的关键点，依次将这些点连接起来即得到曲线图  
        private Random random = new Random();//用于生成随机数  
        private Bitmap currentImage;//当前要绘制的图片  
        private Color backColor;//图片背景色  
        private Color foreColor;//图片前景色  
        /// <summary>  
        /// 图片的高度  
        /// </summary>  
        public int Height
        {
            get { return height; }
            set { height = value; }
        }

        /// <summary>  
        /// 图片的宽度  
        /// </summary>  
        public int Width
        {
            get { return width; }
            set { width = value; }
        }
        /// <summary>  
        /// 构造函数，指定生成的曲线图的宽度和高度  
        /// </summary>  
        /// <param name="width">要生成的曲线图的宽度  
        /// <param name="height">要生成的曲线图的高度  
        public RealTimeImageMaker(int width, int height) : this(width, height, Color.Gray, Color.Blue)
        {

        }
        /// <summary>  
        /// 构造函数，指定生成的曲线图的宽度、高度及背景色和前景色  
        /// </summary>  
        /// <param name="width">要生成的曲线图的宽度  
        /// <param name="height">要生成的曲线图的高度  
        /// <param name="backColor">曲线图背景色  
        /// <param name="foreColor">曲线图前景色  
        public RealTimeImageMaker(int width, int height, Color backColor, Color foreColor)
        {
            this.width = width;
            this.height = height;
            this.backColor = backColor;
            this.foreColor = foreColor;
            pointList = new Point[width];
            Point tempPoint;
            //初始化曲线上的所有点坐标  
            for (int i = 0; i < width; i++)
            {

                tempPoint = new Point();
                //曲线的横坐标沿x轴依次递增，在横向位置上每个像素都有一个点  
                tempPoint.X = i;
                //曲线上每个点的纵坐标随机生成，但保证在显示区域之内  
                tempPoint.Y = random.Next() % height;
                pointList[i] = tempPoint;
            }
        }
        /// <summary>  
        /// 获取当前依次连接曲线上每个点绘制成的曲线  
        /// </summary>  
        /// <returns></returns>  
        public Image GetCurrentCurve()
        {
            //currentImage = historyImage.Clone(new Rectangle(1, 0, width - 1, height), PixelFormat.Format24bppRgb);  
            currentImage = new Bitmap(width, height);
            Point p;
            //将当前定位曲线图的坐标点前移，并且将横坐标减1，  
            //这样做的效果相当于移除当前第一个点  
            for (int i = 0; i < width - 1; i++)
            {
                p = pointList[i + 1];
                pointList[i] = new Point(p.X - 1, p.Y);
            }
            Point tempPoint = new Point();
            //新生成曲线图定位点的最后一个点的坐标  
            tempPoint.X = width;
            //曲线上每个点的纵坐标随机生成，但保证在显示区域之内  
            tempPoint.Y = random.Next(DateTime.Now.Millisecond) % height;
            //在最后再添加一个新坐标点  
            pointList[width - 1] = tempPoint;
            Graphics g = Graphics.FromImage(currentImage);
            g.Clear(backColor);
            //绘制曲线图  
            g.DrawLines(new Pen(foreColor), pointList);
            g.Dispose();
            return currentImage;
        }
    }
    public partial class RealtimeCurve : Form
    {
        Thread thread;
        RealTimeImageMaker rti;
        Color backColor = Color.Black;//指定绘制曲线图的背景色  
        public RealtimeCurve()
        {
            InitializeComponent(); rti = new RealTimeImageMaker(Width, Height, backColor, Color.Green);
            thread = new Thread(new ThreadStart(Run));
            thread.IsBackground = true;//设置为后台线程，不然主程序退出无法结束线程
            thread.Start();
        }
        private void Run()
        {
            while (true)
            {
                Image image = rti.GetCurrentCurve();
                Graphics g = CreateGraphics();
                //用指定背景色清除当前窗体上的图象  
                g.Clear(backColor);
                g.DrawImage(image, 0, 0);
                g.Dispose();
                //每秒钟刷新一次  
                Thread.Sleep(1000);
            }
        }

        private void FormRealTime_FormClosing(object sender, FormClosingEventArgs e)
        {
            //在窗体即将关闭之前中止线程  
            thread.Abort();
        }
    }
}
