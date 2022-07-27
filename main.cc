#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>

GLuint shader(size_t, GLenum const[], char const* const[]);

GLfloat view[16];

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window {glfwCreateWindow(1280, 960, "Modern OpenGL", nullptr, nullptr)};
	glfwSetWindowRefreshCallback(window, [](GLFWwindow* window) {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);
		GLfloat r, l, t, b;
		if (w > h) {
			r =  static_cast<GLfloat>(w) / h;
			l = -static_cast<GLfloat>(w) / h;
			t =  1.0f;
			b = -1.0f;
		} else {
			r =  1.0f;
			l = -1.0f;
			t =  static_cast<GLfloat>(h) / w;
			b = -static_cast<GLfloat>(h) / w;
		}
		GLfloat const n { 1.0f};
		GLfloat const f {11.0f};
		GLfloat const frustum[] {
		2.0f*n/(r-l),         0.0f,            0.0f,  0.0f,
		        0.0f, 2.0f*n/(t-b),            0.0f,  0.0f,
		 (r+l)/(r-l),  (t+b)/(t-b),    -(f+n)/(f-n), -1.0f,
		        0.0f,         0.0f, -2.0f*f*n/(f-n),  0.0f
		};
		GLfloat const translate[] {
			1.0f, 0.0f,  0.0f, 0.0f,
			0.0f, 1.0f,  0.0f, 0.0f,
			0.0f, 0.0f,  1.0f, 0.0f,
			1.0f, 1.0f, -3.0f, 1.0f
		};
		for (size_t i {0}; i < 4; ++i)
			for (size_t j {0}; j < 4; ++j) {
				view[i*4+j] = 0.0f;
				for (size_t k {0}; k < 4; ++k)
					view[i*4+j] += frustum[k*4+j] * translate[i*4+k];
			}
	});
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	gl3wInit();
	std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n' << std::flush;
	glEnable(GL_DEPTH_TEST);

	GLenum const src_t[] {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

	char const* const src[] {
		"#version 450 core\n"
		"uniform ubo { mat4 transform; };\n"
		"layout (location = 0) in vec4 vPosition;\n"
		"layout (location = 1) in vec4 vColor;\n"
		"out vec4 fColor;\n"
		"void main() {\n"
		"	gl_Position = transform * vPosition;\n"
		"	fColor = vColor;\n"
		"}\n"
	,
		"#version 450 core\n"
		"in vec4 fColor;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"	color = fColor;\n"
		"}\n"
	};

	GLuint const sha {shader(2, src_t, src)};
	if (!sha)
		return -1;
	glUseProgram(sha);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat));
	glVertexAttribBinding(1, 0);

	GLuint vbo;
	GLfloat const vertex[] {
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertex), vertex, 0);
	glBindVertexBuffer(0, vbo, 0, 6*sizeof(GLfloat));

	GLuint ebo;
	GLuint const element[] {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7
	};
	glCreateBuffers(1, &ebo);
	glNamedBufferStorage(ebo, sizeof(element), element, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint ubo;
	glGenBuffers(1, &ubo);
	// glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	auto const uboIndex {glGetUniformBlockIndex(sha, "ubo")};
	glUniformBlockBinding(sha, uboIndex, 0);
	GLint uboSize;
	glGetActiveUniformBlockiv(sha, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);
	char const* uboName {"transform"};
	GLuint uboIndices;
	glGetUniformIndices(sha, 1, &uboName, &uboIndices);
	GLint uboOffset;
	glGetActiveUniformsiv(sha, 1, &uboIndices, GL_UNIFORM_OFFSET, &uboOffset);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

	auto const buffer {new char[uboSize]};
	GLfloat const color[] {0.0f, 0.0f, 0.0f, 0.0f};
	GLfloat const depth[] {1.0f};

	while (!glfwWindowShouldClose(window)) {
		memcpy(buffer+uboOffset, &view, 16*sizeof(GLfloat));
		glNamedBufferData(ubo, uboSize, buffer, GL_STATIC_DRAW);
		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfv(GL_DEPTH, 0, depth);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete[] buffer;
	return 0;
}

GLuint shader(size_t num, GLenum const src_t[], char const* const src[]) {
	auto const inf {[](GLuint o, GLenum u){GLint x; glGetShaderiv(o, u, &x); return x;}};
	auto const obj {new GLuint[num]};
	bool fail {false};
	for (size_t i {0}; i < num; ++i) {
		obj[i] = glCreateShader(src_t[i]);
		glShaderSource(obj[i], 1, src+i, nullptr);
		glCompileShader(obj[i]);
		if (inf(obj[i], GL_COMPILE_STATUS))
			continue;
		fail = true;
		auto const len {inf(obj[i], GL_INFO_LOG_LENGTH)};
		auto const log {new char[len]};
		glGetShaderInfoLog(obj[i], len, nullptr, log);
		std::cerr << log;
		delete[] log;
	}
	GLuint pro {0};
	if (!fail) {
		pro = glCreateProgram();
		for (size_t i {0}; i < num; ++i)
			glAttachShader(pro, obj[i]);
		glLinkProgram(pro);
		for (size_t i {0}; i < num; ++i)
			glDetachShader(pro, obj[i]);
	}
	for (size_t i {0}; i < num; ++i)
		glDeleteShader(obj[i]);
	delete[] obj;
	return pro;
}
