#include <stdio.h>
#include <string>
#include <gpu.h>
#include "waifu2x.h"
#include <stdexcept>

bool Waifu2xEnvironment::is_support_gpu() {
    return ncnn::get_gpu_count();
}

int Waifu2xEnvironment::get_gpu_count() {
    return ncnn::get_gpu_count();
}

int Waifu2xEnvironment::get_default_gpu_index() {
    return ncnn::get_default_gpu_index();
}

const char* Waifu2xEnvironment::get_gpu_name(int index) {
    return ncnn::get_gpu_info(index).device_name();
}

unsigned int Waifu2xEnvironment::get_gpu_heap_budget(int index) {
    return ncnn::get_gpu_device(index)->get_heap_budget();
}

unsigned int Waifu2xEnvironment::get_device_id(int index) {
    return ncnn::get_gpu_info(index).device_id();
}

unsigned int Waifu2xEnvironment::get_vendor_id(int index) {
    return ncnn::get_gpu_info(index).vendor_id();
}

void Waifu2xEnvironment::ncnn_destroy_gpu_instance() {
    ncnn::destroy_gpu_instance();
}
