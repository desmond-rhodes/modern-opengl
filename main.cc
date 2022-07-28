#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cmath>
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
			0.0f, 0.0f, -2.0f, 1.0f
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
	glEnable(GL_CULL_FACE);

	GLenum const src_t[] {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

	char const* const src[] {
		"#version 450 core\n"

		"uniform ubo {"
		"	mat4 view;"
		"	mat4 model;"
		"};"

		"layout (location = 0) in vec4 v_position;"
		"layout (location = 1) in vec4 v_normal;"
		"layout (location = 2) in vec4 v_color;"

		"out vec3 f_normal;"
		"out vec4 f_color;"

		"void main() {"
		"	gl_Position = view * model * v_position;"
		"	f_normal = (model * v_normal).xyz;"
		"	f_color = v_color;"
		"}"
	,
		"#version 450 core\n"

		"in vec3 f_normal;"
		"in vec4 f_color;"

		"out vec4 color;"

		"const vec3 light = normalize(vec3(-1.0, 0.0, -3.0));"

		"void main() {"
		"	const float shade = dot(f_normal, -light);"
		"	color = shade * f_color;"
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
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_TRUE, 3*sizeof(GLfloat));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(2);
	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat));
	glVertexAttribBinding(2, 0);

	GLuint vbo;
	GLfloat const vertex[] {
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // Front Bottom Right
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // Front Top    Right
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 0.0f, // Front Top    Left
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Front Bottom Left

		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Back  Bottom Left
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // Back  Top    Left
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // Back  Top    Right
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, // Back  Bottom Right

		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // Back  Bottom Right
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, // Back  Top    Right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Front Top    Right
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 1.0f, // Front Bottom Right

		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, // Front Bottom Left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // Front Top    Left
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 1.0f, // Back  Top    Left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Back  Bottom Left

		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, // Back  Top    Right
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, // Back  Top    Left
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // Front Top    Left
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Front Top    Right

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Back  Bottom Left
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // Back  Bottom Right
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 1.0f, // Front Bottom Right
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 1.0f, // Front Bottom Left
	};
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertex), vertex, 0);
	glBindVertexBuffer(0, vbo, 0, 9*sizeof(GLfloat));

	GLuint ebo;
	GLuint const element[] {
		 0,  1,  2,  0,  2,  3,
		 4,  5,  6,  4,  6,  7,
		 8,  9, 10,  8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};
	glCreateBuffers(1, &ebo);
	glNamedBufferStorage(ebo, sizeof(element), element, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint ubo;
	glGenBuffers(1, &ubo);
	// glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	auto const ubo_index {glGetUniformBlockIndex(sha, "ubo")};
	glUniformBlockBinding(sha, ubo_index, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

	GLint u_size;
	char const* u_name[] {"view", "model"};
	GLuint u_indices[2];
	GLint u_offset[2];
	glGetActiveUniformBlockiv(sha, ubo_index, GL_UNIFORM_BLOCK_DATA_SIZE, &u_size);
	glGetUniformIndices(sha, 2, u_name, u_indices);
	glGetActiveUniformsiv(sha, 2, u_indices, GL_UNIFORM_OFFSET, u_offset);

	auto const buffer {new char[u_size]};
	GLfloat const color[] {0.3f, 0.3f, 0.3f, 0.0f};
	GLfloat const depth[] {1.0f};

	double a {0.0};

	while (!glfwWindowShouldClose(window)) {
		GLfloat model[16];
		{
			GLfloat const sina {static_cast<GLfloat>(std::sin(a))};
			GLfloat const cosa {static_cast<GLfloat>(std::cos(a))};
			GLfloat const rotate[] {
				 cosa, 0.0f, sina, 0.0f,
				 0.0f, 1.0f, 0.0f, 0.0f,
				-sina, 0.0f, cosa, 0.0f,
				 0.0f, 0.0f, 0.0f, 1.0f
			};
			a += 0.011;
			if (a > 360.0)
				a -= 360.0;
			for (size_t i {0}; i < 16; ++i)
				model[i] = rotate[i];
		}
		memcpy(buffer+u_offset[0], &view, 16*sizeof(GLfloat));
		memcpy(buffer+u_offset[1], &model, 16*sizeof(GLfloat));
		glNamedBufferData(ubo, u_size, buffer, GL_STATIC_DRAW);
		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfv(GL_DEPTH, 0, depth);
		glDrawElements(GL_TRIANGLES, sizeof(element)/sizeof(element[0]), GL_UNSIGNED_INT, 0);
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
