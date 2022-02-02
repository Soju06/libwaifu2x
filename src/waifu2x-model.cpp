#include "waifu2x.h"
#include "waifu2x-model-utility.h"
#include "cpu.h"
#include "gpu.h"
#include "platform.h"
#include <datareader.h>
#include "fs.h"

Waifu2xModel::Waifu2xModel(int tile_size, int pre_padding, const byte* param, const byte* model, bool tta_mode) {
	prePadding = pre_padding;
	tileSize = tile_size;
	ttaMode = tta_mode;
	this->param = param;
	this->model = model;
}

Waifu2xModel::~Waifu2xModel() {
	delete param;
	delete model;
}

Waifu2xModel* Waifu2xModel::load(Waifu2xModels model, Waifu2xProcessor* processor, int noise, int scale, bool tta_mode) {
	return load(
		model,
		wut::getTileSize(
			processor->getDevice(),
			model
		),
		noise,
		scale,
		tta_mode
	);
}

Waifu2xModel* Waifu2xModel::load(Waifu2xModels model, int tileSize, int noise, int scale, bool tta_mode) {
	return load(
		fs::executable_directory() / std::filesystem::path("models") / wut::getModelName(model),
		tileSize,
		wut::getPrePadding(model, noise, scale), 
		noise, 
		scale, 
		tta_mode
	);
}

Waifu2xModel* Waifu2xModel::load(int tileSize, int prePadding, fs::path param, fs::path model, bool tta_mode) {
	return Waifu2xFileModel::load(
		tileSize,
		prePadding,
		param,
		model,
		tta_mode
	);
}

Waifu2xModel* Waifu2xModel::load(fs::path src, int tileSize, int prePadding, int noise, int scale, bool tta_mode) {
	auto [param_n, model_n] = wut::getModelName(src, noise, scale);
	return Waifu2xFileModel::load(
		tileSize,
		prePadding,
		param_n,
		model_n,
		tta_mode
	);
}


int Waifu2xModel::load_s(Waifu2xModel** _model, Waifu2xModels model, Waifu2xProcessor* processor, int noise, int scale, bool tta_mode) {
	if (!_model || !processor) return -1;
	return load_s(
		_model,
		model,
		wut::getTileSize(
			processor->getDevice(),
			model
		), 
		noise,
		scale,
		tta_mode
	);
}

int Waifu2xModel::load_s(Waifu2xModel** _model, Waifu2xModels model, int tileSize, int noise, int scale, bool tta_mode) {
	if (!_model) return -1;
	return load_s(
		_model,
		fs::executable_directory() / std::filesystem::path("models") / wut::getModelName(model),
		tileSize,
		wut::getPrePadding(model, noise, scale),
		noise,
		scale,
		tta_mode
	);
}

int Waifu2xModel::load_s(Waifu2xModel** _model, int tileSize, int prePadding, fs::path param, fs::path model, bool tta_mode) {
	if (!_model) return -1;
	return Waifu2xFileModel::load_s(
		_model,
		tileSize,
		prePadding,
		param,
		model,
		tta_mode
	);
}

int Waifu2xModel::load_s(Waifu2xModel** _model, fs::path src, int tileSize, int prePadding, int noise, int scale, bool tta_mode) {
	if (!_model) return -1;
	auto [param_n, model_n] = wut::getModelName(src, noise, scale);
	return Waifu2xFileModel::load_s(
		_model,
		tileSize,
		prePadding,
		param_n,
		model_n,
		tta_mode
	);
}

ncnn::DataReader* Waifu2xModel::getParameter() {
	ncnn::DataReaderFromMemory* r = new ncnn::DataReaderFromMemory(param);
	return r;
}

ncnn::DataReader* Waifu2xModel::getModel() {
	ncnn::DataReaderFromMemory* r = new ncnn::DataReaderFromMemory(model);
	return r;
}

Waifu2xFileModel::Waifu2xFileModel(int tile_size, int pre_padding, fs::File* param_file, fs::File* model_file, bool tta_mode)
	: Waifu2xModel(tile_size, pre_padding, NULL, NULL, tta_mode) {
	this->param_file = param_file;
	this->model_file = model_file;
}

Waifu2xFileModel::~Waifu2xFileModel() {
	if (param_file) delete param_file;
	if (model_file) delete model_file;
}

Waifu2xModel* Waifu2xFileModel::load(int tile_size, int pre_padding, fs::path param, fs::path model, bool tta_mode) {
	Waifu2xModel* _model = 0;
	load_s(&_model, tile_size, pre_padding, param, model, tta_mode);
	return _model;
}

int Waifu2xFileModel::load_s(Waifu2xModel** _model, int tile_size, int pre_padding, fs::path param, fs::path model, bool tta_mode) {
	if (!_model) return -1;
	fs::File* pf, * mf;

	int err = fs::open_s(&pf, param, PATH("rb"));
	if (err) return 0x000013C0;
	err = fs::open_s(&mf, model, PATH("rb"));
	if (err) return 0x000013C0;

	*_model = new Waifu2xFileModel(tile_size, pre_padding, pf, mf, tta_mode);
	return 0;
}

ncnn::DataReader* Waifu2xFileModel::getParameter() {
	return new ncnn::DataReaderFromStdio(param_file->get());
}


ncnn::DataReader* Waifu2xFileModel::getModel() {
	return new ncnn::DataReaderFromStdio(model_file->get());
}