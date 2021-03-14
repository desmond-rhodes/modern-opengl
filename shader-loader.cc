#include "shader-loader.hh"

GLuint shader_loader::create_program(std::vector<shader_loader::info> const& shader_info) {
	GLuint program {glCreateProgram()};
	if (!program)
		throw shader_loader::acquire_error();

	try {
		std::vector<GLuint> detach;

		for (auto const& info : shader_info) {
			std::ifstream file;
			file.exceptions(std::ios_base::failbit | std::ios_base::badbit);

			file.open(info.file, std::ios_base::in);
			file.seekg(0, std::ios_base::end);
			std::string source(file.tellg(), '\0');

			file.seekg(0, std::ios_base::beg);
			file.read(&source[0], source.size());
			file.close();

			GLuint shader {glCreateShader(info.type)};
			if (!shader)
				throw shader_loader::acquire_error();

			GLchar const* source_ptr {source.c_str()};
			glShaderSource(shader, 1, &source_ptr, nullptr);

			glCompileShader(shader);
			glAttachShader(program, shader);
			glDeleteShader(shader);
			detach.push_back(shader);

			GLint status;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

			if (status == GL_FALSE) {
				GLint length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

				std::string log(length, '\0');
				glGetShaderInfoLog(shader, length, nullptr, &log[0]);

				throw shader_loader::compile_error(log);
			}
		}

		glLinkProgram(program);
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);

		if (status == GL_FALSE) {
			GLint length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

			std::string log(length, '\0');
			glGetProgramInfoLog(program, length, nullptr, &log[0]);

			throw shader_loader::compile_error(log);
		}

		for (auto shader : detach)
			glDetachShader(program, shader);
	}
	catch (...) {
		glDeleteProgram(program);
		throw;
	}

	return program;
}
