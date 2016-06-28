using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WorldEditor
{
    public partial class MainForm : Form
    {
        Map map;

        public List<Landscape> landscapes { get; set; }

        const int maxSize = 100;

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
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
                map = new Map(rows, cols);
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
            openFileDialog.Filter = "文本文件(*.txt)|*.txt|所有文件(*.*)|*.*";
            openFileDialog.FilterIndex = 2;
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                localFilePath = openFileDialog.FileName.ToString();
                MapParser.readMap(localFilePath, out map);
            }

        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string localFilePath = String.Empty;
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "文本文件(*.txt)|*.txt|所有文件(*.*)|*.*";
            saveFileDialog.FilterIndex = 2;
            saveFileDialog.RestoreDirectory = true;
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                localFilePath = saveFileDialog.FileName.ToString();
                MapParser.writeMap(localFilePath, map);
            }
        }

        private void MainForm_Click(object sender, EventArgs e)
        {

        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            Environment.Exit(0);
        }

        private void buttonNew_Click(object sender, EventArgs e)
        {
            newToolStripMenuItem_Click(this, e);
        }
    }
}
