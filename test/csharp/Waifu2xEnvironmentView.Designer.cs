namespace waifu2x.test {
    partial class Waifu2xEnvironmentView {
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
            this.env_box = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // env_box
            // 
            this.env_box.Dock = System.Windows.Forms.DockStyle.Fill;
            this.env_box.Location = new System.Drawing.Point(0, 0);
            this.env_box.Margin = new System.Windows.Forms.Padding(10);
            this.env_box.Multiline = true;
            this.env_box.Name = "env_box";
            this.env_box.ReadOnly = true;
            this.env_box.Size = new System.Drawing.Size(386, 309);
            this.env_box.TabIndex = 2;
            this.env_box.Text = "loading...";
            // 
            // Waifu2xEnvironmentView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(386, 309);
            this.ControlBox = false;
            this.Controls.Add(this.env_box);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Waifu2xEnvironmentView";
            this.Text = "Waifu2xEnvironmentView";
            this.Shown += new System.EventHandler(this.Waifu2xEnvironmentView_Shown);
            this.ResumeLayout(false);
            this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox env_box;
}
}