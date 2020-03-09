#pragma once
class Ground
{
private:
	Shader shader;
	GLuint VAO;
	GLuint texture;
	GLuint screenWidth, screenHeight;
public:
	Ground();
	~Ground();
	void Render();
	void setScreen(GLuint width, GLuint height);
};

