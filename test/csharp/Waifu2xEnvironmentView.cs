using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using libwaifu2x;

namespace waifu2x.test {
    public partial class Waifu2xEnvironmentView : Form {
        public Waifu2xEnvironmentView() {
            InitializeComponent();
        }

        private void Waifu2xEnvironmentView_Shown(object sender, EventArgs e) {
            var isgpu = Waifu2xEnvironment.IsSupportGPU();
            var gpucount = Waifu2xEnvironment.GetGPUCount();
            env_box.Text = $@"
mode: {(isgpu ? "GPU" : "CPU")}
gpu count: {gpucount}
{string.Join('\n', gpucount.Repeat(i => $"{Waifu2xEnvironment.GetGPUName(i)} {Waifu2xEnvironment.GetGPUHeapBudget(i)}MB"))}
";
        }
    }
}
