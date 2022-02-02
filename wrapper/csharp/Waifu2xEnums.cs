namespace libwaifu2x {
	/// <summary>
	/// waifu2x processor
	/// </summary>
    public enum Waifu2xProcessors {
		/// <summary>
		/// CPU
		/// </summary>
		CPU = -1,
		/// <summary>
		/// AUTO
		/// </summary>
		AUTO = 0,
		/// <summary>
		/// GPU
		/// </summary>
		GPU = 1
	}

	/// <summary>
	/// waifu2x model
	/// </summary>
	public enum Waifu2xModels {
		CUnet = 2,
		UpConv7Anime = 12,
		UpConv7Photo = 14
	}

	/// <summary>
	/// waifu2x Image format
	/// </summary>
	public enum Waifu2xImageFormat {
		Png = 0,
		Jpg = 2
	}
}
