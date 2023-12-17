#include "glad/glad.h"
#include "GLFW/glfw3.h"

class Window
{
  public:
	GLFWwindow *window;

	Window(std::string title, int width, int height)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		/** Enables macOS compatibility. See here:
		 * <https://www.glfw.org/docs/latest/compat.html#compat_osx>
		 */
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

		if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			return;
		}

		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(1);
		}

		glViewport(0, 0, width, height);
	}
};
