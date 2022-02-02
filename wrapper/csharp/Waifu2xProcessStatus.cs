using System;

namespace libwaifu2x {
    public class Waifu2xProcessStatus : NativeWrapperObject {
        /// <summary>
        /// internal only
        /// </summary>
        /// <param name="ptr"></param>
        public Waifu2xProcessStatus(IntPtr ptr) : base(ptr,
            Waifu2xWrapper.Waifu2xProcessStatus_destroy) {

        }

        public Waifu2xProcessStatus() : this(create()) {

        }

        /// <summary>
        /// Total number of tiles
        /// </summary>
        public int TileCount {
            get => Waifu2xWrapper.Waifu2xProcessStatus_get_tile_count(handle);
        }

        /// <summary>
        /// Number of processed tiles
        /// </summary>
        public int CurrentTileIndex {
            get => Waifu2xWrapper.Waifu2xProcessStatus_get_tile_index(handle);
        }

        /// <summary>
        /// Number of processed tiles
        /// </summary>
        public int CurrentScale {
            get => Waifu2xWrapper.Waifu2xProcessStatus_get_current_scale(handle);
        }

        /// <summary>
        /// target image scale
        /// </summary>
        public int TargetScale {
            get => Waifu2xWrapper.Waifu2xProcessStatus_get_target_scale(handle);
        }

        /// <summary>
        /// progress
        /// </summary>
        public double CurrentTileProgress {
            get {
                var c = CurrentTileIndex;
                return c == 0 ? 0 : (double)c / TileCount;
            }
        }

        /// <summary>
        /// Resets the value. (Not recommended)
        /// </summary>
        public void Reset() {
            Waifu2xWrapper.Waifu2xProcessStatus_reset(handle);
        }

        internal static IntPtr create() =>
            Waifu2xWrapper.Waifu2xProcessStatus_create();
    }
}
