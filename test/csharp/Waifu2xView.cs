using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using libwaifu2x;

namespace waifu2x.test {
    struct GPUStruct {
        public int index;
        public string name;
        public override string ToString() => name;
    }

    public partial class Waifu2xView : Form {
        ToolStripProgressBar progressBar;
        ToolStripLabel statusLabel, progressLabel;

        public Waifu2xView(ToolStripLabel status, ToolStripProgressBar bar, ToolStripLabel progress) {
            progressBar = bar;
            progressLabel = progress;
            statusLabel = status;
            InitializeComponent();
            _model_box.Items.AddRange(Enum.GetNames<Waifu2xModels>());
            _scale_box.Items.AddRange(6.Repeat(i => (object)(int)Math.Pow(2, i + 1)));
            _noise_box.Items.AddRange(4.Repeat(i => (object)i));
            var procI = _processor_box.Items;
            var gpuCount = Waifu2xEnvironment.GetGPUCount();
            procI.Add(new GPUStruct { index = -1, name = "-1 - CPU" });
            procI.AddRange(gpuCount.Repeat(i =>
                (object)new GPUStruct { index = i, name = $"{i} - {Waifu2xEnvironment.GetGPUName(i)}" }));
            _model_box.SelectedIndex = 0;
            _scale_box.SelectedIndex = 0;
            _noise_box.SelectedIndex = 0;
            _processor_box.SelectedIndex = gpuCount < 1 ? 0 : Waifu2xEnvironment.GetDefaultGPUIndex() + 1;
            _tta_mode_box.Items.Add("true");
            _tta_mode_box.Items.Add("false");
            _tta_mode_box.SelectedIndex = 1;
        }

        private void _drop_view_DragEnter(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(DataFormats.FileDrop) ||
                e.Data.GetDataPresent(DataFormats.Bitmap)) {
                e.Effect = DragDropEffects.Copy;
            }
        }

        Bitmap image;

        private void _drop_view_DragDrop(object sender, DragEventArgs e) {
            Bitmap? img = null;
            if (e.Data.GetDataPresent(DataFormats.FileDrop)) {
                try {
                    var file = (e.Data.GetData(DataFormats.FileDrop) as string[])?.First();
                    if (file != null && File.Exists(file)) {
                        img = new Bitmap(file);
                    }
                } catch (Exception ex) {
                    MessageBox.Show(ex.Message);
                }
            } else if (e.Data.GetDataPresent(DataFormats.Bitmap) && e.Data.GetData(DataFormats.Bitmap) is Bitmap _img) {
                img = _img;
            }

            if (img != null) {
                panel1.BackgroundImage?.Dispose();
                image?.Dispose();
                panel1.BackgroundImage = image = img;
            }
        }

        Thread _process;

        private void button1_Click(object sender, EventArgs e) {
            if (image == null) {
                MessageBox.Show("Images are not selected.");
                return;
            }
            if (_process?.IsAlive == true) {
                _process.Interrupt();
                button1.Text = "Process image";
                ResetStatus("Canceled");
                return;
            }

            using (var dialog = new SaveFileDialog()) {
                dialog.Filter = "Png File|*.png";
                if (dialog.ShowDialog() != DialogResult.OK) return;
                ResetStatus("Starting");
                (_process = new Thread(() => run(dialog.FileName)) { IsBackground = true }).Start();
            }
        }

        void UpdateStatus(string status) {
            Invoke(() => {
                statusLabel.Text = status;
            });
        }

        void UpdateStatus(double status) {
            Invoke(() => {
                progressLabel.Visible = true;
                progressLabel.Text = $"{progressBar.Value = (int)(status * 100)}%";
            });
        }

        void ResetStatus(string status) {
            Invoke(() => {
                UpdateStatus(status);
                progressBar.Value = 0;
                progressLabel.Visible = false;
            });
        }


        private void Invoke(Action p) => Invoke(new MethodInvoker(p));

        T Invoke<T>(Func<T> p) {
            T t = default;
            Invoke(() => {
                t = p.Invoke();
            });
            return t;
        }

        void run(string outPath) {
            try {
                Invoke(() => button1.Text = "Stop process");
                var device = Invoke(() => (GPUStruct)_processor_box.SelectedItem);
                UpdateStatus("Preparing Processor");
                using var processor = Waifu2xProcessor.Get(device.index < 0 ? Waifu2xProcessors.CPU : Waifu2xProcessors.GPU, device.index);
                UpdateStatus("Preparing Waifu2x");
                using var waifu2x = new Waifu2x(processor);
                UpdateStatus("Preparing Model");
                var scale = Invoke(() => (int)_scale_box.SelectedItem);
                using var model = Waifu2xModel.Load(Enum.Parse<Waifu2xModels>(Invoke(() => (string)_model_box.SelectedItem)),
                    processor, Invoke(() => (int)_noise_box.SelectedItem), scale, bool.Parse(Invoke(() => (string)_tta_mode_box.SelectedItem)));
                UpdateStatus("Reading Image");
                byte[] ds;
                using (var ms = new MemoryStream()) {
                    image.Save(ms, ImageFormat.Png);
                     ds = ms.ToArray();
                }
                using var inMat = Waifu2xMat.Load(ds);
                UpdateStatus("Loading Model");
                waifu2x.LoadModel(model);
                UpdateStatus("Processing Image");
                using var status = new Waifu2xProcessStatus();
                bool isRunning = true;

                var upT = new Thread(() => {
                    var c = -1D;
                    while (isRunning) {
                        var sta = status.CurrentTileProgress;
                        if(sta != c) UpdateStatus(c = sta);
                        Thread.Sleep(50);
                    }
                    UpdateStatus(1);
                }) { IsBackground = true };
                upT.Start();
                using var outMat = waifu2x.Process(inMat, scale, status);
                isRunning = false;
                upT.Join();
                ResetStatus("Converting Image");
                using var bmp = MatToBitmap(outMat);
                ResetStatus("Saveing Image");
                bmp.Save(outPath, ImageFormat.Png);
                ResetStatus("Done");
            } catch (Exception ex) {
                MessageBox.Show(ex.Message);
                ResetStatus("Error");
            }
            Waifu2xEnvironment.NCNNDestroyGPUInstance();
            Invoke(() => button1.Text = "Process image");
        }

        Bitmap MatToBitmap(Waifu2xMat mat) {
            var bmp = new Bitmap(mat.Width, mat.Height);
            var em = mat.ElementSize;
            var data = bmp.LockBits(new(0, 0, bmp.Width, bmp.Height), 
                ImageLockMode.ReadWrite, em == 4 ? PixelFormat.Format32bppArgb : PixelFormat.Format24bppRgb);
            var pdata = mat.GetPixelData(true);
            Marshal.Copy(pdata, 0, data.Scan0, pdata.Length);
            bmp.UnlockBits(data);
            return bmp;
        }
    }
}
