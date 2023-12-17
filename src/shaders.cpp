#include <filesystem>
#include <fstream>

#include "shaders.hpp"

std::string
getFullExtension(const std::filesystem::path &path)
{
	std::string filename = path.filename().string();
	size_t dotPos = filename.find('.');
	if (dotPos == std::string::npos) {
		// No dot found, or it's the first character (hidden files in Unix)
		return "";
	}
	return filename.substr(dotPos);
}

Shaders::Shaders(std::filesystem::path path)
{
	// iterate over all files in the shaders directory
	for (auto &p : std::filesystem::directory_iterator(path)) {
		std::cout << p.path() << std::endl;

		// get the file extension
		auto ext = getFullExtension(p.path());

		// if the file extension is not .vert or .frag, skip it
		if (ext != ".vert.glsl" && ext != ".frag.glsl") {
			std::cout << "Skipping " << p.path() << std::endl;
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

Shaders::~Shaders()
{
	for (auto &s : shaders) {
		glDeleteShader(s.second.id);
	}

	shaders.clear();
}

CompiledShader &
Shaders::get(std::string name)
{
	return shaders[name];
}
