namespace libwaifu2x {
    /// <summary>
    /// waifu2x environment
    /// </summary>
    public static class Waifu2xEnvironment {
        /// <summary>
        /// GPU support
        /// </summary>
        public static bool IsSupportGPU() => Waifu2xWrapper.Waifu2xEnvironment_is_support_gpu();

        /// <summary>
        /// Gets the GPU number.
        /// </summary>
        public static int GetGPUCount() => Waifu2xWrapper.Waifu2xEnvironment_get_gpu_count();

        /// <summary>
        /// Gets the name of the GPU.
        /// </summary>
        public static string GetGPUName(int index) => Waifu2xWrapper.Waifu2xEnvironment_get_gpu_name(index).PtrToStringAnsi();

        /// <summary>
        /// https://ncnn.docsforge.com/master/api/ncnn/ncnn_destroy_gpu_instance/
        /// </summary>
        public static void NCNNDestroyGPUInstance() => Waifu2xWrapper.Waifu2xEnvironment_ncnn_destroy_gpu_instance();

        /// <summary>
        /// current gpu heap memory budget in MB
        /// </summary>
        public static uint GetGPUHeapBudget(int index) => Waifu2xWrapper.Waifu2xEnvironment_get_gpu_heap_budget(index);

        /// <summary>
        /// device id
        /// </summary>
        public static uint GetDeviceId(int index) => Waifu2xWrapper.Waifu2xEnvironment_get_device_id(index);

        /// <summary>
        /// vendor id
        /// </summary>
        public static uint GetVendorId(int index) => Waifu2xWrapper.Waifu2xEnvironment_get_vendor_id(index);

        /// <summary>
        /// default gpu index
        /// </summary>
        public static int GetDefaultGPUIndex() => Waifu2xWrapper.Waifu2xEnvironment_get_default_gpu_index();
    }
}
