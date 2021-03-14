#include "shader-loader.hh"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window {glfwCreateWindow(640, 480, "Hello, world!", nullptr, nullptr)};
	glfwMakeContextCurrent(window);
	gl3wInit();

	GLuint shader_program;
	try {
		std::vector<shader_loader::info> shader_info {
			{GL_VERTEX_SHADER, "shader.vert"},
			{GL_FRAGMENT_SHADER, "shader.frag"}
		};
		shader_program = shader_loader::create_program(shader_info);
	}
	catch (std::exception const& error) {
		std::cerr << error.what() << '\n';
		return -1;
	}
	glUseProgram(shader_program);

	GLint position_location {glGetAttribLocation(shader_program, "vPosition")};
	GLint colour_location {glGetAttribLocation(shader_program, "vColour")};

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

	glfwTerminate();
	std::cout << "Hello, world!\n";
	return 0;
}
