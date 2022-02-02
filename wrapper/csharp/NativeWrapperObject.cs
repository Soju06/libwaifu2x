using System;

namespace libwaifu2x {
    public abstract class NativeWrapperObject : IDisposable {
        protected bool disposedValue, destory_handle = true, throw_destory_error = false;
        protected IntPtr handle;
        protected Action<IntPtr> destory_func;

        public NativeWrapperObject() {
            destory_func = _destory_func;
        }

        public NativeWrapperObject(IntPtr ptr, Action<IntPtr> destoryFunc) {
            if (ptr == IntPtr.Zero) throw new PointerIsZeroException(nameof(ptr));
            handle = ptr;
            destory_func = destoryFunc ?? throw new ArgumentNullException(nameof(destoryFunc));
        }

        /// <summary>
        /// object pointer
        /// </summary>
        public IntPtr Handle {
            get => handle;
        }

        protected virtual void Dispose(bool disposing) {
            if (!disposedValue) {
                disposedValue = true;
                if (destory_handle) {
                    try {
                        if (handle != IntPtr.Zero) destory_func(handle);
                    } catch (Exception ex) {
                        if (throw_destory_error) throw ex;
                    }
                }
            }
        }

        static void _destory_func(IntPtr ptr) {
            Waifu2xWrapper.destroy_object(ptr);
        }

        public static explicit operator IntPtr(NativeWrapperObject obj) => obj.Handle;

        ~NativeWrapperObject() {
            Dispose(disposing: false);
        }

        public void Dispose() {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
    }
}
