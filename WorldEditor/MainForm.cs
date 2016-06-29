using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows;

namespace WorldEditor
{
    public partial class MainForm : Form
    {

        #region UI 
        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            landscapeForm = new LandScapeForm(this);
            landscapes = MapParser.readLandscape(landscapeJsonName);
            refreshComboBox();
            newToolStripMenuItem_Click(sender, e);
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int rows, cols;
            try
            {
                rows = Convert.ToInt32(textBoxRows.Text);
                cols = Convert.ToInt32(textBoxCols.Text);
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("请不要用不是数字的东西来日地图编辑器");
                return;
            }
            if (rows > 0 && rows < maxSize && cols > 0 && cols < maxSize)
            {
                if (landscapes.Count > 0)
                {
                    map = new Map(rows, cols, landscapes[0]);
                }
                else
                {
                    map = new Map(rows, cols, new Landscape());
                }
                DrawMap();
            }
            else
            {
                MessageBox.Show("行数和列数都请在0和"+maxSize.ToString()+"之间");
                return;
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string localFilePath = String.Empty;
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "地图json(*.json)|*.json|所有文件(*.*)|*.*";
            openFileDialog.FilterIndex = 1;
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                localFilePath = openFileDialog.FileName.ToString();
                map = MapParser.readMap(localFilePath);
                DrawMap();
            }

        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string localFilePath = String.Empty;
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "地图json(*.json)|*.json|所有文件(*.*)|*.*";
            saveFileDialog.FilterIndex = 1;
            saveFileDialog.RestoreDirectory = true;
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                localFilePath = saveFileDialog.FileName.ToString();
                MapParser.writeMap(localFilePath, map);
            }
        }

        private void ToolStripMenuItemLoadRes_Click(object sender, EventArgs e)
        {
            string localFilePath = String.Empty;
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "资源csv(*.csv)|*.csv|所有文件(*.*)|*.*";
            openFileDialog.FilterIndex = 1;
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                localFilePath = openFileDialog.FileName.ToString();
                int rows = -1;
                int cols = -1;
                int[,] table = CSVParser.readIntTable(localFilePath, ref rows, ref cols);
                if (table != null)
                {
                    if (rows == map.rows && cols == map.cols)
                    {
                        map.resources = table;
                        DrawMap();
                    }
                    else
                    {
                        MessageBox.Show("csv文件的行列数和地图长宽不对应");
                    }
                }
                else
                {
                    MessageBox.Show("加载csv文件出现错误");
                }
            }
        }

        private void ToolStripMenuItemSaveRes_Click(object sender, EventArgs e)
        {
            string localFilePath = String.Empty;
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "资源csv(*.csv)|*.csv|所有文件(*.*)|*.*";
            saveFileDialog.FilterIndex = 1;
            saveFileDialog.RestoreDirectory = true;
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                localFilePath = saveFileDialog.FileName.ToString();
                CSVParser.writeIntTable(localFilePath, map.resources, map.rows, map.cols);
            }
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            MapParser.writeLandscape(landscapeJsonName, landscapes);
            Environment.Exit(0);
        }

        private void buttonNew_Click(object sender, EventArgs e)
        {
            newToolStripMenuItem_Click(this, e);
        }

        private void buttonLandScape_Click(object sender, EventArgs e)
        {
            landscapeForm.Show();
            this.Hide();
        }

        void refreshComboBox()
        {
            comboBox1.Items.Clear();
            if (landscapes != null)
            {
                foreach (var ls in landscapes)
                {
                    comboBox1.Items.Add(ls.Name);
                }
            }
        }

        protected override void SetVisibleCore(bool value)
        {
            if(value)
                refreshComboBox();
            base.SetVisibleCore(value);
        }
        #endregion

        Map map { get; set; }
        Bitmap mapImage { get; set; }

        LandScapeForm landscapeForm;
        const string landscapeJsonName = "landscape.json";
        public List<Landscape> landscapes { get; set; }

        const int maxSize = 50;

        const int mapStartX = 140;
        const int mapStartY = 30;
        const int mapSquareSize = 12;
        const int mapSquareDist = 13;

        int selectX { get; set; }
        int selectY { get; set; }


        void DrawMap()
        {
            int rows = map.rows;
            int cols = map.cols;
            mapImage = new Bitmap(cols * mapSquareDist, rows * mapSquareDist);
            Graphics g = Graphics.FromImage(mapImage);
            g.Clear(this.BackColor);
            for (int i = 0; i < cols; i++)
            {
                for (int j = 0; j < rows; j++)
                {
                    using (var brush = new System.Drawing.SolidBrush(map.landscape[i, j].LandColor))
                    {
                        Rectangle r = new Rectangle(
                            i * mapSquareDist,
                            j * mapSquareDist,
                            mapSquareSize,
                            mapSquareSize
                        );
                        g.FillRectangle(brush, r);
                    }
                }
            }
            g.Dispose();
            pictureBox1.Width = mapImage.Width;
            pictureBox1.Height = mapImage.Height;
            pictureBox1.Image = mapImage;
            textBoxRows.Text = rows.ToString();
            textBoxCols.Text = cols.ToString();
        }

        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            int res;
            try
            {
                res = Convert.ToInt32(textBoxRes.Text);
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("请不要用不是数字的东西来日地图编辑器");
                textBoxRes.Text = "0";
                return;
            }
            map.resources[selectX, selectY] = res;

            selectX = e.X / mapSquareDist;
            selectY = e.Y / mapSquareDist;
            labelPosX.Text = "当前横坐标:" + selectX.ToString();
            labelPosY.Text = "当前纵坐标:" + selectY.ToString();
            int index = landscapes.FindIndex(ls => ls.Equals(map.landscape[selectX, selectY]));
            if (index != -1)
            {
                comboBox1.SelectedIndex = index;
            }
            else
            {
                comboBox1.SelectedIndex = -1;
            }
            textBoxRes.Text = map.resources[selectX, selectY].ToString();
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox1.SelectedIndex != -1)
            {
                map.landscape[selectX, selectY] = landscapes[comboBox1.SelectedIndex].Clone();
                DrawMap();
            }
        }

        
    }
}
