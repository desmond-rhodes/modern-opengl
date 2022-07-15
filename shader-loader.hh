#ifndef SHADER_LOADER_HH
#define SHADER_LOADER_HH

#include <GL/gl3w.h>
#include <ostream>

GLuint create_shader(size_t, GLenum const[], char const* const[], std::ostream&);

#endif
