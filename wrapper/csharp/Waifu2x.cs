using System;

namespace libwaifu2x {
    public class Waifu2x : NativeWrapperObject {
        protected Waifu2xProcessor? processor;
        protected Waifu2xModel? model;

        /// <summary>
        /// internal only
        /// </summary>
        public Waifu2x(IntPtr ptr) : base(ptr,
            Waifu2xWrapper.Waifu2x_destroy) {

        }

        public Waifu2x(Waifu2xProcessor processor) : this(create(processor)) {
            // GC automatically destroys.
            Waifu2xWrapper.Waifu2x_set_no_destroy_processor(handle, true);
            this.processor = processor;
        }

        /// <summary>
        /// waifu2x model
        /// </summary>
        /// <exception cref="ArgumentNullException"></exception>
        /// <exception cref="NativeException"></exception>
        public Waifu2xModel? Model {
            get => model != null && GetModel() != (IntPtr)0 ? model : null;
            set {
                if(value == null) throw new ArgumentNullException(nameof(value));
                LoadModel(value);
            }
        }

        /// <summary>
        /// internal only
        /// </summary>
        /// <param name="model">waifu2x model</param>
        /// <exception cref="NativeException"></exception>
        public void LoadModel(IntPtr model) {
            Waifu2xWrapper.Waifu2x_load_model(handle, model).EnsureSuccessStatus();
        }

        public void LoadModel(Waifu2xModel value) {
            LoadModel(value.Handle);
            // GC automatically destroys.
            Waifu2xWrapper.Waifu2x_set_no_destroy_model(handle, true);
            model = value;
        }

        /// <summary>
        /// internal only
        /// </summary>
        /// <returns>If the return value is 0, the model is not loaded.</returns>
        public IntPtr GetModel() {
            return Waifu2xWrapper.Waifu2x_get_model(handle);
        }

        /// <summary>
        /// 이미지를 프로세스합니다.
        /// </summary>
        /// <param name="input">original image</param>
        /// <param name="scale">scale (only multiple of 2)</param>
        /// <param name="status">status</param>
        /// <exception cref="NativeException"></exception>
        public Waifu2xMat Process(Waifu2xMat input, int scale, Waifu2xProcessStatus? status = null) {
            Waifu2xWrapper.Waifu2x_process_s(out var ptr, handle, input.Handle, scale, status == null ? (IntPtr)0 : status.Handle)
                .EnsureSuccessStatus();
            return new Waifu2xMat(ptr);
        }

        /// <summary>
        /// get waifu2x
        /// </summary>
        internal static IntPtr create(Waifu2xProcessor processor) =>
            Waifu2xWrapper.Waifu2x_create(processor.Handle);
    }
}
