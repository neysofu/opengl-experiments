#include <unordered_map>
#include <filesystem>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

struct CompiledShader
{
	std::string source;
	GLuint id;
};

class Shaders
{
  private:
	std::unordered_map<std::string, CompiledShader> shaders;

  public:
	Shaders(std::filesystem::path path)
	{
		// iterate over all files in the shaders directory
		for (auto &p : std::filesystem::directory_iterator(path)) {
			// get the file extension
			auto ext = p.path().extension().string();

			// if the file extension is not .vert or .frag, skip it
			if (ext != ".vert.glsl" && ext != ".frag.glsl") {
				continue;
			}

			// get the shader type
			auto type = ext == ".vert.glsl" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

			// read the file
			std::ifstream file(p.path());
			std::string source((std::istreambuf_iterator<char>(file)),
			                   std::istreambuf_iterator<char>());

			// compile the shader
			GLuint id = glCreateShader(type);
			const char *csource = source.c_str();
			glShaderSource(id, 1, &csource, NULL);
			glCompileShader(id);

			// check for errors
			int success;
			char infoLog[512];
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);

			if (!success) {
				glGetShaderInfoLog(id, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				          << infoLog << std::endl;
			}

			// add the shader to the map
			shaders[p.path().stem().string()] = { source, id };
		}
	}

	~Shaders()
	{
		for (auto &s : shaders) {
			glDeleteShader(s.second.id);
		}

		shaders.clear();
	}

	// getters
	CompiledShader &get(std::string name) { return shaders[name]; }
};
