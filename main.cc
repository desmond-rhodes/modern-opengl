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

	GLFWwindow* window {glfwCreateWindow(1280, 960, "Hello, world!", nullptr, nullptr)};
	if (!window)
		return -1;

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

	GLint position_location {glGetAttribLocation(shader, "vPosition")};
	GLint colour_location {glGetAttribLocation(shader, "vColour")};

	GLfloat clear_colour[] {0.0f, 0.0f, 0.0f, 0.0f};
	constexpr GLuint NumVertices {4};
	GLfloat vertices[NumVertices * 2] {
		 0.5f,  0.5f,
		-0.5f,  0.5f,
		-0.5f, -0.5f,
		 0.5f, -0.5f
	};
	GLfloat colours[NumVertices * 3] {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};
	constexpr GLuint NumElements {4};
	GLuint elements[NumElements] {1, 0, 2, 3};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertices) + sizeof(colours), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo, 0, sizeof(vertices), vertices);
	glNamedBufferSubData(vbo, sizeof(vertices), sizeof(colours), colours);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(position_location);

	glVertexAttribPointer(colour_location, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) sizeof(vertices));
	glEnableVertexAttribArray(colour_location);

	GLuint ebo;
	glCreateBuffers(1, &ebo);
	glNamedBufferStorage(ebo, sizeof(elements), elements, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glClearBufferfv(GL_COLOR, 0, clear_colour);
	glDrawElements(GL_TRIANGLE_STRIP, NumElements, GL_UNSIGNED_INT, 0);
	glfwSwapBuffers(window);

	auto poll_limit_last {std::chrono::steady_clock::now()};
	std::chrono::microseconds poll_limit_time {16666}; /* 60Hz */

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		std::this_thread::sleep_for(poll_limit_time - (std::chrono::steady_clock::now() - poll_limit_last));
		poll_limit_last += poll_limit_time;
	}

	std::cout << "Hello, world!\n";
	return 0;
}
