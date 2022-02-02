#include "waifu2x.h"
#include "waifu2x-wrapper.h"
#include "cpu.h"
#include "gpu.h"
#include "platform.h"
#include <datareader.h>
#include <mutex>

#include "shader/waifu2x_preproc.comp.hex.h"
#include "shader/waifu2x_postproc.comp.hex.h"
#include "shader/waifu2x_preproc_tta.comp.hex.h"
#include "shader/waifu2x_postproc_tta.comp.hex.h"

Waifu2x::Waifu2x(Waifu2xProcessor* processor) {
	this->processor = processor;
}

Waifu2x::Waifu2x(Waifu2xModel* model, Waifu2xProcessor* processor) {
	this->processor = processor;
	loadModel(model);
}

Waifu2x::~Waifu2x() {
    {
        if (!nodes_processor && processor) {
            delete processor;
            processor = 0;
        }
        if (!nodes_model && model) {
            delete model;
            model = 0;
        }
    }
    {
        if (pre_pipe) {
            delete pre_pipe;
            pre_pipe = 0;
        }
        if (post_pipe) {
            delete post_pipe;
            post_pipe = 0;
        }
    }
    if (bicubic_2x) {
        bicubic_2x->destroy_pipeline(net.opt);
        delete bicubic_2x;
        bicubic_2x = 0;
    }
}

int Waifu2x::loadModel(Waifu2xModel* model) {
    if (!model) return -1;
	auto use_vulkan = net.opt.use_vulkan_compute = processor->processor == Waifu2xProcessors::Gpu;
    auto tta_mode = model->ttaMode;

	net.opt.use_fp16_packed = true;
	net.opt.use_fp16_storage = true;
	net.opt.use_fp16_arithmetic = false;
	net.opt.use_int8_storage = true;

	auto* device = processor->getDevice();
	net.set_vulkan_device(device);

    auto* pm = model->getParameter();
    int erroc;

    erroc = net.load_param(*pm);
    delete pm;

    if (erroc) return 0x0000101F;

    auto* md = model->getModel();
    erroc = net.load_model(*md);
    delete md;

    if (erroc) return 0x0000101F;

    // initialize preprocess and postprocess pipeline
    if (use_vulkan) {
        std::vector<ncnn::vk_specialization_type> specializations(1);
        #if _WIN32
        specializations[0].i = 1;
        #else
        specializations[0].i = 0;
        #endif

        #pragma region PreProc
        {
            std::vector<uint32_t> spirv;
            if (tta_mode) {
                if (compile_spirv_module(waifu2x_preproc_tta_comp_data, sizeof(waifu2x_preproc_tta_comp_data), net.opt, spirv))
                    return 0x00001012;
            }
            else {
                if (compile_spirv_module(waifu2x_preproc_comp_data, sizeof(waifu2x_preproc_comp_data), net.opt, spirv))
                    return 0x00001012;
            }

            pre_pipe = new ncnn::Pipeline(device);
            pre_pipe->set_optimal_local_size_xyz(8, 8, 3);
            if (pre_pipe->create(spirv.data(), spirv.size() * 4, specializations))
                return 0x00001019;
            spirv.clear();
        }
        #pragma endregion
        
        #pragma region PostProc
        {
            std::vector<uint32_t> spirv;
            if (tta_mode) {
                if (compile_spirv_module(waifu2x_postproc_tta_comp_data, sizeof(waifu2x_postproc_tta_comp_data), net.opt, spirv))
                    return 0x00001012;
            }
            else {
                if (compile_spirv_module(waifu2x_postproc_comp_data, sizeof(waifu2x_postproc_comp_data), net.opt, spirv))
                    return 0x00001012;
            }

            post_pipe = new ncnn::Pipeline(device);
            post_pipe->set_optimal_local_size_xyz(8, 8, 3);
            if (post_pipe->create(spirv.data(), spirv.size() * 4, specializations))
                return 0x00001019;
            spirv.clear();
        }
        #pragma endregion
    }

    // bicubic 2x for alpha channel
    {
        bicubic_2x = ncnn::create_layer("Interp");
        bicubic_2x->vkdev = device;

        ncnn::ParamDict pd;
        pd.set(0, 3); // bicubic
        pd.set(1, 2.f);
        pd.set(2, 2.f);
        if (bicubic_2x->load_param(pd))
            return 0x0000101F;

        if (bicubic_2x->create_pipeline(net.opt))
            return 0x00001019;
    }

    this->model = model;
    return 0;
}

Waifu2xModel* Waifu2x::getModel() {
    return model;
}
