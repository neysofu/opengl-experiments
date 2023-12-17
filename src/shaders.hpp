#include <unordered_map>
#include <iostream>
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
	Shaders(std::filesystem::path path);
	~Shaders();

	CompiledShader &get(std::string name);
};
