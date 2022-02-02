using System;

namespace libwaifu2x {
    /// <summary>
    /// processor
    /// </summary>
    public class Waifu2xProcessor : NativeWrapperObject {
        /// <summary>
        /// internal only
        /// </summary>
        public Waifu2xProcessor(IntPtr handle) : base(handle,
            Waifu2xWrapper.Waifu2xProcessor_destroy) {

        }

        /// <summary>
        /// processor type
        /// </summary>
        public Waifu2xProcessors Processor {
            get => Waifu2xWrapper.Waifu2xProcessor_get_processer(handle);
        }

        /// <summary>
        /// Returns the GPU index, if the processor is CPU, returns -1.
        /// </summary>
        public int Index {
            get => Waifu2xWrapper.Waifu2xProcessor_get_index(handle);
        }

        /// <summary>
        /// GPU only
        /// const ncnn::GpuInfo*
        /// </summary>
        /// <returns>const ncnn::GpuInfo handle</returns>
        public IntPtr GetInfo() {
            return Waifu2xWrapper.Waifu2xProcessor_get_info(handle);
        }

        /// <summary>
        /// GPU only
        /// ncnn::VulkanDevice*
        /// </summary>
        /// <returns>ncnn::VulkanDevice handle</returns>
        public IntPtr GetDevice() {
            return Waifu2xWrapper.Waifu2xProcessor_get_device(handle);
        }

        /// <summary>
        /// Gets the processor.
        /// </summary>
        /// <param name="processors">processor</param>
        /// <param name="index">gpu only If the value is -1, <paramref name="index"/> is selected as the default GPU.</param>
        /// <exception cref="NativeException"></exception>
        public static Waifu2xProcessor Get(Waifu2xProcessors processors = Waifu2xProcessors.AUTO, int index = -1) {
            Waifu2xWrapper.Waifu2xProcessor_get_s(out var ptr, processors, index < 0 ? Waifu2xEnvironment.GetDefaultGPUIndex() : index)
                .EnsureSuccessStatus();
            return new Waifu2xProcessor(ptr);
        }
    }
}
