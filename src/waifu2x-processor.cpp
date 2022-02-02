#include "waifu2x.h"
#include "gpu.h"

Waifu2xProcessor::Waifu2xProcessor(ncnn::VulkanDevice* device, int index) {
	if (device) {
		this->processor = Waifu2xProcessors::Gpu;
		this->index = index;
	} else {
		this->processor = Waifu2xProcessors::Cpu;
		this->index = 0;
	}

	this->device = device;
}

Waifu2xProcessor::~Waifu2xProcessor() {
	
}

Waifu2xProcessor* Waifu2xProcessor::get(Waifu2xProcessors proc, int index) {
	Waifu2xProcessor* processor = 0;
	get_s(&processor, proc, index);
	return processor;
}

int Waifu2xProcessor::get_s(Waifu2xProcessor** processor, Waifu2xProcessors proc, int index) {
	if (!processor) return -1;
	if (ncnn::get_gpu_count() < 1 || proc == Waifu2xProcessors::Cpu) *processor = new Waifu2xProcessor(0, -1);
	else {
		if (index < 0) return -2;
 	*processor = new Waifu2xProcessor(ncnn::get_gpu_device(index), index);
	}
	return 0;
}

const ncnn::GpuInfo* Waifu2xProcessor::getInfo() {
	if (device) return &device->info;
	else return 0;
}

ncnn::VulkanDevice* Waifu2xProcessor::getDevice() {
	return device;
}