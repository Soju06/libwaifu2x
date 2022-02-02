#pragma once

#include <stdio.h>
#include <string>
#include "fs.h"
#include <net.h>

#if _WIN32
#define _EXPORT __declspec(dllexport)
#define EXPORT extern "C" __declspec(dllexport)
#else
#define _EXPORT __attribute__((visibility("default")))
#define EXPORT extern "C" __attribute__((visibility("default")))
#endif

typedef unsigned char byte;

// get error message
EXPORT const char* get_error_message(int error_code);

template<typename ... Args>
#if _WIN32
std::wstring
#else
std::string
#endif
 string_format(const
	 #if _WIN32
	 std::wstring
	 #else
	 std::string
	 #endif
	 & format, Args ... args) {
	int csize = sizeof(
		#if _WIN32
		wchar_t
		#else
		char
		#endif	
		);
	int size_s = 
		#if _WIN32
		std::swprintf
		#else
		std::snprintf
		#endif
		(nullptr, 0, format.c_str(), args ...) + csize; // Extra space for '\0'
	if (size_s < csize) return PATH("");
	auto size = static_cast<size_t>(size_s);
	auto buf = std::make_unique<
		#if _WIN32
		wchar_t
		#else
		char
		#endif	
		[]>(size);
	#if _WIN32
	std::swprintf
		#else
	std::snprintf
		#endif
	(buf.get(), size, format.c_str(), args ...);
	return 
		#if _WIN32
		std::wstring
		#else
		std::string
		#endif
		(buf.get(), buf.get() + size - csize); // We don't want the '\0' inside
}

template<typename ... Args>
std::string _string_format(const std::string& format, Args ... args) {
	int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) return "";
	auto size = static_cast<size_t>(size_s);
	auto buf = std::make_unique<char[]>(size);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

// waifu2x processor
enum class _EXPORT Waifu2xProcessors {
	Cpu = -1,
	Auto = 0,
	Gpu = 1
};

// waifu2x default models
enum class _EXPORT Waifu2xModels {
	CUnet = 2,
	UpConv7Anime = 12,
	UpConv7Photo = 14
};

// waifu2x Image format
enum class _EXPORT Waifu2xImageFormat {
	Png = 0,
	Jpg = 2
};

// waifu2x processer
class _EXPORT Waifu2xProcessor {
public:
	Waifu2xProcessor(ncnn::VulkanDevice* device, int index = -1);
	~Waifu2xProcessor();

	static Waifu2xProcessor* get(Waifu2xProcessors proc = Waifu2xProcessors::Auto, int index = 0);
	static int get_s(Waifu2xProcessor** processor, Waifu2xProcessors proc = Waifu2xProcessors::Auto, int index = 0);
	
	Waifu2xProcessors processor;
	int index;

	// gpu only
	const ncnn::GpuInfo* getInfo();
	// gpu only
	ncnn::VulkanDevice* getDevice();
private:
	ncnn::VulkanDevice* device;
};

// waifu2x image mat
class _EXPORT Waifu2xMat {
public:
	Waifu2xMat(ncnn::Mat* mat, bool nodatarelease = true);
	~Waifu2xMat();

	ncnn::Mat* getSource();
	// "quality" is only used in jpg format.
	int save(fs::path path, Waifu2xImageFormat format, int quality = 100);
	// "quality" is only used in jpg format.
	int save(byte** data, int* length, Waifu2xImageFormat format, int quality = 100);

	int save_png(byte** data, int* length);
	int save_png(fs::path path);
	int save_png(fs::File* file);
	int save_jpg(byte** data, int* length, int quality = 100);
	int save_jpg(fs::path path, int quality = 100);
	int save_jpg(fs::File* file, int quality = 100);

	static Waifu2xMat* load(fs::path path);
	static Waifu2xMat* load(fs::File* file);
	static Waifu2xMat* load(byte* data, int size);
	static int load_s(Waifu2xMat** mat, fs::path path);
	static int load_s(Waifu2xMat** mat, fs::File* file);
	static int load_s(Waifu2xMat** mat, byte* data, int size);

	byte* pixelData;
	int width, height, element_size;
	bool _norelease = false, _nodatarelease = true;
private:
	ncnn::Mat* source;
};


// waifu2x image model
class _EXPORT Waifu2xModel {
public:
 	Waifu2xModel(int tile_size, int pre_padding, const byte* param, const byte* model, bool tta_mode = true);
	virtual ~Waifu2xModel();

	static Waifu2xModel* load(Waifu2xModels model, Waifu2xProcessor* processor, int noise = 0, int scale = 2, bool tta_mode = false);
	static Waifu2xModel* load(Waifu2xModels model, int tileSize, int noise = 0, int scale = 2, bool tta_mode = false);
	static Waifu2xModel* load(int prePadding, int tileSize, fs::path param, fs::path model, bool tta_mode = false);
	static Waifu2xModel* load(fs::path src, int prePadding, int tileSize, int noise = 0, int scale = 2, bool tta_mode = false);

	static int load_s(Waifu2xModel** _model, Waifu2xModels model, Waifu2xProcessor* processor, int noise = 0, int scale = 2, bool tta_mode = false);
	static int load_s(Waifu2xModel** _model, Waifu2xModels model, int tileSize, int noise = 0, int scale = 2, bool tta_mode = false);
	static int load_s(Waifu2xModel** _model, int tileSize, int prePadding, fs::path param, fs::path model, bool tta_mode = false);
	static int load_s(Waifu2xModel** _model, fs::path src, int tileSize, int prePadding, int noise = 0, int scale = 2, bool tta_mode = false);

 	int prePadding;
	int tileSize;
	bool ttaMode;

	virtual ncnn::DataReader* getParameter();
	virtual ncnn::DataReader* getModel();
private:
	const byte* param;
	const byte* model;
};

class _EXPORT Waifu2xFileModel : public Waifu2xModel {
public:
	explicit Waifu2xFileModel(int tile_size, int pre_padding, fs::File* param_file, fs::File* model_file, bool tta_mode = false);
	~Waifu2xFileModel();

	static Waifu2xModel* load(int tile_size, int pre_padding, fs::path param, fs::path model, bool tta_mode = false);
	static int load_s(Waifu2xModel** _model, int tile_size, int pre_padding, fs::path param, fs::path model, bool tta_mode = false);
	virtual ncnn::DataReader* getParameter();
	virtual ncnn::DataReader* getModel();
private:
	fs::File* param_file, *model_file;
};

// waifu2x process status
class _EXPORT Waifu2xProcessStatus {
public:
	int tile_count = 0, tile_index = 0;
	int target_scale = 0, current_scale = 0;
};

class _EXPORT Waifu2x {
public:
	Waifu2x(Waifu2xProcessor* processor);
	Waifu2x(Waifu2xModel* model, Waifu2xProcessor* processor);
	~Waifu2x();

	int loadModel(Waifu2xModel* model);
	Waifu2xModel* getModel();
	Waifu2xMat* process(Waifu2xMat* input, int scale, Waifu2xProcessStatus* processStatus = 0);
	int process_s(Waifu2xMat** output,Waifu2xMat* input, int scale, Waifu2xProcessStatus* processStatus = 0);

	bool nodes_processor = false, nodes_model = false;
private:
	Waifu2xModel* model = 0;
	Waifu2xProcessor* processor = 0;
	ncnn::Net net;
	ncnn::Pipeline* pre_pipe = 0;
	ncnn::Pipeline* post_pipe = 0;
	ncnn::Layer* bicubic_2x = 0;

	int process_cpu(int scale, Waifu2xProcessStatus* processStatus, int w, int h, int channels, const byte* pixeldata, void* outdata);
	int process(int scale, Waifu2xProcessStatus* processStatus, int w, int h, int channels, const byte* pixeldata, void* outdata);
};

class _EXPORT Waifu2xEnvironment {
public:
	static bool is_support_gpu();
	static int get_gpu_count();
	static int get_default_gpu_index();
	static const char* get_gpu_name(int index);
	static unsigned int get_gpu_heap_budget(int index);
	static unsigned int get_device_id(int index);
	static unsigned int get_vendor_id(int index);

	static void ncnn_destroy_gpu_instance();
private:

};

class _EXPORT Waifu2xException : public std::exception {
public:
	using _Mybase = exception;
	#if _WIN32
	Waifu2xException(int errorCode, const char* errorMessage) : errorcode(errorCode),
		_Mybase(_string_format("%s Message: %s", get_error_message(errorcode), errorMessage).c_str()) {

	}
	#else
	const std::string errorMessage;

	Waifu2xException(int errorCode, const char* errorMessage) : errorcode(errorCode),
		errorMessage(_string_format("%s Message: %s", get_error_message(errorcode), errorMessage)), _Mybase() {

	}

	const char* what() const throw () {
		return errorMessage.c_str();
	}
	#endif

	const int errorcode;
private:
};