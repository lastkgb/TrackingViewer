#include <iostream>

//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Shader.h"
#include "Ground.h"


unsigned char* loadBmp(const char *imagePath, GLuint *width, GLuint *height);

GLfloat vertices[] = {
	-20.0f, -1.8f, -20.0f,    20.0f, -20.0f,
	 20.0f, -1.8f, -20.0f,    20.0f,  20.0f,
	 20.0f, -1.8f,  20.0f,   -20.0f,  20.0f,
	-20.0f, -1.8f,  20.0f,   -20.0f, -20.0f
};

GLuint indices[] = {
	0, 1, 2,
	0, 3, 2
};

Ground::Ground()
{
	this->shader = Shader("Ground/vertex.shader", "Ground/fragment.shader");

	GLuint VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_INT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	GLuint texWidth, texHeight;
	GLfloat aniso = 0.0f;
	unsigned char *image = loadBmp("Ground/groundTex-box.bmp", &texWidth, &texHeight);
	printf("Texture width: %d, Texture height: %d\n", texWidth, texHeight);
	if (image) {
		printf("BMP image loaded successfully\n");
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST /*GL_LINEAR_MIPMAP_LINEAR*/);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
		printf("Anisotropy: %f\n", aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	printf("Ground constructed successfully\n");
}

Ground::~Ground()
{
	glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
}

void Ground::Render()
{
	shader.Use();
	GLuint program = this->shader.getProgram();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glUniform1i(glGetUniformLocation(program, "groundSampler"), 0);

	glm::mat4 model(1.0), view(1.0), projection(1.0);
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
	projection = glm::perspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.5f, 100.0f);
	GLint modelLoc = glGetUniformLocation(program, "model");
	GLint viewLoc = glGetUniformLocation(program, "view");
	GLint projectLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Ground::setScreen(GLuint width, GLuint height)
{
	screenWidth = width;
	screenHeight = height;
}

unsigned char* loadBmp(const char *imagePath, GLuint *width, GLuint *height)
{
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned char *data;

	ifstream file(imagePath, ios::binary);
	if (!file) {
		printf("Error::Failed to open file - %s\n", imagePath);
		return NULL;
	}
	file.read((char*)header, 54);
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Error::Not a BMP file\n");
		return NULL;
	}

	dataPos = *(int*)&header[0x0A];
	imageSize = *(int*)&header[0x22];
	*width = *(int*)&header[0x12];
	*height = *(int*)&header[0x16];
	printf("Image size: %d\n", imageSize);
	printf("Data position: %d\n", dataPos);
	printf("BMP width: %d\n", *width);
	printf("BMP height: %d\n", *height);

	if (imageSize == 0) imageSize = (*width) * (*height) * 3;
	if (dataPos == 0) dataPos = 54;

	data = new unsigned char[imageSize];
	file.read((char*)data, imageSize);
	/*
	printf("Pixel[0]: %d, %d, %d\n", data[0], data[1], data[2]);
	printf("Pixel[400]: %d, %d, %d\n", data[1200], data[1201], data[1202]);
	printf("Pixel[401]: %d, %d, %d\n", data[1203], data[1204], data[1205]);
	printf("Pixel[402]: %d, %d, %d\n", data[1206], data[1207], data[1208]);
	*/
	file.close();
	return data;
}