#include "fs.h"
#include <string.h>
#include <stdexcept>
#if _WIN32
#include <io.h>
#include <windows.h>
#else // _WIN32
#include <sys/stat.h>
#include <unistd.h>
#endif // _WIN32

namespace fs {
	// TODO: FIXED BUFFER
	path combine(const path& p1, const path& p2) {
		//return std::filesystem::path(p1) / p2;
		return p1[p1.length()] != sep ? p1 + sep + p2 : p1 + p2;
	}

	path combine(const path& p1, const path& p2, const path& p3) {
		//return p1 / std::filesystem::path(p2) / p2;
		return combine(combine(p1, p2), p3);
	}

	int open_s(File** file, path path, fstr mode) {
		FILE* _file;
		#if _WIN32
		auto r = _wfopen_s(&_file, path.c_str(), mode);
		#else // _WIN32
		_file = fopen(path.c_str(), mode);
		auto r = _file ? 0 : -1;
		#endif // _WIN32
		if (!r) *file = new File(_file);
		return r;
	}

	File* open(path path, fstr mode) {
		File* file = new File(path, mode);
		return file;
	}

	std::string to_string(path path) {
		#if _WIN32
		auto str = std::wstring(path);
		return std::string(str.begin(), str.end());
		#else
		return path;
		#endif
	}

	bool exists(path path) {
		#if _WIN32
		struct _stat64i32 buffer;
		return (_wstat(path.c_str(), &buffer) == 0);
		#else
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
		#endif
	}

	int status(path path) {
		#if _WIN32
		struct _stat64i32 buffer;
		if (_wstat(path.c_str(), &buffer) != 0) return -1;
		#else
		struct stat buffer;
		if (stat(path.c_str(), &buffer) != 0) return -1;
		#endif

		if (buffer.st_mode & S_IFDIR) return 0;
		else if (buffer.st_mode & S_IFREG) return 1;
		else return -1;
	}

	path executable_path() {
		#if _WIN32
		wchar_t filepath[MAX_PATH];
		GetModuleFileNameW(NULL, filepath, MAX_PATH);
		return filepath;
		#else // _WIN32
		char filepath[256];
		readlink("/proc/self/exe", filepath, 256);
		return filepath;
		#endif // _WIN32
	}

	path executable_directory() {
	#if _WIN32
		wchar_t filepath[MAX_PATH];
		GetModuleFileNameW(NULL, filepath, MAX_PATH);
		wchar_t* backslash = wcsrchr(filepath, L'\\');
		backslash[1] = L'\0';
		return filepath;
	#else // _WIN32
		char filepath[256];
		readlink("/proc/self/exe", filepath, 256);
		char* slash = strrchr(filepath, '/');
		slash[1] = '\0';
		return filepath;
	#endif // _WIN32
	}

	File::File() { 
		_file = 0;
	}

	File::File(FILE* file) {
		_file = file;
	}

	File::File(path path, fstr mode) { 
		_file = 0;
		open_s(path, mode);
	}

	File::~File() {
		if (this && !_noclose) close();
	}

	int File::open_s(path path, fstr mode) {
		#if _WIN32
		return _wfopen_s(&_file, path.c_str(), mode);
		#else // _WIN32
		_file = fopen(path.c_str(), mode);
		auto r = _file ? 0 : -1;
		#endif // _WIN32
	}

	bool File::is_open() {
		return _file;
	}

	void File::open(path path, fstr mode) {
		#if _WIN32
		if (_wfopen_s(&_file, path.c_str(), mode))
			throw std::runtime_error(strerror(errno));
		#else // _WIN32
		_file = fopen(path.c_str(), mode);
		#endif // _WIN32
	}

	int File::seek(long offset, int origin) {
		return fseek(_file, offset, origin);
	}

	long File::tell() {
		return ftell(_file);
	}

	void File::rewind() {
		return std::rewind(_file);
	}

	size_t File::read(void* buf, size_t buf_size, size_t element_size, size_t count) {
		#if _WIN32
		return fread_s(buf, buf_size, element_size, count, _file);
		#else
		return fread(buf, buf_size, element_size, _file);
		#endif
	}

	size_t File::write(const void* buf, size_t element_size, size_t count) {
		return fwrite(buf, element_size, count, _file);
	}

	long File::length(long offset) {
		fseek(_file, offset, SEEK_END);
		long sz = ftell(_file);
		fseek(_file, offset, SEEK_SET);
		return sz;
	}

	int File::close() {
		if (_file) {
			auto* f = _file;
			_file = 0;
			return fclose(f);
		}
		else return 0;
	}

	FILE* File::get() {
		return _file;
	}
}