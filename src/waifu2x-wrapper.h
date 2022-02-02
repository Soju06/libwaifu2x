#pragma once
#include "waifu2x.h"

// Waifu2xProcessor

EXPORT Waifu2xProcessor* Waifu2xProcessor_create(ncnn::VulkanDevice* device, int index) {
	return new Waifu2xProcessor(device, index);
}

EXPORT void Waifu2xProcessor_destroy(Waifu2xProcessor* ptr) {
	delete ptr;
}

EXPORT Waifu2xProcessor* Waifu2xProcessor_get(Waifu2xProcessors proc, int index) {
	return Waifu2xProcessor::get(proc, index);
}

EXPORT int Waifu2xProcessor_get_s(Waifu2xProcessor** processor, Waifu2xProcessors proc, int index) {
	return Waifu2xProcessor::get_s(processor, proc, index);
}

EXPORT Waifu2xProcessors Waifu2xProcessor_get_processer(Waifu2xProcessor* processor) {
	return processor->processor;
}

EXPORT int Waifu2xProcessor_get_index(Waifu2xProcessor* processor) {
	return processor->index;
}

EXPORT const ncnn::GpuInfo* Waifu2xProcessor_get_info(Waifu2xProcessor* processor) {
	return processor->getInfo();
}

EXPORT ncnn::VulkanDevice* Waifu2xProcessor_get_device(Waifu2xProcessor* processor) {
	return processor->getDevice();
}

// Waifu2xMat

EXPORT Waifu2xMat* Waifu2xMat_create(ncnn::Mat* mat, bool nodatarelease) {
	return new Waifu2xMat(mat, nodatarelease);
}

EXPORT void Waifu2xMat_destroy(Waifu2xMat* ptr) {
	delete ptr;
}

EXPORT ncnn::Mat* Waifu2xMat_get_source(Waifu2xMat* mat) {
	return mat->getSource();
}

EXPORT int Waifu2xMat_save(Waifu2xMat* mat, fs::fstr path, Waifu2xImageFormat format, int quality) {
	return mat->save(path, format, quality);
}

EXPORT byte* Waifu2xMat_save_a(int* err, Waifu2xMat* mat, int* length, Waifu2xImageFormat format, int quality) {
	byte* data = 0;
	*err = mat->save(&data, length, format, quality);
	return data;
}

EXPORT Waifu2xMat* Waifu2xMat_load(fs::fstr path) {
	return Waifu2xMat::load(path);
}

EXPORT int Waifu2xMat_load_s(Waifu2xMat** mat, fs::fstr path) {
	return Waifu2xMat::load_s(mat, path);
}

EXPORT Waifu2xMat* Waifu2xMat_load_a(byte* data, int size) {
	return Waifu2xMat::load(data, size);
}

EXPORT int Waifu2xMat_load_s_a(Waifu2xMat** mat, byte* data, int size) {
	return Waifu2xMat::load_s(mat, data, size);
}

EXPORT int Waifu2xMat_get_pixel_data(Waifu2xMat* mat, byte** data) {
	*data = mat->pixelData;
	return mat->height * mat->width * mat->element_size;
}

EXPORT int Waifu2xMat_get_width(Waifu2xMat* mat) {
	return mat->width;
}

EXPORT int Waifu2xMat_get_height(Waifu2xMat* mat) {
	return mat->height;
}

EXPORT int Waifu2xMat_get_element_size(Waifu2xMat* mat) {
	return mat->element_size;
}

// Waifu2xModel

EXPORT Waifu2xModel* Waifu2xModel_create(int tile_size, int pre_padding, const byte* param, const byte* model, bool tta_mode) {
	return new Waifu2xModel(tile_size, pre_padding, param, model, tta_mode);
}

EXPORT void Waifu2xModel_destroy(Waifu2xModel* ptr) {
	delete ptr;
}

EXPORT Waifu2xModel* Waifu2xModel_load(Waifu2xModels model, Waifu2xProcessor* processor, int noise, int scale, bool tta_mode) {
	return Waifu2xModel::load(model, processor, noise, scale, tta_mode);
}

EXPORT Waifu2xModel* Waifu2xModel_load_a(Waifu2xModels model, int tileSize, int noise, int scale, bool tta_mode) {
	return Waifu2xModel::load(model, tileSize, noise, scale, tta_mode);
}

EXPORT Waifu2xModel* Waifu2xModel_load_b(int prePadding, int tileSize, fs::fstr param, fs::fstr model, bool tta_mode) {
	return Waifu2xModel::load(prePadding, tileSize, param, model, tta_mode);
}

EXPORT Waifu2xModel* Waifu2xModel_load_c(fs::fstr src, int prePadding, int tileSize, int noise, int scale, bool tta_mode) {
	return Waifu2xModel::load(src, prePadding, tileSize, noise, scale, tta_mode);
}

EXPORT int Waifu2xModel_load_s(Waifu2xModel** _model, Waifu2xModels model, Waifu2xProcessor* processor, int noise, int scale, bool tta_mode) {
	return Waifu2xModel::load_s(_model, model, processor, noise, scale, tta_mode);
}

EXPORT int Waifu2xModel_load_s_a(Waifu2xModel** _model, Waifu2xModels model, int tileSize, int noise, int scale, bool tta_mode) {
	return Waifu2xModel::load_s(_model, model, tileSize, noise, scale, tta_mode);
}

EXPORT int Waifu2xModel_load_s_b(Waifu2xModel** _model, int tileSize, int prePadding, fs::fstr param, fs::fstr model, bool tta_mode) {
	return Waifu2xModel::load_s(_model, tileSize, prePadding, param, model, tta_mode);
}

EXPORT int Waifu2xModel_load_s_c(Waifu2xModel** _model, fs::fstr src, int tileSize, int prePadding, int noise, int scale, bool tta_mode) {
	return Waifu2xModel::load_s(_model, src, tileSize, prePadding, noise, scale, tta_mode);
}

EXPORT int Waifu2xModel_get_pre_padding(Waifu2xModel* model) {
	return model->prePadding;
}

EXPORT int Waifu2xModel_get_tile_size(Waifu2xModel* model) {
	return model->tileSize;
}

EXPORT bool Waifu2xModel_get_tta_mode(Waifu2xModel* model) {
	return model->ttaMode;
}

EXPORT ncnn::DataReader* Waifu2xModel_get_parameter(Waifu2xModel* model) {
	return model->getParameter();
}

EXPORT ncnn::DataReader* Waifu2xModel_get_model(Waifu2xModel* model) {
	return model->getModel();
}

// Waifu2xFileModel

EXPORT Waifu2xModel* Waifu2xFileModel_load(int tile_size, int pre_padding, fs::fstr param, fs::fstr model, bool tta_mode = false) {
	return Waifu2xFileModel::load(tile_size, pre_padding, param, model, tta_mode);
}

EXPORT int Waifu2xFileModel_load_s(Waifu2xModel** _model, int tile_size, int pre_padding, fs::fstr param, fs::fstr model, bool tta_mode = false) {
	return Waifu2xFileModel::load_s(_model, tile_size, pre_padding, param, model, tta_mode);
}

// Waifu2xProcessStatus

EXPORT Waifu2xProcessStatus* Waifu2xProcessStatus_create() {
	return new Waifu2xProcessStatus();
}

EXPORT void Waifu2xProcessStatus_destroy(Waifu2xProcessStatus* ptr) {
	delete ptr;
}

EXPORT int Waifu2xProcessStatus_get_tile_count(Waifu2xProcessStatus* status) {
	return status->tile_count;
}

EXPORT int Waifu2xProcessStatus_get_tile_index(Waifu2xProcessStatus* status) {
	return status->tile_index;
}

EXPORT int Waifu2xProcessStatus_get_target_scale(Waifu2xProcessStatus* status) {
	return status->target_scale;
}

EXPORT int Waifu2xProcessStatus_get_current_scale(Waifu2xProcessStatus* status) {
	return status->current_scale;
}

EXPORT void Waifu2xProcessStatus_reset(Waifu2xProcessStatus* status) {
	status->tile_count = 0;
	status->tile_index = 0;
	status->target_scale = 0;
	status->current_scale = 0;
}

// Waifu2x

EXPORT Waifu2x* Waifu2x_create(Waifu2xProcessor* processor) {
	return new Waifu2x(processor);
}

EXPORT Waifu2x* Waifu2x_create_a(Waifu2xModel* model, Waifu2xProcessor* processor) {
	return new Waifu2x(model, processor);
}

EXPORT void Waifu2x_destroy(Waifu2x* ptr) {
	delete ptr;
}

EXPORT int Waifu2x_load_model(Waifu2x* waifu2x, Waifu2xModel* model) {
	return waifu2x->loadModel(model);
}

EXPORT Waifu2xModel* Waifu2x_get_model(Waifu2x* waifu2x) {
	return waifu2x->getModel();
}

EXPORT Waifu2xMat* Waifu2x_process(Waifu2x* waifu2x, Waifu2xMat* input, int scale, Waifu2xProcessStatus* processStatus) {
	return waifu2x->process(input, scale, processStatus);
}

EXPORT int Waifu2x_process_s(Waifu2xMat** mat, Waifu2x* waifu2x, Waifu2xMat* input, int scale, Waifu2xProcessStatus* processStatus) {
	return waifu2x->process_s(mat, input, scale, processStatus);
}

EXPORT bool Waifu2x_get_no_destroy_processor(Waifu2x* waifu2x) {
	return waifu2x->nodes_processor;
}

EXPORT bool Waifu2x_get_no_destroy_model(Waifu2x* waifu2x) {
	return waifu2x->nodes_model;
}

EXPORT void Waifu2x_set_no_destroy_processor(Waifu2x* waifu2x, bool value) {
	waifu2x->nodes_processor = value;
}

EXPORT void Waifu2x_set_no_destroy_model(Waifu2x* waifu2x, bool value) {
	waifu2x->nodes_model = value;
}

// Waifu2xEnvironment

EXPORT bool Waifu2xEnvironment_is_support_gpu() {
	return Waifu2xEnvironment::is_support_gpu();
}

EXPORT int Waifu2xEnvironment_get_gpu_count() {
	return Waifu2xEnvironment::get_gpu_count();
}

EXPORT const char* Waifu2xEnvironment_get_gpu_name(int index) {
	return Waifu2xEnvironment::get_gpu_name(index);
}

EXPORT unsigned int Waifu2xEnvironment_get_gpu_heap_budget(int index) {
	return Waifu2xEnvironment::get_gpu_heap_budget(index);
}

EXPORT unsigned int Waifu2xEnvironment_get_device_id(int index) {
	return Waifu2xEnvironment::get_device_id(index);
}

EXPORT unsigned int Waifu2xEnvironment_get_vendor_id(int index) {
	return Waifu2xEnvironment::get_vendor_id(index);
}

EXPORT void Waifu2xEnvironment_ncnn_destroy_gpu_instance() {
	Waifu2xEnvironment::ncnn_destroy_gpu_instance();
}

EXPORT int Waifu2xEnvironment_get_default_gpu_index() {
	return Waifu2xEnvironment::get_default_gpu_index();
}

EXPORT void destroy_object(void* obj) {
	delete obj;
}

EXPORT void pixel_rgb_to_bgr(byte* data, int w, int h, int em_size) {
	if (em_size < 3 || em_size > 4) return;
	for (int y = 0; y < h; y++) {
		byte* ydata = data + y * w * em_size;
		for (int x = 0; x < w; x++) {
			byte* xdata = ydata + x * em_size;
			byte r = xdata[0];
			xdata[0] = xdata[2];
			xdata[2] = r;
		}
	}
}

EXPORT int pixel_rgb_to_bgr_a(byte* data, int w, int h, int em_size, byte** ndata) {
	int size = w * h * em_size;
	auto* _data = (byte*)malloc(size);

	if (em_size >= 3 && em_size <= 4) {
		for (int y = 0; y < h; y++) {
			byte* ydata = data + y * w * em_size;
			byte* _ydata = _data + y * w * em_size;
			for (int x = 0; x < w; x++) {
				byte* xdata = ydata + x * em_size;
				byte* _xdata = _ydata + x * em_size;

				_xdata[0] = xdata[2];
				_xdata[1] = xdata[1];
				_xdata[2] = xdata[0];
				if (em_size == 4) _xdata[3] = xdata[3];
			}
		}
	}

	*ndata = _data;
	return size;
}