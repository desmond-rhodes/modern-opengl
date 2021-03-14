#ifndef SHADER_LOADER_HH
#define SHADER_LOADER_HH

#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <GL/gl3w.h>

namespace shader_loader {
	struct info {
		GLenum type;
		std::string file;
	};

	GLuint create_program(std::vector<shader_loader::info> const&);
	/* may throw std::ios_base::failure */
	/* may throw shader_loader::acquire_error */
	/* may throw shader_loader::compile_error */

	class acquire_error : public std::exception {
		public : const char* what() const noexcept { return msg; }
		private: const char* msg {"shader_loader::acquire_error"};
	};
	class compile_error : public std::runtime_error { using runtime_error::runtime_error; };
}

#endif
