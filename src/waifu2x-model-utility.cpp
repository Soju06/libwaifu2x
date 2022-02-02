#include "waifu2x-model-utility.h"
#include <tuple>

namespace wut {
    int getPrePadding(Waifu2xModels model, int noise, int scale) {
        switch (model) {
        case Waifu2xModels::CUnet:
            return noise == -1 || scale > 1 ? 18 : 28;
        case Waifu2xModels::UpConv7Anime:
        case Waifu2xModels::UpConv7Photo:
        default: return 7;
        }
    }

    int getTileSize(ncnn::VulkanDevice* device, Waifu2xModels model) {
        if (!device) return 4000;

        uint32_t heap_budget = device->get_heap_budget();

        switch (model) {
        case Waifu2xModels::CUnet:
            if (heap_budget > 2600)
                return 400;
            else if (heap_budget > 740)
                return 200;
            else if (heap_budget > 250)
                return 100;
            else
                return 32;
        case Waifu2xModels::UpConv7Anime:
        case Waifu2xModels::UpConv7Photo:
            if (heap_budget > 1900)
                return 400;
            else if (heap_budget > 550)
                return 200;
            else if (heap_budget > 190)
                return 100;
            else
                return 32;
            break;
        default: return 400;
        }
    }

    fs::fstr getModelName(Waifu2xModels model) {
        switch (model) {
        case Waifu2xModels::CUnet:
        _DEFAULT:
            return PATH("models-cunet");
        case Waifu2xModels::UpConv7Anime:
            return PATH("models-upconv_7_anime_style_art_rgb");
        case Waifu2xModels::UpConv7Photo:
            return PATH("models-upconv_7_photo");
        default: goto _DEFAULT;
        }
    }

    std::tuple<fs::path, fs::path> getModelName(fs::path src, int noise, int scale) {
        fs::path param_name, model_name;
        if (noise == -1) {
            param_name = PATH("scale2.0x_model.param");
            model_name = PATH("scale2.0x_model.bin");
        } else {
            //param_name = std::format(PATH("noise{}{}_model."), noise, scale <= 1 ? "" : "_scale2.0x");
            param_name = string_format(PATH("noise%d%s_model."), noise, scale <= 1 ? PATH("") : PATH("_scale2.0x"));
            model_name = param_name + PATH("bin");
            param_name = param_name + PATH("param");
        }

        return std::make_tuple(fs::combine(src, param_name), fs::combine(src, model_name));
    }

    std::tuple<fs::path, fs::path> getModelName(fs::path dir, Waifu2xModels model, int noise, int scale) {
        return getModelName(fs::combine(dir, getModelName(model)), noise, scale);
    }

    int createMat(ncnn::Mat** o_mat, ncnn::Mat* i_mat, int scale) {
        if (!o_mat || !i_mat) return -1;
        if (scale <= 0) return -2;
        *o_mat = new ncnn::Mat(i_mat->w * scale, i_mat->h * scale, (size_t)i_mat->elemsize, (int)i_mat->elemsize);
        return 0;
    }
}