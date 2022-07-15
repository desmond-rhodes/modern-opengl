#include "shader-loader.hh"
#include "cleanup.hh"
#include <fstream>

GLuint create_shader(size_t n, GLenum const type[], char const* const filename[], std::ostream& es) {
	GLint status;
	GLint length;
	char* log {nullptr};
	optional_cleanup c_log {[&]{
		if (log)
			es << log << '\n';
		else
			es << "Unable to allocate shader program error log.\n";
		delete[] log;
	}, false};

	GLuint pro {glCreateProgram()};
	if (!pro)
		return 0;
	optional_cleanup c_pro {[&]{ glDeleteProgram(pro); }};

	auto det {new (std::nothrow) GLuint[n]};
	if (!det)
		return 0;
	cleanup c_det {[&]{ delete[] det; }};

	for (size_t i {0}; i < n; ++i) {
		std::ifstream file;
		file.open(filename[i], std::ios_base::in);
		if (file.fail()) {
			es << "Unable to open file " << filename[i] << ".\n";
			return 0;
		}
		cleanup c_file {[&]{ file.close(); }};

		file.seekg(0, std::ios_base::end);
		size_t size = file.tellg();
		auto src {new (std::nothrow) char[size+1]()};
		if (!src)
			return 0;
		cleanup c_src {[&]{ delete[] src; }};

		file.seekg(0, std::ios_base::beg);
		file.read(src, size);
		if (file.fail()) {
			es << "Unable to read file " << filename[i] << ".\n";
			return 0;
		}

		GLuint obj {glCreateShader(type[i])};
		if (!obj)
			return 0;
		cleanup c_ojb {[&]{ glDeleteShader(obj); }};

		glShaderSource(obj, 1, &src, nullptr);

		glCompileShader(obj);
		glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			c_log.enable();
			glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
			log = new (std::nothrow) char[length];
			if (!log)
				return 0;
			glGetShaderInfoLog(obj, length, nullptr, log);
			return 0;
		}

		glAttachShader(pro, obj);
		det[i] = obj;
	}

	glLinkProgram(pro);
	glGetProgramiv(pro, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		c_log.enable();
		glGetProgramiv(pro, GL_INFO_LOG_LENGTH, &length);
		log = new (std::nothrow) char[length];
		if (!log)
			return 0;
		glGetProgramInfoLog(pro, length, nullptr, log);
		return 0;
	}

	for (size_t i {0}; i < n; ++i)
		glDetachShader(pro, det[i]);

	c_pro.disable();
	return pro;
}
