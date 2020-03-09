#include <iostream>
//#include <cstdio>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Ground/Ground.h"

const GLuint SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
unsigned char* loadBMP_custom(const char *imagePath, GLuint *width, GLuint *height);

GLfloat lineVertices[] = {
	-1.0f, 0.0f,  1.0f, // 0
	-0.5f, 0.0f,  1.0f, // 1
	 0.0f, 0.0f,  1.0f, // 2
	 0.5f, 0.0f,  1.0f, // 3
 	 1.0f, 0.0f,  1.0f, // 4

	-1.0f, 0.0f,  0.5f, // 5
	 1.0f, 0.0f,  0.5f, // 6

	-1.0f, 0.0f,  0.0f, // 7
	 1.0f, 0.0f,  0.0f, // 8

	-1.0f, 0.0f, -0.5f, // 9
	 1.0f, 0.0f, -0.5f, // 10

    -1.0f, 0.0f, -1.0f, // 11
	-0.5f, 0.0f, -1.0f, // 12
	 0.0f, 0.0f, -1.0f, // 13
	 0.5f, 0.0f, -1.0f, // 14
	 1.0f, 0.0f, -1.0f  // 15
};
GLuint lineIndices[] = { // for "indexed drawing"
	0, 11, 12, 1, 2, 13, 14, 3, 4, 15
};

GLfloat cubeVertices[] = {
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // Top Right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // Top Left
};

GLuint cubeIndices[] = {  // Note that we start from 0!
	0, 1, 3, // First Triangle
	1, 2, 3  // Second Triangle
};

GLfloat texCoords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.5f, 1.0f
};

int main()
{
	printf("Starting GLFW context, OpenGL 3.3\n");
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tracking Viewer", nullptr, nullptr);
	if (window == nullptr) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	GLint nAttribute;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttribute);
	printf("Maximum number of vertex attributes supported: %d\n", nAttribute);

	Ground ground = Ground();
	ground.setScreen(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Uncommenting this call will result in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		ground.Render();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	printf("Key: %d\n", key);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

unsigned char* loadBMP_custom(const char *imagePath, GLuint *width, GLuint *height)
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

	if (imageSize == 0) imageSize = (*width) * (*height) * 3;
	if (dataPos == 0) dataPos = 54;

	data = new unsigned char[imageSize];
	file.read((char*)data, imageSize);
	return data;
}