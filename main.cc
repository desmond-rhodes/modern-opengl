#include "shader-loader.hh"
#include "cleanup.hh"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	if (!glfwInit())
		return -1;
	cleanup c_glfw {[&]{ glfwTerminate(); }};

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window {glfwCreateWindow(1280, 960, "Modern OpenGL", nullptr, nullptr)};
	if (!window)
		return -1;

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
		glViewport(0, 0, width, height);
	});

	glfwMakeContextCurrent(window);
	if (gl3wInit())
		return -1;

	std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	GLuint shader;
	{
		GLenum const type[] {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
		char const* const filename[] {"shader.vert", "shader.frag"};
		shader = create_shader(2, type, filename, std::cerr);
		if (!shader)
			return -1;
	}
	glUseProgram(shader);

	GLuint const bindingindex {0};
	GLuint vao;
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLint const attrib_position {glGetAttribLocation(shader, "vPosition")};
		glEnableVertexAttribArray(attrib_position);
		glVertexAttribFormat(attrib_position, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(attrib_position, bindingindex);

		GLint const attrib_color {glGetAttribLocation(shader, "vColour")};
		glEnableVertexAttribArray(attrib_color);
		glVertexAttribFormat(attrib_color, 3, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat));
		glVertexAttribBinding(attrib_color, bindingindex);

		glBindVertexArray(0);
	}

	GLuint vbo;
	{
		GLfloat vertex[] {
			 0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 1.0f
		};
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, sizeof(vertex), vertex, 0);
	}

	GLuint ebo;
	{
		GLuint elements[] {1, 0, 2, 3};
		glCreateBuffers(1, &ebo);
		glNamedBufferStorage(ebo, sizeof(elements), elements, 0);
	}

	glBindVertexArray(vao);
	glBindVertexBuffer(bindingindex, vbo, 0, 5*sizeof(GLfloat));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLfloat const fill[] {0.0f, 0.0f, 0.0f, 0.0f};

	auto poll_limit_last {std::chrono::steady_clock::now()};
	std::chrono::microseconds poll_limit_time {16666}; /* 60Hz */

	while (!glfwWindowShouldClose(window)) {
		glClearBufferfv(GL_COLOR, 0, fill);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);

		glfwPollEvents();

		std::this_thread::sleep_for(poll_limit_time - (std::chrono::steady_clock::now() - poll_limit_last));
		poll_limit_last += poll_limit_time;
	}

	return 0;
}
