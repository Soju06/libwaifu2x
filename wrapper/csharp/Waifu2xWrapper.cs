using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace libwaifu2x {
    /// <summary>
    /// wrapper
    /// </summary>
    public static class Waifu2xWrapper {
        #region Waifu2xEnvironment

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xEnvironment_get_gpu_count();

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xEnvironment_get_gpu_name(int index);

        [DllImport("libwaifu2x.dll")]
        public static extern bool Waifu2xEnvironment_is_support_gpu();

        [DllImport("libwaifu2x.dll")]
        public static extern uint Waifu2xEnvironment_get_gpu_heap_budget(int index);

        [DllImport("libwaifu2x.dll")]
        public static extern uint Waifu2xEnvironment_get_device_id(int index);

        [DllImport("libwaifu2x.dll")]
        public static extern uint Waifu2xEnvironment_get_vendor_id(int index);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2xEnvironment_ncnn_destroy_gpu_instance();

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xEnvironment_get_default_gpu_index();

        #endregion

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xMat_create(IntPtr mat, bool nodatarelease);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2xMat_destroy(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xMat_get_element_size(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xMat_get_height(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xMat_get_width(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xMat_get_pixel_data(IntPtr ptr, out IntPtr data);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xMat_get_source(IntPtr ptr);

        [DllImport("libwaifu2x.dll", CharSet = CharSet.Unicode)]
        public static extern int Waifu2xMat_load_s(out IntPtr ptr, string path);

        [DllImport("libwaifu2x.dll", CharSet = CharSet.Unicode)]
        public static extern int Waifu2xMat_load_s_a(out IntPtr ptr, byte[] data, int size);

        [DllImport("libwaifu2x.dll", CharSet = CharSet.Unicode)]
        public static extern int Waifu2xMat_save(IntPtr mat, string path, Waifu2xImageFormat format, int quality);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xMat_save_a(out IntPtr ptr, IntPtr mat, ref int length, Waifu2xImageFormat format, int quality);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xModel_create(int tile_size, int pre_padding, byte[] param, byte[] model, bool tta_mode);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2xModel_destroy(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xModel_get_pre_padding(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xModel_get_tile_size(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern bool Waifu2xModel_get_tta_mode(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xModel_get_parameter(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xModel_get_model(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xModel_load_s(out IntPtr ptr, Waifu2xModels model, IntPtr processor, int noise, int scale, bool tta_mode);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xModel_load_s_a(out IntPtr ptr, Waifu2xModels model, int tileSize, int noise, int scale, bool tta_mode);

        [DllImport("libwaifu2x.dll", CharSet = CharSet.Unicode)]
        public static extern int Waifu2xModel_load_s_b(out IntPtr ptr, int tileSize, int prePadding, string param, string model, bool tta_mode);

        [DllImport("libwaifu2x.dll", CharSet = CharSet.Unicode)]
        public static extern int Waifu2xModel_load_s_c(out IntPtr ptr, string src, int tileSize, int prePadding, int noise, int scale, bool tta_mode);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xProcessStatus_create();

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2xProcessStatus_destroy(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xProcessStatus_get_tile_count(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xProcessStatus_get_tile_index(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xProcessStatus_get_target_scale(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xProcessStatus_get_current_scale(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2xProcessStatus_reset(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xProcessor_create(IntPtr device, int index);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2xProcessor_destroy(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xProcessor_get(Waifu2xProcessors proc, int index);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xProcessor_get_s(out IntPtr ptr, Waifu2xProcessors proc, int index);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2xProcessor_get_index(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern Waifu2xProcessors Waifu2xProcessor_get_processer(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xProcessor_get_info(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2xProcessor_get_device(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2x_create(IntPtr processor);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2x_create_a(IntPtr model, IntPtr processor);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2x_destroy(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr Waifu2x_get_model(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern bool Waifu2x_get_no_destroy_model(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern bool Waifu2x_get_no_destroy_processor(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2x_load_model(IntPtr ptr, IntPtr model);

        [DllImport("libwaifu2x.dll")]
        public static extern int Waifu2x_process_s(out IntPtr ptr, IntPtr waifu2x, IntPtr input, int scale, IntPtr processStatus);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2x_set_no_destroy_model(IntPtr ptr, bool value);

        [DllImport("libwaifu2x.dll")]
        public static extern void Waifu2x_set_no_destroy_processor(IntPtr ptr, bool value);

        [DllImport("libwaifu2x.dll")]
        public static extern void destroy_object(IntPtr ptr);

        [DllImport("libwaifu2x.dll")]
        public static extern IntPtr get_error_message(int err);

        [DllImport("libwaifu2x.dll")]
        public static extern int pixel_rgb_to_bgr(IntPtr data, int w, int h, int em_size);

        [DllImport("libwaifu2x.dll")]
        public static extern int pixel_rgb_to_bgr_a(IntPtr data, int w, int h, int em_size, out IntPtr ndata);
    }
}
