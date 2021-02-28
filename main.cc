#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>

extern const GLchar* vertex_shader_source;
extern const GLchar* fragment_shader_source;

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

	GLuint vertex_shader {glCreateShader(GL_VERTEX_SHADER)};
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	GLint vertex_compile_status;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compile_status);

	if (vertex_compile_status == GL_FALSE) {
		GLint vertex_log_length;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &vertex_log_length);

		GLchar* vertex_info_log {new GLchar [vertex_log_length]};
		glGetShaderInfoLog(vertex_shader, vertex_log_length, NULL, vertex_info_log);

		std::cerr
			<< "Vertex shader fail to compile.\n"
			<< vertex_info_log << std::endl;

		delete[] vertex_info_log;
	}

	GLuint fragment_shader {glCreateShader(GL_FRAGMENT_SHADER)};
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	GLint fragment_compile_status;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compile_status);

	if (fragment_compile_status == GL_FALSE) {
		GLint fragment_log_length;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &fragment_log_length);

		GLchar* fragment_info_log {new GLchar [fragment_log_length]};
		glGetShaderInfoLog(fragment_shader, fragment_log_length, NULL, fragment_info_log);

		std::cerr
			<< "Fragment shader fail to compile.\n"
			<< fragment_info_log << std::endl;

		delete[] fragment_info_log;
	}

	GLuint shader_program {glCreateProgram()};
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint shader_link_status;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_link_status);

	if (shader_link_status == GL_FALSE) {
		GLint shader_log_length;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &shader_log_length);

		GLchar* shader_info_log {new GLchar [shader_log_length]};
		glGetProgramInfoLog(shader_program, shader_log_length, NULL, shader_info_log);

		std::cerr
			<< "Shader program fail to link.\n"
			<< shader_info_log << std::endl;

		delete[] shader_info_log;
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

const GLchar* vertex_shader_source {
	"#version 450 core\n"

	"in vec4 vPosition;"
	"in vec4 vColour;"
	"out vec4 fColour;"

	"void main() {"
	"	gl_Position = vPosition;"
	"	fColour = vColour;"
	"}"
};

const GLchar* fragment_shader_source {
	"#version 450 core\n"

	"in vec4 fColour;"
	"out vec4 colour;"

	"void main() {"
	"	colour = fColour;"
	"}"
};
