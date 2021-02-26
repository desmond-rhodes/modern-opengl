#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window {glfwCreateWindow(640, 480, "Hello, world!", NULL, NULL)};
	glfwMakeContextCurrent(window);
	gl3wInit();

	auto poll_limit_last {std::chrono::steady_clock::now()};
	std::chrono::microseconds poll_limit_time {16666}; /* 60Hz */

	GLfloat blue[] {0.0f, 0.0f, 0.25f, 0.0f};
	glClearBufferfv(GL_COLOR, 0, blue);
	glfwSwapBuffers(window);

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
