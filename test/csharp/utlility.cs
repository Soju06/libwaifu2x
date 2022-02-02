using System;
using System.Collections.Generic;

namespace waifu2x.test {
    internal static class Utlility {
        public static T[] Repeat<T>(this int num, Func<int, T> func) {
            var ls = new List<T>();
            for (int i = 0; i < num; i++) {
                ls.Add(func.Invoke(i));
            }
            return ls.ToArray();
        }
    }
}
