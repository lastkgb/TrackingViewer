#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
#include <GL/glew.h>; // Include glew to get all the required OpenGL headers
class Shader
{
private:
	// The program ID
	GLuint Program;

public:
	// Constructor reads and builds the shader
	Shader();
	Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath);
	~Shader();

	GLuint getProgram();
	// Use the program
	void Use();
};
