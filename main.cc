#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	if (!glfwInit()) {
		std::cerr << "Fail to initialise GLFW.\n";
		return -1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window {glfwCreateWindow(640, 480, "Hello, world!", NULL, NULL)};

	if (!window) {
		std::cerr << "Fail to create window.\n";
		return -2;
	}

	glfwMakeContextCurrent(window);
	if (gl3wInit()) {
		std::cerr << "Fail to initialise GL3W.\n";
		return -3;
	}

	if (!gl3wIsSupported(4, 5)) {
		std::cerr << "OpenGL version 4.5 is not supported by GL3W.\n";
		return -4;
	}

	GLfloat clear_colour[] {0.0f, 0.0f, 0.25f, 0.0f};

	constexpr GLuint NumVertices {4};
	GLfloat vertices[NumVertices * 2] {
		 0.5f,  0.5f,
		-0.5f,  0.5f,
		-0.5f, -0.5f,
		 0.5f, -0.5f
	};

	constexpr GLuint NumElements {4};
	GLuint elements[NumElements] {1, 0, 2, 3};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertices), vertices, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLuint ebo;
	glCreateBuffers(1, &ebo);
	glNamedBufferStorage(ebo, sizeof(elements), elements, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

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

	glfwDestroyWindow(window);
	glfwTerminate();

	std::cout << "Hello, world!\n";
	return 0;
}
