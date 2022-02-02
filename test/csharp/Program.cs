using System;
using System.Windows.Forms;

namespace waifu2x.test {
    internal static class Program {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main() {
            //using (var processor = Waifu2xProcessor.Get()) {
                //using var model = Waifu2xModel.Load(Waifu2xModels.CUnet, processor);

                //using var waifu2x = new Waifu2x(processor);
                //waifu2x.LoadModel(model);

                //using var mat = Waifu2xMat.Load(@"A:\ts\m1.png");

                //using var outMat = waifu2x.Process(mat, 2);

                //outMat.Save(@"A:\ts\mewfesaf1.png", Waifu2xImageFormat.Png);
            //}
            //Waifu2xEnvironment.NCNNDestroyGPUInstance();
            //GC.Collect();
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Main());
        }
    }
}
