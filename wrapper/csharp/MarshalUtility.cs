using System;
using System.Runtime.InteropServices;

namespace libwaifu2x {
    internal static class MarshalUtility {
        public static string PtrToStringAnsi(this IntPtr ptr) => Marshal.PtrToStringAnsi(ptr);
        public static string PtrToStringAuto(this IntPtr ptr) => Marshal.PtrToStringAuto(ptr);
        public static string PtrToStringBSTR(this IntPtr ptr) => Marshal.PtrToStringBSTR(ptr);
        public static string PtrToStringUni(this IntPtr ptr) => Marshal.PtrToStringUni(ptr);
        public static string PtrToStringUTF8(this IntPtr ptr) => Marshal.PtrToStringUTF8(ptr);
    }
}
