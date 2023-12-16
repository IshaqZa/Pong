#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H
#include <string>
#include <glad/glad.h>
#include <fstream>
#include <sstream>

std::string get_file_contents(std::string inputFile);

class Shader {

	public:
		GLuint ID;
		Shader(const char* vertexFile, const char* fragmentFile);
		
		void Activate();
		void Delete();
};



#endif // !SHADER_CLASS_H

