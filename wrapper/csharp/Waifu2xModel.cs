using System;

namespace libwaifu2x {
    /// <summary>
    /// model
    /// </summary>
    public class Waifu2xModel : NativeWrapperObject {
        /// <summary>
        /// internal only
        /// </summary>
        public Waifu2xModel(IntPtr handle) : base(handle, 
            Waifu2xWrapper.Waifu2xModel_destroy) {

        }

        /// <summary>
        /// pre padding
        /// </summary>
        public int PrePadding {
            get => Waifu2xWrapper.Waifu2xModel_get_pre_padding(handle);
        }

        /// <summary>
        /// tile size
        /// </summary>
        public int TileSize {
            get => Waifu2xWrapper.Waifu2xModel_get_tile_size(handle);
        }

        /// <summary>
        /// tta mode
        /// </summary>
        public bool TTAMole {
            get => Waifu2xWrapper.Waifu2xModel_get_tta_mode(handle);
        }

        /// <summary>
        /// get parameter data
        /// ncnn::DataReader*
        /// </summary>
        public IntPtr GetParameter() {
            return Waifu2xWrapper.Waifu2xModel_get_parameter(handle);
        }

        /// <summary>
        /// get model data
        /// ncnn::DataReader*
        /// </summary>
        public IntPtr GetModel() {
            return Waifu2xWrapper.Waifu2xModel_get_model(handle);
        }

        /// <summary>
        /// Load the model
        /// </summary>
        /// <param name="model">model</param>
        /// <param name="processor">processor</param>
        /// <param name="noise">noise</param>
        /// <param name="scale">scale</param>
        /// <param name="tta_mode">tta mode</param>
        /// <exception cref="NativeException"></exception>
        public static Waifu2xModel Load(Waifu2xModels model, Waifu2xProcessor processor, int noise = 0, int scale = 2, bool tta_mode = false) {
            Waifu2xWrapper.Waifu2xModel_load_s(out var ptr, model, processor.Handle, noise, scale, tta_mode)
                .EnsureSuccessStatus();
            return new Waifu2xModel(ptr);
        }

        /// <summary>
        /// Load the model
        /// </summary>
        /// <param name="model">model</param>
        /// <param name="tileSize">tile size</param>
        /// <param name="noise">noise</param>
        /// <param name="scale">scale</param>
        /// <param name="tta_mode">tta mode</param>
        /// <exception cref="NativeException"></exception>
        public static Waifu2xModel Load(Waifu2xModels model, int tileSize, int noise = 0, int scale = 2, bool tta_mode = false) {
            Waifu2xWrapper.Waifu2xModel_load_s_a(out var ptr, model, tileSize, noise, scale, tta_mode)
                .EnsureSuccessStatus();
            return new Waifu2xModel(ptr);
        }

        /// <summary>
        /// Load the model
        /// </summary>
        /// <param name="tileSize">tile size</param>
        /// <param name="prePadding">pre padding</param>
        /// <param name="paramFile">parameter file</param>
        /// <param name="modelFile">model file</param>
        /// <param name="tta_mode">tta mode</param>
        /// <exception cref="NativeException"></exception>
        public static Waifu2xModel Load(int tileSize, int prePadding, string paramFile, string modelFile, bool tta_mode = false) {
            Waifu2xWrapper.Waifu2xModel_load_s_b(out var ptr, tileSize, prePadding, paramFile, modelFile, tta_mode)
                .EnsureSuccessStatus();
            return new Waifu2xModel(ptr);
        }
    }
}
