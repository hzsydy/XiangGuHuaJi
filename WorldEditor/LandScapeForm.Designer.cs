namespace WorldEditor
{
    partial class LandScapeForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonOK = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxName = new System.Windows.Forms.TextBox();
            this.textBoxAttack = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxDefend = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxEffect = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.buttonNew = new System.Windows.Forms.Button();
            this.buttonDel = new System.Windows.Forms.Button();
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.SuspendLayout();
            // 
            // buttonOK
            // 
            this.buttonOK.Location = new System.Drawing.Point(202, 191);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(76, 40);
            this.buttonOK.TabIndex = 0;
            this.buttonOK.Text = "完成";
            this.buttonOK.UseVisualStyleBackColor = true;
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(167, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "名称";
            // 
            // textBoxName
            // 
            this.textBoxName.Location = new System.Drawing.Point(224, 23);
            this.textBoxName.Name = "textBoxName";
            this.textBoxName.Size = new System.Drawing.Size(89, 21);
            this.textBoxName.TabIndex = 2;
            // 
            // textBoxAttack
            // 
            this.textBoxAttack.Location = new System.Drawing.Point(224, 59);
            this.textBoxAttack.Name = "textBoxAttack";
            this.textBoxAttack.Size = new System.Drawing.Size(89, 21);
            this.textBoxAttack.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(167, 62);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "攻击系数";
            // 
            // textBoxDefend
            // 
            this.textBoxDefend.Location = new System.Drawing.Point(224, 95);
            this.textBoxDefend.Name = "textBoxDefend";
            this.textBoxDefend.Size = new System.Drawing.Size(89, 21);
            this.textBoxDefend.TabIndex = 6;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(167, 98);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 12);
            this.label3.TabIndex = 5;
            this.label3.Text = "防御系数";
            // 
            // textBoxEffect
            // 
            this.textBoxEffect.Location = new System.Drawing.Point(224, 130);
            this.textBoxEffect.Name = "textBoxEffect";
            this.textBoxEffect.Size = new System.Drawing.Size(89, 21);
            this.textBoxEffect.TabIndex = 8;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(167, 133);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 12);
            this.label4.TabIndex = 7;
            this.label4.Text = "特殊效果";
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.listBox1.ItemHeight = 12;
            this.listBox1.Location = new System.Drawing.Point(11, 12);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(132, 184);
            this.listBox1.TabIndex = 9;
            // 
            // buttonNew
            // 
            this.buttonNew.Location = new System.Drawing.Point(11, 202);
            this.buttonNew.Name = "buttonNew";
            this.buttonNew.Size = new System.Drawing.Size(61, 29);
            this.buttonNew.TabIndex = 10;
            this.buttonNew.Text = "新建";
            this.buttonNew.UseVisualStyleBackColor = true;
            this.buttonNew.Click += new System.EventHandler(this.buttonNew_Click);
            // 
            // buttonDel
            // 
            this.buttonDel.Location = new System.Drawing.Point(82, 202);
            this.buttonDel.Name = "buttonDel";
            this.buttonDel.Size = new System.Drawing.Size(61, 29);
            this.buttonDel.TabIndex = 11;
            this.buttonDel.Text = "删除";
            this.buttonDel.UseVisualStyleBackColor = true;
            this.buttonDel.Click += new System.EventHandler(this.buttonDel_Click);
            // 
            // colorDialog1
            // 
            this.colorDialog1.AnyColor = true;
            // 
            // LandScapeForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(323, 243);
            this.Controls.Add(this.buttonDel);
            this.Controls.Add(this.buttonNew);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.textBoxEffect);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBoxDefend);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBoxAttack);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBoxName);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonOK);
            this.Name = "LandScapeForm";
            this.Text = "地形编辑器";
            this.Load += new System.EventHandler(this.LandScapeForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonOK;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxName;
        private System.Windows.Forms.TextBox textBoxAttack;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxDefend;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBoxEffect;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Button buttonNew;
        private System.Windows.Forms.Button buttonDel;
        private System.Windows.Forms.ColorDialog colorDialog1;
    }
}