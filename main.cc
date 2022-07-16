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

	std::vector<double> o_x_sam;
	std::vector<double> o_y_sam;

	int const r {500};
	int x {0};
	int y {r};
	int d {3 - 2*r};

	while (y >= x) {
	  o_x_sam.push_back(static_cast<double>(x)/r);
	  o_y_sam.push_back(static_cast<double>(y)/r);
	  ++x;
	  if (d > 0) {
	    --y;
	    d = d + 4*(x - y) + 10;
	  }
	  else {
	    d = d + 4*x + 6;
	  }
	}

	std::vector<double> d_x_sam;
	std::vector<double> d_y_sam;

	auto const inc {o_x_sam.size()/20};

	for (size_t i {inc}; i < o_x_sam.size()-inc/2; i += inc) {
	  d_x_sam.push_back(o_x_sam[i]);
	  d_y_sam.push_back(o_y_sam[i]);
	}

	std::vector<double> c_x_sam;
	std::vector<double> c_y_sam;

	c_x_sam.push_back( o_x_sam.front());
	c_y_sam.push_back( o_y_sam.front());
	for (size_t i {0}; i < d_x_sam.size(); ++i) {
	  c_x_sam.push_back( d_x_sam[i]);
	  c_y_sam.push_back( d_y_sam[i]);
	}
	c_x_sam.push_back( o_x_sam.back());
	c_y_sam.push_back( o_y_sam.back());
	for (size_t i {d_x_sam.size()}; i > 0; --i) {
	  c_x_sam.push_back( d_y_sam[i-1]);
	  c_y_sam.push_back( d_x_sam[i-1]);
	}
	c_x_sam.push_back( o_y_sam.front());
	c_y_sam.push_back( o_x_sam.front());
	for (size_t i {0}; i < d_x_sam.size(); ++i) {
	  c_x_sam.push_back( d_y_sam[i]);
	  c_y_sam.push_back(-d_x_sam[i]);
	}
	c_x_sam.push_back( o_x_sam.back());
	c_y_sam.push_back(-o_y_sam.back());
	for (size_t i {d_x_sam.size()}; i > 0; --i) {
	  c_x_sam.push_back( d_x_sam[i-1]);
	  c_y_sam.push_back(-d_y_sam[i-1]);
	}
	c_x_sam.push_back( o_x_sam.front());
	c_y_sam.push_back(-o_y_sam.front());
	for (size_t i {0}; i < d_x_sam.size(); ++i) {
	  c_x_sam.push_back(-d_x_sam[i]);
	  c_y_sam.push_back(-d_y_sam[i]);
	}
	c_x_sam.push_back(-o_x_sam.back());
	c_y_sam.push_back(-o_y_sam.back());
	for (size_t i {d_x_sam.size()}; i > 0; --i) {
	  c_x_sam.push_back(-d_y_sam[i-1]);
	  c_y_sam.push_back(-d_x_sam[i-1]);
	}
	c_x_sam.push_back(-o_y_sam.front());
	c_y_sam.push_back( o_x_sam.front());
	for (size_t i {0}; i < d_x_sam.size(); ++i) {
	  c_x_sam.push_back(-d_y_sam[i]);
	  c_y_sam.push_back( d_x_sam[i]);
	}
	c_x_sam.push_back(-o_x_sam.back());
	c_y_sam.push_back( o_y_sam.back());
	for (size_t i {d_x_sam.size()}; i > 0; --i) {
	  c_x_sam.push_back(-d_x_sam[i-1]);
	  c_y_sam.push_back( d_y_sam[i-1]);
	}

	if (!glfwInit())
		return -1;
	cleanup c_glfw {[]{ glfwTerminate(); }};

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window {glfwCreateWindow(960, 960, "Modern OpenGL", nullptr, nullptr)};
	if (!window)
		return -1;

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
		glViewport(0, 0, width, height);
	});

	glfwMakeContextCurrent(window);
	if (gl3wInit())
		return -1;

	std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	GLuint sha;
	{
		GLenum const type[] {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
		char const* const filename[] {"shader.vert", "shader.frag"};
		sha = create_shader(2, type, filename, std::cerr);
		if (!sha)
			return -1;
	}
	glUseProgram(sha);

	GLuint const bindingindex {0};
	GLuint vao;
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLint const attrib_position {glGetAttribLocation(sha, "vPosition")};
		glEnableVertexAttribArray(attrib_position);
		glVertexAttribFormat(attrib_position, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(attrib_position, bindingindex);

		GLint const attrib_color {glGetAttribLocation(sha, "vColour")};
		glEnableVertexAttribArray(attrib_color);
		glVertexAttribFormat(attrib_color, 3, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat));
		glVertexAttribBinding(attrib_color, bindingindex);

		glBindVertexArray(0);
	}

	GLuint vbo;
	{
		std::vector<GLfloat> vertex {0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
		for (size_t i {0}; i < c_x_sam.size(); ++i) {
			vertex.push_back(c_x_sam[i]/2.0);
			vertex.push_back(c_y_sam[i]/2.0);
			switch (i % 4) {
			case 0: vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); break;
			case 1: vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); break;
			case 2: vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f); break;
			case 3: vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(1.0f); break;
			}
		}
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, vertex.size()*sizeof(GLfloat), vertex.data(), 0);
	}

	size_t ebo_n;
	GLuint ebo;
	{
		std::vector<GLuint> elements;
		for (size_t i {0}; i < c_x_sam.size()-1; ++i) {
			elements.push_back(0);
			elements.push_back(i+1);
			elements.push_back(i+2);
		}
		elements.push_back(0);
		elements.push_back(c_x_sam.size());
		elements.push_back(1);
		glCreateBuffers(1, &ebo);
		glNamedBufferStorage(ebo, elements.size()*sizeof(GLuint), elements.data(), 0);
		ebo_n = elements.size();
	}

	glBindVertexArray(vao);
	glBindVertexBuffer(bindingindex, vbo, 0, 5*sizeof(GLfloat));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLfloat const fill[] {0.0f, 0.0f, 0.0f, 0.0f};

	auto poll_limit_last {std::chrono::steady_clock::now()};
	std::chrono::microseconds poll_limit_time {16666}; /* 60Hz */

	std::cout << std::flush;

	while (!glfwWindowShouldClose(window)) {
		glClearBufferfv(GL_COLOR, 0, fill);
		glDrawElements(GL_TRIANGLES, ebo_n, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);

		glfwPollEvents();

		std::this_thread::sleep_for(poll_limit_time - (std::chrono::steady_clock::now() - poll_limit_last));
		poll_limit_last += poll_limit_time;
	}

	return 0;
}
