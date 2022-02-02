#pragma once

#include <string>
#include <filesystem>

namespace fs {
	#if _WIN32
	typedef std::wstring path;
	#define PATH(X) L##X

	typedef std::wstring str;
	typedef const wchar_t* fstr;
	typedef const wchar_t fchar;
	constexpr fchar sep = '\\';
	#else
	typedef std::string path;
	#define PATH(X) X

	typedef std::string str;
	typedef const char* fstr;
	typedef const char fchar;
	constexpr fchar sep = '/';
	#endif

	class File {
	public:
		File();
		File(FILE* file);
		File(path path, fstr mode);
		~File();

		bool is_open();
		void open(path path, fstr mode);
		int open_s(path path, fstr mode);
		int seek(long offset, int origin);
		long tell();
		void rewind();
		size_t read(void* buf, size_t buf_size, size_t element_size, size_t count);
		size_t write(const void* buf, size_t element_size, size_t count);
		long length(long offset = 0L);
		int close();
		FILE* get();
		bool _noclose = false;
	private:
		FILE* _file = 0;
	};

	path combine(const path& p1, const path& p2);
	path combine(const path& p1, const path& p2, const path& p3);
	int open_s(File** file, path path, fstr mode);
	File* open(path path, fstr mode);
	std::string to_string(path path);
	bool exists(path path);
	// Returns 1 for a file, 0 for a directory, -1 otherwise.
	int status(path path);
	path executable_path();
	path executable_directory();
}

