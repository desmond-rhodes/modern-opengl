#include "shader-loader.hh"
#include "cleanup.hh"
#include <fstream>
#include <filesystem>

GLuint create_shader(size_t n, GLenum const type[], char const* const filename[], std::ostream& es) {
	auto const obj {new (std::nothrow) GLuint[n]};
	if (!obj) {
		es << "Unable to allocate buffer for shader objects.\n";
		return 0;
	}
	size_t obj_n {0};
	cleanup c_obj {[&]{
		for (size_t i {0}; i < obj_n; ++i)
			glDeleteShader(obj[i]);
	}};

	for (size_t i {0}; i < n; ++i) {
		obj[i] = glCreateShader(type[i]);
		if (!obj[i]) {
			es << "Unable to create shader object.\n";
			return 0;
		}
		obj_n += 1;

		std::ifstream file;
		file.open(filename[i]);
		if (file.fail()) {
			es << "Unable to open file " << filename[i] << ".\n";
			return 0;
		}

		std::error_code ec;
		auto const sz {std::filesystem::file_size(filename[i], ec)};
		if (ec) {
			es << "Unable to get size of file " << filename[i] << ".\n";
			return 0;
		}

		auto const src {new (std::nothrow) char[sz+1]};
		if (!src) {
			es << "Unable to allocate buffer of size " << sz+1 << " bytes to load file " << filename[i] << ".\n";
			return 0;
		}
		cleanup c_src {[&]{ delete[] src; }};

		file.read(src, sz);
		if (file.fail()) {
			es << "Unable to read file " << filename[i] << ".\n";
			return 0;
		}

		src[file.gcount()] = '\0';

		glShaderSource(obj[i], 1, &src, nullptr);
		glCompileShader(obj[i]);

		GLint sta;
		glGetShaderiv(obj[i], GL_COMPILE_STATUS, &sta);
		if (sta == GL_FALSE) {
			GLint len;
			glGetShaderiv(obj[i], GL_INFO_LOG_LENGTH, &len);
			auto const log {new (std::nothrow) char[len]};
			if (log) {
				glGetShaderInfoLog(obj[i], len, nullptr, log);
				es << log << '\n';
				delete[] log;
			}
			else
				es << "Unable to allocate buffer of size " << len << " bytes to load error log from compiling shader " << filename[i] << ".\n";
			return 0;
		}
	}

	auto const pro {glCreateProgram()};
	if (!pro) {
		es << "Unable to create shader program.\n";
		return 0;
	}
	optional_cleanup c_pro {[&]{ glDeleteProgram(pro); }};

	for (size_t i {0}; i < n; ++i)
		glAttachShader(pro, obj[i]);
	cleanup d_pro {[&]{
		for (size_t i {0}; i < n; ++i)
			glDetachShader(pro, obj[i]);
	}};

	glLinkProgram(pro);

	GLint sta;
	glGetProgramiv(pro, GL_LINK_STATUS, &sta);
	if (sta == GL_FALSE) {
		GLint len;
		glGetProgramiv(pro, GL_INFO_LOG_LENGTH, &len);
		auto const log {new (std::nothrow) char[len]};
		if (log) {
			glGetProgramInfoLog(pro, len, nullptr, log);
			es << log << '\n';
			delete[] log;
		}
		else
			es << "Unable to allocate buffer of size " << len << " bytes to load error log from linking shader.\n";
		return 0;
	}

	c_pro.disable();
	return pro;
}
