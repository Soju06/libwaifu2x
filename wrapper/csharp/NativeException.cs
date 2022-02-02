using System;

namespace libwaifu2x {
    [Serializable]
    public class NativeException : Exception {
        public NativeException(string message) : base(message) { }
        public NativeException(string message, Exception inner) : base(message, inner) { }
        public NativeException(int errorcode) : base(try_get_error_code(errorcode)) { }

        internal static string try_get_error_code(int err) {
            try {
                return Waifu2xWrapper.get_error_message(err).PtrToStringAnsi();
            } catch (Exception ex) {
                return $"Failed to get an error message. | {ex.Message} | error code: {err}";
            }
        }
    }

    public static class NativeExceptionStatic {
        public static void EnsureSuccessStatus(this int err) {
            if (err == 0) return;
            throw new NativeException(err);
        }
    }

    [Serializable]
    public class PointerIsZeroException : ArgumentException {
        public PointerIsZeroException() { }
        public PointerIsZeroException(string param) : base("pointer is zero", param) { }
        public PointerIsZeroException(string message, string param) : base(message, param) { }
    }
}
