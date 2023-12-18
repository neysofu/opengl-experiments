#include <iostream>

#include "quill/Quill.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "shaders.hpp"
#include "window.hpp"

struct __attribute__((packed)) Vertex
{
	float x, y, z;
};

void
process_input(GLFWwindow *window);

void
draw_triangle(Shaders &shaders, const Vertex vertices[3]);

quill::Logger *
init_logging(void);

void
framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height);

int32_t
main(void)
{
	quill::Logger *logger = init_logging();
	LOG_INFO(logger, "Logging initialization complete with Quill");

	glfwInit();

	Window window = Window("LearnOpenGL", 800, 600);
	LOG_DEBUG(logger, "Window created");

	GLint max_vertex_attributes_count;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes_count);
	LOG_DEBUG(logger, "Max vertex attributes count: {}", max_vertex_attributes_count);

	Shaders shaders = Shaders(std::filesystem::path("./../../shaders"));
	LOG_DEBUG(logger, "Successfully compiled all shaders");

	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window.window)) {
		process_input(window.window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		const Vertex vertices[] = {
			{ -0.5f, -0.5f, 0.0f }, // left
			{ 0.5f, -0.5f, 0.0f },  // right
			{ 0.0f, 0.5f, 0.0f }    // top
		};
		draw_triangle(shaders, vertices);
		const Vertex vertices2[] = {
			{ 0.0f, 0.5f, 0.0f },
			{ 0.5f, -0.5f, 0.0f },
			{ -0.5f, -0.5f, 0.0f },
		};
		draw_triangle(shaders, vertices2);

		// check and call events and swap the buffers
		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void
draw_triangle(Shaders &shaders, const Vertex vertices[3])
{
	GLuint vertexShaderId = shaders.get("triangle.vert").id;
	GLuint fragmentShaderId = shaders.get("triangle.frag").id;

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(struct Vertex), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)0);
	glEnableVertexAttribArray(0);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragmentShaderId);
	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	char infoLog[512];
	if (!success) {
		// print error
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Failed to link shader program:" << std::endl;
		std::cout << infoLog << std::endl;
		exit(1);
	}
	glUseProgram(shaderProgram);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void
framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height)
{
	(void)window;

	glViewport(0, 0, width, height);
}

void
process_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

quill::Logger *
init_logging(void)
{
	quill::configure([]() {
		/** See Config.h and Tweaks.h for run time and compile time configuration
		 * options */
		quill::Config cfg;
		return cfg;
	}());

	/** Starts the logging backend thread */
	quill::start();

	/** Create a file logger */
	quill::Logger *logger = quill::create_logger(
	  "file_logger", quill::file_handler(std::filesystem::path("example.log"), []() {
		  quill::FileHandlerConfig cfg;
		  cfg.set_open_mode('w');
		  cfg.set_pattern(
		    "[%(ascii_time)] [%(thread)] [%(filename):%(lineno)] [%(logger_name)] "
		    "[%(level_name)] - %(message)",
		    "%H:%M:%S.%Qms");
		  return cfg;
	  }()));

	logger->set_log_level(quill::LogLevel::TraceL3);

	// enable a backtrace that will get flushed when we log CRITICAL
	logger->init_backtrace(2u, quill::LogLevel::Critical);

	return logger;
}
