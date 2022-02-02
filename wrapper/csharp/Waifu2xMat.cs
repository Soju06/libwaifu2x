using System;
using System.Runtime.InteropServices;

namespace libwaifu2x {
    /// <summary>
    /// waifu2x mat
    /// </summary>
    public class Waifu2xMat : NativeWrapperObject {
        public Waifu2xMat(IntPtr ptr) : base(ptr, 
            Waifu2xWrapper.Waifu2xMat_destroy) {

        }

        /// <summary>
        /// image width
        /// </summary>
        public int Width {
            get => Waifu2xWrapper.Waifu2xMat_get_width(handle);
        }

        /// <summary>
        /// image height
        /// </summary>
        public int Height {
            get => Waifu2xWrapper.Waifu2xMat_get_height(handle);
        }

        public int ElementSize {
            get => Waifu2xWrapper.Waifu2xMat_get_element_size(handle);
        }

        public IntPtr GetSource() { 
            return Waifu2xWrapper.Waifu2xMat_get_source(handle);
        }

        /// <summary>
        /// Copy the pixel data.
        /// </summary>
        /// <returns></returns>
        public byte[] GetPixelData(bool swap_bgr) {
            var size = Waifu2xWrapper.Waifu2xMat_get_pixel_data(handle, out var ptr);
            if (swap_bgr) size = Waifu2xWrapper.pixel_rgb_to_bgr_a(ptr, Width, Height, ElementSize, out ptr);
            var sdata = new byte[size];
            Marshal.Copy(ptr, sdata, 0, size);
            if (swap_bgr) Waifu2xWrapper.destroy_object(ptr);
            return sdata;
        }

        /// <summary>
        /// Save as a file
        /// </summary>
        /// <param name="path">path</param>
        /// <param name="format">image format</param>
        /// <param name="quality">quality" is only used in jpg format.</param>
        public void Save(string path, Waifu2xImageFormat format, int quality = 100) {
            Waifu2xWrapper.Waifu2xMat_save(handle, path, format, quality).EnsureSuccessStatus();
        }

        /// <summary>
        /// Save to memory
        /// </summary>
        /// <param name="format">image foramt</param>
        /// <param name="quality">quality" is only used in jpg format.</param>
        /// <returns></returns>
        public byte[] Save(Waifu2xImageFormat format, int quality) {
            int size = 0;
            Waifu2xWrapper.Waifu2xMat_save_a(out var ptr, handle, ref size, format, quality)
                .EnsureSuccessStatus();
            var sdata = new byte[size];
            Marshal.Copy(ptr, sdata, 0, size);
            return sdata;
        }

        /// <summary>
        /// load the image
        /// </summary>
        /// <exception cref="NativeException"></exception>
        public static Waifu2xMat Load(string path) {
            Waifu2xWrapper.Waifu2xMat_load_s(out var ptr, path)
                .EnsureSuccessStatus();
            return new Waifu2xMat(ptr);
        }

        /// <summary>
        /// load the image
        /// </summary>
        /// <exception cref="NativeException"></exception>
        public static Waifu2xMat Load(byte[] data) {
            Waifu2xWrapper.Waifu2xMat_load_s_a(out var ptr, data, data.Length)
                .EnsureSuccessStatus();
            return new Waifu2xMat(ptr);
        }
    }
}
