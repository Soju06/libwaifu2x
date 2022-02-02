#pragma once
#include "waifu2x.h"

namespace wut {
	int getPrePadding(Waifu2xModels model, int noise, int scale);
	int getTileSize(ncnn::VulkanDevice* device, Waifu2xModels model);
	fs::fstr getModelName(Waifu2xModels model);
	std::tuple<fs::path, fs::path> getModelName(fs::path src, int noise, int scale);
	std::tuple<fs::path, fs::path> getModelName(fs::path dir, Waifu2xModels model, int noise, int scale);
	int createMat(ncnn::Mat** o_mat, ncnn::Mat* i_mat, int scale);
}
