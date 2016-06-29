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
        MainForm mainform;
        int lastindex { get; set; }

        public LandScapeForm(MainForm mf)
        {
            InitializeComponent();
            mainform = mf;
            lastindex = -1;
        }

        Landscape getNewLandscape()
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
                return null;
            }
            return new Landscape(name, attack, defense, effect, labelColor.BackColor);
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            mainform.Show();
            changeLandscape(lastindex);
            this.Hide();
        }

        void changeLandscape(int index)
        {
            if (index != -1)
            {
                var ls = getNewLandscape();
                if (ls != null && !ls.Equals(mainform.landscapes[index]))
                {
                    mainform.landscapes[index] = ls;
                    listBox1.Items[index] = ls.Name;
                }
            }
        }

        private void buttonDel_Click(object sender, EventArgs e)
        {
            int index = listBox1.SelectedIndex;
            if (index != -1)
            {
                mainform.landscapes.RemoveAt(index);
                listBox1.Items.RemoveAt(index);
            }
        }

        private void buttonNew_Click(object sender, EventArgs e)
        {
            var ls = getNewLandscape();
            if (ls != null)
            {
                mainform.landscapes.Add(ls);
                listBox1.Items.Add(ls.Name);
            }
        }

        void refreshListbox()
        {
            listBox1.Items.Clear();
            foreach (Landscape ls in mainform.landscapes)
            {
                listBox1.Items.Add(ls.Name);
            }
        }

        protected override void SetVisibleCore(bool value)
        {
            if (value)
                refreshListbox();
            base.SetVisibleCore(value);
        }

        private void labelColor_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                labelColor.BackColor = colorDialog1.Color;
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            changeLandscape(lastindex);
            int index = listBox1.SelectedIndex;
            if (index != -1 && index != lastindex)
            {
                var ls = mainform.landscapes[index];
                textBoxName.Text = ls.Name;
                textBoxAttack.Text = ls.Attack.ToString();
                textBoxDefend.Text = ls.Defense.ToString();
                textBoxEffect.Text = ls.Effect.ToString();
                labelColor.BackColor = ls.LandColor;
            }
            lastindex = index;
        }

        private void LandScapeForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            mainform.Show();
            changeLandscape(lastindex);
        }

        private void textBoxName_TextChanged(object sender, EventArgs e)
        {
            ;
        }

        private void textBoxAttack_TextChanged(object sender, EventArgs e)
        {
            ;
        }

        private void textBoxDefend_TextChanged(object sender, EventArgs e)
        {
            ;
        }

        private void textBoxEffect_TextChanged(object sender, EventArgs e)
        {
            ;
        }

    }
}
