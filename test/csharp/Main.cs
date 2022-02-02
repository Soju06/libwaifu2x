using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using libwaifu2x;

namespace waifu2x.test {
    public partial class Main : Form {
        public Main() {
            InitializeComponent();
            Application.ApplicationExit += ApplicationExit;
        }

        Point _dfP = new Point(20, 20);

        void AddForm(Form form) {
            form.TopLevel = false;
            Controls.Add(form);
            form.Location = _dfP = new Point(_dfP.X + 30, _dfP.Y + 30);
            form.Show();
            form.Enter += Form_Enter;
            form.BringToFront();
            statusStrip1.BringToFront();
        }

        private void Form_Enter(object? sender, EventArgs e) {
            (sender as Form)?.Show();
        }

        // !! important !!
        void ApplicationExit(object? sender, EventArgs e) {
            // You must call this function before the main thread is shut down!
            Waifu2xEnvironment.NCNNDestroyGPUInstance();
        }

        void Main_Load(object sender, EventArgs e) {
            AddForm(new Waifu2xEnvironmentView());
            AddForm(new Waifu2xView(toolStripStatusLabel1, toolStripProgressBar1, toolStripStatusLabel2));
        }
    }
}
