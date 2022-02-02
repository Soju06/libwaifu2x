#include <stdio.h>
#include <algorithm>
#include <queue>
#include <vector>
#include <clocale>
#include <iostream>
#include <thread>
#include <fs.h>
#include <chrono>

// ncnn
#include "cpu.h"
#include "gpu.h"
#include "platform.h"

#include <waifu2x.h>

#if _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#if _WIN32
#define _printf_a(a)                  printf_s(a)
#define _printf_b(a, b)               printf_s(a, b)
#define _printf_c(a, b, c)            printf_s(a, b, c)
#define _printf_d(a, b, c, d)         printf_s(a, b, c, d)
#define _printf_e(a, b, c, d, e)      printf_s(a, b, c, d, e)
#define _printf_f(a, b, c, d, e, f)   printf_s(a, b, c, d, e, f)
#else
#define _printf_a(a)                  printf(a)
#define _printf_b(a, b)               printf(a, b)
#define _printf_c(a, b, c)            printf(a, b, c)
#define _printf_d(a, b, c, d)         printf(a, b, c, d)
#define _printf_e(a, b, c, d, e)      printf(a, b, c, d, e)
#define _printf_f(a, b, c, d, e, f)   printf(a, b, c, d, e, f)
#endif

#define GET_7TH_ARG(a, b, c, d, e, f, g, ...) g
#define _printf__(...) GET_7TH_ARG(__VA_ARGS__, _printf_f, _printf_e, _printf_d, _printf_c, _printf_b, _printf_a, )
#define _printf(...) _printf__(__VA_ARGS__)(__VA_ARGS__)


bool ERRC(int err, const char* message) {
    if (err) {
        _printf("\n%8x - %s, %s\n", err, get_error_message(err), message);
        return false;
    }
    return true;
}

int main() {
    #if _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif
    {
        int noise   = 2;
        int scale   = 2;
        #if _WIN32
        std::wstring 
        #else
        std::string
        #endif
            inImage, outImage;
        _printf("source image path: ");
        std::getline(
        #if _WIN32
            std::wcin,
        #else
            std::cin,
        #endif 
            inImage);
        _printf("output image path: ");
        std::getline(
        #if _WIN32
            std::wcin,
        #else
            std::cin,
        #endif 
            outImage);
        outImage += PATH(".png");

        _printf("processor: auto\n");
        Waifu2xProcessor* processor = 0;
        if (!ERRC(Waifu2xProcessor::get_s(&processor, Waifu2xProcessors::Auto), "Failed to import processor."))
            return -1;

        _printf("processor type: %s\n", processor->processor == Waifu2xProcessors::Gpu ? "GPU" : "CPU");
        _printf("processor index: %d\n", processor->index);
        _printf("processor info:\n");
        if (processor->processor == Waifu2xProcessors::Gpu) {
            _printf(Waifu2xEnvironment::get_gpu_name(processor->index));
            _printf(" %dMB\n",Waifu2xEnvironment::get_gpu_heap_budget(processor->index));
        } else {
            _printf("just CPU");
        }

        _printf("model info:\nmodel name: CUnet\nnoise level: %d\nscale: %d\n", noise, scale);

        Waifu2xModel* model = 0;
        if (!ERRC(Waifu2xModel::load_s(&model, Waifu2xModels::CUnet, processor, noise, scale), "Failed to load model file."))
            return -1;

        _printf("load model\n");
        Waifu2x waifu2x(processor);
        if (!ERRC(waifu2x.loadModel(model), "Failed to load the model."))
            return -1;

        _printf("open image\n");
        Waifu2xMat* input = 0;
        if (!ERRC(Waifu2xMat::load_s(&input, inImage), "Failed to load image."))
            return -1;

        bool is_success = false;
        Waifu2xProcessStatus status = { };

        _printf("process image\n");
        auto thread = std::thread([&is_success, &status]() {
            int current = -1;
            while (!is_success) {
                if (status.tile_count && current != status.tile_index) {
                    current = status.tile_index;
                    _printf("process status: %d/%d\n", current, status.tile_count);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
        Waifu2xMat* out = 0;
        if (!ERRC(waifu2x.process_s(&out, input, scale, &status), "Failed to process image."))
            return -1;

        is_success = true;
        thread.join();

        _printf("image incoding\n");
        if (!ERRC(out->save(outImage, Waifu2xImageFormat::Png), "Failed to save image."))
            return -1;

        delete out;
        delete input;
    }
    ncnn::destroy_gpu_instance();

    #if _WIN32
    _CrtDumpMemoryLeaks();
    #endif
    return 0;
}