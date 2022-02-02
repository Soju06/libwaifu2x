namespace waifu2x.test {
    partial class Waifu2xView {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this._drop_view = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.button1 = new System.Windows.Forms.Button();
            this._model_box = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this._scale_box = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this._noise_box = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this._processor_box = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this._tta_mode_box = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // _drop_view
            // 
            this._drop_view.AllowDrop = true;
            this._drop_view.BackColor = System.Drawing.Color.Transparent;
            this._drop_view.Font = new System.Drawing.Font("맑은 고딕", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this._drop_view.Location = new System.Drawing.Point(81, 95);
            this._drop_view.Name = "_drop_view";
            this._drop_view.Size = new System.Drawing.Size(221, 82);
            this._drop_view.TabIndex = 0;
            this._drop_view.Text = "Image drop here";
            this._drop_view.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this._drop_view.DragDrop += new System.Windows.Forms.DragEventHandler(this._drop_view_DragDrop);
            this._drop_view.DragEnter += new System.Windows.Forms.DragEventHandler(this._drop_view_DragEnter);
            // 
            // panel1
            // 
            this.panel1.AllowDrop = true;
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.panel1.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.panel1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Controls.Add(this._drop_view);
            this.panel1.Location = new System.Drawing.Point(12, 12);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(392, 283);
            this.panel1.TabIndex = 1;
            this.panel1.DragDrop += new System.Windows.Forms.DragEventHandler(this._drop_view_DragDrop);
            this.panel1.DragEnter += new System.Windows.Forms.DragEventHandler(this._drop_view_DragEnter);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(425, 239);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(329, 54);
            this.button1.TabIndex = 2;
            this.button1.Text = "Process image";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // _model_box
            // 
            this._model_box.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._model_box.FormattingEnabled = true;
            this._model_box.Location = new System.Drawing.Point(603, 14);
            this._model_box.Name = "_model_box";
            this._model_box.Size = new System.Drawing.Size(151, 28);
            this._model_box.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(425, 14);
            this.label2.Margin = new System.Windows.Forms.Padding(5);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(172, 27);
            this.label2.TabIndex = 4;
            this.label2.Text = "Model";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // _scale_box
            // 
            this._scale_box.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._scale_box.FormattingEnabled = true;
            this._scale_box.Location = new System.Drawing.Point(603, 51);
            this._scale_box.Name = "_scale_box";
            this._scale_box.Size = new System.Drawing.Size(151, 28);
            this._scale_box.TabIndex = 3;
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(425, 51);
            this.label3.Margin = new System.Windows.Forms.Padding(5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(172, 27);
            this.label3.TabIndex = 4;
            this.label3.Text = "Scale";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // _noise_box
            // 
            this._noise_box.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._noise_box.FormattingEnabled = true;
            this._noise_box.Location = new System.Drawing.Point(603, 88);
            this._noise_box.Name = "_noise_box";
            this._noise_box.Size = new System.Drawing.Size(151, 28);
            this._noise_box.TabIndex = 3;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(425, 88);
            this.label4.Margin = new System.Windows.Forms.Padding(5);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(172, 27);
            this.label4.TabIndex = 4;
            this.label4.Text = "Noise";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // _processor_box
            // 
            this._processor_box.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._processor_box.FormattingEnabled = true;
            this._processor_box.Location = new System.Drawing.Point(603, 125);
            this._processor_box.Name = "_processor_box";
            this._processor_box.Size = new System.Drawing.Size(151, 28);
            this._processor_box.TabIndex = 3;
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(425, 125);
            this.label5.Margin = new System.Windows.Forms.Padding(5);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(172, 27);
            this.label5.TabIndex = 4;
            this.label5.Text = "Processor";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // _tta_mode_box
            // 
            this._tta_mode_box.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._tta_mode_box.FormattingEnabled = true;
            this._tta_mode_box.Location = new System.Drawing.Point(603, 162);
            this._tta_mode_box.Name = "_tta_mode_box";
            this._tta_mode_box.Size = new System.Drawing.Size(151, 28);
            this._tta_mode_box.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(425, 162);
            this.label1.Margin = new System.Windows.Forms.Padding(5);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(172, 27);
            this.label1.TabIndex = 4;
            this.label1.Text = "TTA Mode";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Waifu2xView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(766, 307);
            this.ControlBox = false;
            this.Controls.Add(this.label1);
            this.Controls.Add(this._tta_mode_box);
            this.Controls.Add(this.label5);
            this.Controls.Add(this._processor_box);
            this.Controls.Add(this.label4);
            this.Controls.Add(this._noise_box);
            this.Controls.Add(this.label3);
            this.Controls.Add(this._scale_box);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._model_box);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.panel1);
            this.HelpButton = true;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Waifu2xView";
            this.Text = "Waifu2xView";
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label _drop_view;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ComboBox _model_box;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox _scale_box;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox _noise_box;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox _processor_box;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox _tta_mode_box;
        private System.Windows.Forms.Label label1;
    }
}