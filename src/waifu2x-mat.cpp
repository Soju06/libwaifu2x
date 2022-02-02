#include "waifu2x.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Waifu2xMat::Waifu2xMat(ncnn::Mat* mat, bool nodatarelease) {
	source = mat;
	pixelData = (byte*)mat->data;
	element_size = mat->elemsize;
	width = mat->w;
	height = mat->h;
	_nodatarelease = nodatarelease;
}

Waifu2xMat::~Waifu2xMat() {
	if (!_norelease) {
		if (!_nodatarelease) delete pixelData;
		delete source;
	}
}

ncnn::Mat* Waifu2xMat::getSource() {
	return source;
}

int Waifu2xMat::save(fs::path path, Waifu2xImageFormat format, int quality) {
	switch (format) {
	case Waifu2xImageFormat::Png:
		return save_png(path);
	case Waifu2xImageFormat::Jpg:
		return save_jpg(path);
	}
	return -2;
}

int Waifu2xMat::save(byte** data, int* length, Waifu2xImageFormat format, int quality) {
	if (!data || !length) return -1;
	switch (format) {
	case Waifu2xImageFormat::Png:
		return save_png(data, length);
		break;
	case Waifu2xImageFormat::Jpg:
		return save_jpg(data, length, quality);
		break;
	default: return -2;
	}
}

int Waifu2xMat::save_png(byte** data, int* length) {
	if (!data || !length) return -1;
	int len;
	auto* _data = stbi_write_png_to_mem(pixelData, 0, width, height, element_size, &len);
	if (_data) *data = _data;
	else return -1;
	return 0;
}

int Waifu2xMat::save_png(fs::path path) {
	fs::File file(path, PATH("wb"));
	if (!file.is_open()) return 0x000013C3;
	int err = save_png(&file);
	if (err) return err;
	file.close();
	return 0;
}

int Waifu2xMat::save_png(fs::File* file) {
	if (!file) return -1;
	int len;
	auto* data = stbi_write_png_to_mem(pixelData, 0, width, height, element_size, &len);
	if (!data) return 0x000013C1;
	file->write(data, 1, len);
	delete data;
	return 0;
}

constexpr auto IMAGE_WRITE_MEM_BUFFER_SIZE = 0xFFFF;

class write_content {
public:
	byte* content = (byte*)malloc(IMAGE_WRITE_MEM_BUFFER_SIZE);
	int length = IMAGE_WRITE_MEM_BUFFER_SIZE, offset = 0;
};

int Waifu2xMat::save_jpg(byte** data, int* length, int quality) {
	if (!data || !length) return -1;
	write_content content = { };
	stbi_write_jpg_to_func(
		[](void* context, void* data, int size) {
			auto* content = (write_content*)context;
			// buffer realloc
			if (content->offset + size > content->length) {
				auto* buf = realloc(content->content, (size_t)(content->length *= 2));
				content->content = (byte*)buf;
			}
			// copy buffer
			#if _WIN32
			memcpy_s(content->content + content->offset, size, data, size);
			#else
			memcpy(content->content + content->offset, data, size);
			#endif
			content->offset += size;
		},
		&content,
		width, 
		height, 
		element_size, 
		pixelData, 
		quality
	);
	*data = (byte*)content.content;
	*length = content.offset;
	return 0;
}

int Waifu2xMat::save_jpg(fs::path path, int quality) {
	fs::File file(path, PATH("wb"));
	if (!file.is_open()) return 0x000013C3;
	int err = save_png(&file);
	if (err) return err;
	file.close();
	return 0;
}

int Waifu2xMat::save_jpg(fs::File* file, int quality) {
	if (!file) return -1;
	stbi_write_jpg_to_func(
		[](void* context, void* data, int size) {
			((fs::File*)context)->write(data, 1, size);
		},
		file,
		width,
		height,
		element_size,
		pixelData,
		quality
	);
	return 0;
}

Waifu2xMat* Waifu2xMat::load(fs::path path) {
	Waifu2xMat* mat = 0;
	load_s(&mat, path);
	return mat;
}

int Waifu2xMat::load_s(Waifu2xMat** mat, fs::path path) {
	if (!mat) return -1;
	fs::File* file = nullptr;

	if (fs::open_s(&file, path, PATH("rb"))) {
		return 0x000013C0;
	}

	auto r = load_s(mat, file);
	delete file;

	return r;
}

Waifu2xMat* Waifu2xMat::load(fs::File* file) {
	Waifu2xMat* mat = 0;
	load_s(&mat, file);
	return mat;
}

int Waifu2xMat::load_s(Waifu2xMat** mat, fs::File* file) {
	if (!mat || !file) return -1;

	long length = file->length();
	if (length <= 0) return 0x000013C1;

	void* _data = malloc(length);
	byte* data = (unsigned char*)_data;

	if (data) {
		file->read(data, length, 1, length);
	}

	int err = load_s(mat, data, length);
	free(_data);
	return err;
}

Waifu2xMat* Waifu2xMat::load(byte* data, int size) {
	Waifu2xMat* mat = 0;
	load_s(&mat, data, size);
	return mat;
}

int Waifu2xMat::load_s(Waifu2xMat** mat, byte* data, int size) {
	if (!mat || !data) return -1;
	if (data) {
		unsigned char* pixeldata = 0;
		int w, h, c;
		pixeldata = stbi_load_from_memory(data, size, &w, &h, &c, 0);

		*mat = new Waifu2xMat(new ncnn::Mat(w, h, (void*)pixeldata, (size_t)c, c), false);
	} else {
		return 0x000013C1;
	}

	return 0;
}
