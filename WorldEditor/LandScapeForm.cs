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
    public partial class LandScapeForm : Form
    {
        public LandScapeForm()
        {
            InitializeComponent();
        }

        public LandScapeForm(MainForm mf)
        {

        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            string name = textBoxName.Text;
            int attack, defense, effect;
            try
            {
                attack = Convert.ToInt32(textBoxAttack.Text);
                defense = Convert.ToInt32(textBoxDefend.Text);
                effect = Convert.ToInt32(textBoxEffect.Text);
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("请不要用不是数字的东西来日地图编辑器");
                return;
            }
            
        }

        private void buttonDel_Click(object sender, EventArgs e)
        {

        }

        private void buttonNew_Click(object sender, EventArgs e)
        {

        }

        private void LandScapeForm_Load(object sender, EventArgs e)
        {

        }

    }
}
