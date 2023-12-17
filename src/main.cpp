#include <iostream>

#include "quill/Quill.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "shaders.hpp"
#include "window.hpp"

void
process_input(GLFWwindow *window);

void
draw_triangle(Shaders &shaders);

quill::Logger *
init_logging(void);

void
framebuffer_size_callback(GLFWwindow *window, int width, int height);

int
main(void)
{
	quill::Logger *logger = init_logging();
	LOG_INFO(logger, "Logging initialization complete with Quill");

	glfwInit();

	Window window = Window("LearnOpenGL", 800, 600);
	LOG_DEBUG(logger, "Window created");

	Shaders shaders = Shaders(std::filesystem::path("./../../shaders"));
	LOG_DEBUG(logger, "Successfully compiled all shaders");

	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window.window)) {
		process_input(window.window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_triangle(shaders);

		// check and call events and swap the buffers
		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void
draw_triangle(Shaders &shaders)
{
	GLuint vertexShaderId = shaders.get("triangle.vert").id;
	std::cout << vertexShaderId << std::endl;
	GLuint fragmentShaderId = shaders.get("triangle.frag").id;
	std::cout << fragmentShaderId << std::endl;

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
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
framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// window is unused
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
