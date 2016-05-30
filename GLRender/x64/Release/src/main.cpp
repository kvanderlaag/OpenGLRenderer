#define GLEW_STATIC

#include <Windows.h>
#include <GL/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "SDL/SDL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Model.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "lib/SDL2.lib")
#pragma comment(lib, "lib/SDL2main.lib")
#pragma comment(lib, "lib/SOIL.lib")
#pragma comment(lib, "lib/glew32s.lib")

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>

#include <cstdlib>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;


/* LoadShader
	
	- Loads a shader from a specified text file.

*/
GLchar* LoadShader(const std::string& shaderFile) {
	std::ifstream file;

	file.open(shaderFile.c_str(), std::ios::in);

	if (!file.good()) {
		return nullptr;
	}

	// Get file length;
	file.seekg(0, std::ios::end);
	unsigned long filelength = file.tellg();
	file.seekg(0, std::ios::beg);

	GLchar* shader = (GLchar*)malloc((sizeof(GLchar) * filelength) + 1);
	memset(shader, 0, (sizeof(GLchar) * filelength) + 1);

	for (uint32_t i = 0; i < filelength; ++i) {
		file.read((char*) &shader[i], 1);
	}

	file.close();

	return shader;
}


/*	init_sdl

	- Initializes SDL, creates a GL context, and initializes GLEW for good measure.

*/
void init_sdl(SDL_Window* window) {

}


/* 
	
	Main program body

*/
int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);

	GLfloat cube_vertices[] = {
		// Front face UVs
		// 3,2,0	0,2,1
		//	Vertex			  |		UV		  |		Normal
		-0.5f,	0.5f,	0.5f,	0.0f,	0.0f,	0.0f, 0.0f, -1.0f, // 0  - front top left
		 0.5f,	0.5f,	0.5f,	0.5f,	0.0f,	0.0f, 0.0f, -1.0f, // 1  - front top right
		0.5f,	-0.5f,	0.5f,	0.5f,	1.0f,	0.0f, 0.0f, -1.0f, // 2  - front bottom right
		-0.5f,	-0.5f,	0.5f,	0.0f,	1.0f,	0.0f, 0.0f, -1.0f, // 3  - front bottom left

		// Top face UVs
		// 7,6,4	4,6,5
		//	Vertex			  |		UV		  |		Normal
		-0.5f,	0.5f,	-0.5f,	0.5f,	0.0f,	0.0f, -1.0f, 0.0f, // 4  - back top left
		0.5f,	0.5f,	-0.5f,	1.0f,	0.0f,	0.0f, -1.0f, 0.0f, // 5  - back top right
		0.5f,	0.5f,	0.5f,	1.0f,	1.0f,	0.0f, -1.0f, 0.0f, // 7  - front top right
		-0.5f,	0.5f,	0.5f,	0.5f,	1.0f,	0.0f, -1.0f, 0.0f, // 6  - front top left
				
		// Back face UVs
		// 11,10,8	8,10,9
		//	Vertex			  |		UV		  |		Normal
		0.5f,	0.5f,	-0.5f,	0.0f,	0.0f,	0.0f, 0.0f, 1.0f, // 8  - back top right
		-0.5f,	0.5f,	-0.5f,	0.5f,	0.0f,	0.0f, 0.0f, 1.0f, // 9  - back top left
		-0.5f,	-0.5f,	-0.5f,	0.5f,	1.0f,	0.0f, 0.0f, 1.0f, // 10 - back bottom left
		0.5f,	-0.5f,	-0.5f,	0.0f,	1.0f,	0.0f, 0.0f, 1.0f, // 11 - back bottom right

		// Bottom face UVs
		// 15,14,12		12,14,13
		//	Vertex			  |		UV		  |		Normal
		-0.5f,	-0.5f,	0.5f,	0.5f,	0.0f,	0.0f, 1.0f, 0.0f, // 12 - front bottom left
		0.5f,	-0.5f,	0.5f,	1.0f,	0.0f,	0.0f, 1.0f, 0.0f, // 13 - front bottom right
		0.5f,	-0.5f,	-0.5f,	1.0f,	1.0f,	0.0f, 1.0f, 0.0f, // 14 - back bottom right
		-0.5f,	-0.5f,	-0.5f,	0.5f,	1.0f,	0.0f, 1.0f, 0.0f, // 15 - back bottom left

		// Left face UVs
		// 19,18,16		16,18,17
		//	Vertex			  |		UV		  |		Normal
		-0.5f,	0.5f,	-0.5f,	0.0f,	0.0f,	1.0f, 0.0f, 0.0f, // 16 - back top left
		-0.5f,	0.5f,	0.5f,	0.5f,	0.0f,	1.0f, 0.0f, 0.0f, // 17 - front top left
		-0.5f,	-0.5f,	0.5f,	0.5f,	1.0f,	1.0f, 0.0f, 0.0f, // 18 - front bottom left
		-0.5f,	-0.5f,	-0.5f,	0.0f,	1.0f,	1.0f, 0.0f, 0.0f, // 19 - back bottom left

		// Right face UVs
		//  23,22,20	20,22,21
		//	Vertex			  |		UV		  |		Normal
		0.5f,	0.5f,	0.5f,	0.0f,	0.0f,	-1.0f, 0.0f, 0.0f, // 20 - front top right
		0.5f,	0.5f,	-0.5f,	0.5f,	0.0f,	-1.0f, 0.0f, 0.0f, // 21 - back top right
		0.5f,	-0.5f,	-0.5f,	0.5f,	1.0f,	-1.0f, 0.0f, 0.0f, // 22 - back bottom right
		0.5f,	-0.5f,	0.5f,	0.0f,	1.0f,	-1.0f, 0.0f, 0.0f, // 23 - front bottom right

		// Ground quad
		//	24, 27, 26
		//	Vertex				|		UV			|		Normal
		-10.0f,	-0.5f,	-10.f,		0.0f,	0.0f,		0.0f,	-1.0f,	0.0f, // 24 - back left
		10.0f,	-0.5f,	-10.f,		1.0f,	0.0f,		0.0f,	-1.0f,	0.0f, // 25 - back right
		10.0f,	-0.5f,	10.f,		1.0f,	1.0f,		0.0f,	-1.0f,	0.0f, // 26 - front right
		-10.0f,	-0.5f,	10.f,		0.0f,	1.0f,		0.0f,	-1.0f,	0.0f, // 27 - front left

		// Back quad
		//	28, 31, 30		28, 30, 29
		//	Vertex				|		UV			|		Normal
		-10.0f,	9.5f,	-10.f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f, // 28 - top left
		10.0f,	9.5f,	-10.f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f, // 29 - top right
		10.0f,	-0.5f,	-10.f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f, // 30 - bottom right
		-10.0f,	-0.5f,	-10.f,		0.0f,	1.0f,		0.0f,	0.0f,	-1.0f, // 31 - bottom left

		// Left quad
		//	32, 35, 35		32, 34, 33
		//	Vertex				|		UV			|		Normal
		-10.0f,	9.5f,	10.f,		0.0f,	0.0f,		-1.0f,	0.0f,	0.0f, // 32 - top left
		-10.0f,	9.5f,	-10.f,		1.0f,	0.0f,		-1.0f,	0.0f,	0.0f, // 33 - top right
		-10.0f,	-0.5f,	-10.f,		1.0f,	1.0f,		-1.0f,	0.0f,	0.0f, // 34 - bottom right
		-10.0f,	-0.5f,	10.f,		0.0f,	1.0f,		-1.0f,	0.0f,	0.0f, // 35 - bottom left

		// Right quad
		//	36, 39, 38		36, 38, 37
		//	Vertex				|		UV			|		Normal
		10.0f,	9.5f,	-10.f,		0.0f,	0.0f,		1.0f,	0.0f,	0.0f, // 36 - top left
		10.0f,	9.5f,	10.f,		1.0f,	0.0f,		1.0f,	0.0f,	0.0f, // 37 - top right
		10.0f,	-0.5f,	10.f,		1.0f,	1.0f,		1.0f,	0.0f,	0.0f, // 38 - bottom right
		10.0f,	-0.5f,	-10.f,		0.0f,	1.0f,		1.0f,	0.0f,	0.0f, // 39 - bottom left

		// Front quad
		//	40, 43, 42		40, 42, 41
		//	Vertex				|		UV			|		Normal
		10.0f,	9.5f,	10.f,		0.0f,	0.0f,		0.0f,	0.0f,	1.0f, // 40 - top left
		-10.0f,	9.5f,	10.f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f, // 41 - top right
		-10.0f,	-0.5f,	10.f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f, // 42 - bottom right
		10.0f,	-0.5f,	10.f,		0.0f,	1.0f,		0.0f,	0.0f,	1.0f, // 43 - bottom left
		
		// Top quad
		//	44, 47, 46		44, 46, 45
		//	Vertex				|		UV			|		Normal
		10.0f,	9.5f,	10.f,		0.0f,	0.0f,		0.0f,	1.0f,	0.0f, // 44 - back left
		-10.0f,	9.5f,	10.f,		1.0f,	0.0f,		0.0f,	1.0f,	0.0f, // 45 - back right
		-10.0f,	9.5f,	-10.f,		1.0f,	1.0f,		0.0f,	1.0f,	0.0f, // 46 - front right
		10.0f,	9.5f,	-10.f,		0.0f,	1.0f,		0.0f,	1.0f,	0.0f, // 47 - front left
	};

	GLfloat room_vertices[] = {
		
		// Ground quad
		//	24, 27, 26
		//	Vertex				|		UV			|		Normal
		-10.0f,	-0.5f,	-10.f,		0.0f,	0.0f,		0.0f,	-1.0f,	0.0f, // 24 - back left
		10.0f,	-0.5f,	-10.f,		1.0f,	0.0f,		0.0f,	-1.0f,	0.0f, // 25 - back right
		10.0f,	-0.5f,	10.f,		1.0f,	1.0f,		0.0f,	-1.0f,	0.0f, // 26 - front right
		-10.0f,	-0.5f,	10.f,		0.0f,	1.0f,		0.0f,	-1.0f,	0.0f, // 27 - front left

		// Back quad
		//	28, 31, 30		28, 30, 29
		//	Vertex				|		UV			|		Normal
		-10.0f,	9.5f,	-10.f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f, // 28 - top left
		10.0f,	9.5f,	-10.f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f, // 29 - top right
		10.0f,	-0.5f,	-10.f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f, // 30 - bottom right
		-10.0f,	-0.5f,	-10.f,		0.0f,	1.0f,		0.0f,	0.0f,	-1.0f, // 31 - bottom left

		// Left quad
		//	32, 35, 35		32, 34, 33
		//	Vertex				|		UV			|		Normal
		-10.0f,	9.5f,	10.f,		0.0f,	0.0f,		-1.0f,	0.0f,	0.0f, // 32 - top left
		-10.0f,	9.5f,	-10.f,		1.0f,	0.0f,		-1.0f,	0.0f,	0.0f, // 33 - top right
		-10.0f,	-0.5f,	-10.f,		1.0f,	1.0f,		-1.0f,	0.0f,	0.0f, // 34 - bottom right
		-10.0f,	-0.5f,	10.f,		0.0f,	1.0f,		-1.0f,	0.0f,	0.0f, // 35 - bottom left

		// Right quad
		//	36, 39, 38		36, 38, 37
		//	Vertex				|		UV			|		Normal
		10.0f,	9.5f,	-10.f,		0.0f,	0.0f,		1.0f,	0.0f,	0.0f, // 36 - top left
		10.0f,	9.5f,	10.f,		1.0f,	0.0f,		1.0f,	0.0f,	0.0f, // 37 - top right
		10.0f,	-0.5f,	10.f,		1.0f,	1.0f,		1.0f,	0.0f,	0.0f, // 38 - bottom right
		10.0f,	-0.5f,	-10.f,		0.0f,	1.0f,		1.0f,	0.0f,	0.0f, // 39 - bottom left

		// Front quad
		//	40, 43, 42		40, 42, 41
		//	Vertex				|		UV			|		Normal
		10.0f,	9.5f,	10.f,		0.0f,	0.0f,		0.0f,	0.0f,	1.0f, // 40 - top left
		-10.0f,	9.5f,	10.f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f, // 41 - top right
		-10.0f,	-0.5f,	10.f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f, // 42 - bottom right
		10.0f,	-0.5f,	10.f,		0.0f,	1.0f,		0.0f,	0.0f,	1.0f, // 43 - bottom left

		// Top quad
		//	44, 47, 46		44, 46, 45
		//	Vertex				|		UV			|		Normal
		10.0f,	9.5f,	10.f,		0.0f,	0.0f,		0.0f,	1.0f,	0.0f, // 44 - back left
		-10.0f,	9.5f,	10.f,		1.0f,	0.0f,		0.0f,	1.0f,	0.0f, // 45 - back right
		-10.0f,	9.5f,	-10.f,		1.0f,	1.0f,		0.0f,	1.0f,	0.0f, // 46 - front right
		10.0f,	9.5f,	-10.f,		0.0f,	1.0f,		0.0f,	1.0f,	0.0f // 47 - front left
	};

	GLuint cube_elements[] = {
		//Front 
		3,2,0,
		0,2,1,
		// Top face UVs
		7,6,4,
		4,6,5,
		// Back face UVs
		11,10,8,
		8,10,9,
		// Bottom face UVs
		15,14,12,
		12,14,13,
		// Left face UVs
		19,18,16,
		16,18,17,
		// Right face UVs
		23,22,20,
		20,22,21
	};

	GLuint room_elements[] = {

		// Ground plane
		0, 3, 2,
		0, 2, 1,

		// Back plane
		4, 7, 6,
		4, 6, 5,

		// Left plane
		8, 11, 10,
		8, 10, 9,

		// Right plane
		12, 15, 14,
		12, 14, 13,

		// Front plane
		16, 19, 18,
		16, 18, 17,

		// Roof plane
		20, 23, 22,
		20, 22, 21
	};

	GLchar* vertexShader = LoadShader("src/vertex.glsl");

	if (vertexShader == NULL) {
		SDL_GL_DeleteContext(context);
		SDL_Quit();
		return -1;
	}

	GLchar* fragmentShader = LoadShader("src/fragment.glsl");

	if (fragmentShader == NULL) {
		SDL_GL_DeleteContext(context);
		SDL_Quit();
		return -1;
	}

	

	GLuint glVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(glVertexShader, 1, &vertexShader, NULL);
	glCompileShader(glVertexShader);

	GLint status;
	glGetShaderiv(glVertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		return -2;
	}

	GLuint glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glFragmentShader, 1, &fragmentShader, NULL);
	glCompileShader(glFragmentShader);

	glGetShaderiv(glFragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		return -3;
	}

	GLuint shaderProgram = glCreateProgram();
	
	glAttachShader(shaderProgram, glVertexShader);
	glAttachShader(shaderProgram, glFragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLuint vao[2];
	glGenVertexArrays(2, vao);

	GLuint ebo[2];
	glGenBuffers(2, ebo);

	GLuint vbo[2];
	glGenBuffers(2, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(room_vertices), room_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(room_elements), room_elements, GL_STATIC_DRAW);


	GLuint textures[2];
	glGenTextures(2, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	{
		int width, height;
		unsigned char* image = SOIL_load_image("texture.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	{
		int width, height;
		unsigned char* image = SOIL_load_image("texture2.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	
	glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 1);
	

	for (int i = 0; i < 2; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexCoords");
		GLint normAttrib = glGetAttribLocation(shaderProgram, "normal");
		
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);


		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));


		glEnableVertexAttribArray(normAttrib);
		glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	}

	glm::vec4 cameraPos(0, 0.75, 2.0, 1);
	glm::vec4 cameraDirection(0, 0, -1, 1);
	glm::vec4 cameraUp(0, 1, 0, 1);

	glm::vec3 lightPos(-5, 5, 5);
	glUniform3f(glGetUniformLocation(shaderProgram, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);
	glm::float32 lightIntensity = 200.0f;
	glUniform1f(glGetUniformLocation(shaderProgram, "lightIntensity"), lightIntensity);

	glm::mat4 matModel, matView, matProj;
	matModel = glm::translate(glm::mat4(), glm::vec3(0.0, 0.5, 0.0));
	matModel = glm::scale(matModel, glm::vec3(2.0, 2.0, 2.0));
	matView = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z) + glm::vec3(cameraDirection.x, cameraDirection.y, cameraDirection.z), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));
	matProj = glm::perspective(glm::radians(65.f), aspect, 0.1f, 50.f);

	GLint uniCameraPos = glGetUniformLocation(shaderProgram, "cameraPosition");
	glUniform3f(uniCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(matProj));
	uint32_t t_start, t_now;
	t_start = t_now = SDL_GetTicks();
	uint32_t time = 0;

	uint32_t ticksSinceRender = 0;
	const uint32_t renderInterval = 16;

	bool keysHeld[] = { false, false, false, false,
						false, false, false, false,
						false, false
	};
	const float moveSpeed = 2.0f;
	const float rotateSpeed = 90.f;
	float run = 1;
	glm::vec2 mousemovement;

	Model* model_cube =  new Model("cube.obj", "texture_old.png", uniModel, shaderProgram);

	bool running = true;
	while (running) {
		
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)) {
				running = false;
			}
			if (e.type == SDL_MOUSEMOTION) {
				mousemovement.x = e.motion.xrel;
				mousemovement.y = e.motion.yrel;
			}
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_LSHIFT:
				case SDLK_RSHIFT:
					run = 3;
					break;
				case SDLK_w:
					keysHeld[0] = true;
					break;
				case SDLK_a:
					keysHeld[1] = true;
					break;
				case SDLK_s:
					keysHeld[2] = true;
					break;
				case SDLK_d:
					keysHeld[3] = true;
					break;
				case SDLK_UP:
					keysHeld[4] = true;
					break;
				case SDLK_LEFT:
					keysHeld[5] = true;
					break;
				case SDLK_DOWN:
					keysHeld[6] = true;
					break;
				case SDLK_RIGHT:
					keysHeld[7] = true;
					break;
				case SDLK_q:
					keysHeld[8] = true;
					break;
				case SDLK_e:
				case SDLK_SPACE:
					keysHeld[9] = true;
					break;
				}
			}
			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
				case SDLK_LSHIFT:
				case SDLK_RSHIFT:
					run = 1;
					break;
				case SDLK_w:
					keysHeld[0] = false;
					break;
				case SDLK_a:
					keysHeld[1] = false;
					break;
				case SDLK_s:
					keysHeld[2] = false;
					break;
				case SDLK_d:
					keysHeld[3] = false;
					break;
				case SDLK_UP:
					keysHeld[4] = false;
					break;
				case SDLK_LEFT:
					keysHeld[5] = false;
					break;
				case SDLK_DOWN:
					keysHeld[6] = false;
					break;
				case SDLK_RIGHT:
					keysHeld[7] = false;
					break;
				case SDLK_q:
					keysHeld[8] = false;
					break;
				case SDLK_e:
				case SDLK_SPACE:
					keysHeld[9] = false;
					break;
				}
			}

		}
		if (running == false)
			break;

		t_now = SDL_GetTicks();
		time = t_now - t_start;

		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(cameraDirection.x, cameraDirection.y, cameraDirection.z), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z)));
		
		if (mousemovement.x || mousemovement.y) {
			glm::mat4 cameraRotateVertical = glm::mat4();
			glm::mat4 cameraRotateHorizontal = glm::mat4();
			cameraRotateVertical = glm::rotate(cameraRotateVertical, -mousemovement.y * (time / 1000.f) * glm::radians(rotateSpeed), right);
			cameraRotateHorizontal = glm::rotate(cameraRotateHorizontal, -mousemovement.x * (time / 1000.f) * glm::radians(rotateSpeed), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));
			cameraDirection = cameraRotateVertical * cameraRotateHorizontal * cameraDirection;
			if (mousemovement.x > 0) {
				mousemovement.x -= 1;
			}
			else if (mousemovement.x < 0) {
				mousemovement.x += 1;
			}
			if (mousemovement.y > 0) {
				mousemovement.y -= 1;
			}
			else if (mousemovement.y < 0) {
				mousemovement.y += 1;
			}
		}
		
		
		if (keysHeld[4]) {
			glm::mat4 cameraRotate = glm::mat4();
			cameraRotate = glm::rotate(cameraRotate, (time / 1000.f) * glm::radians(rotateSpeed), right);
			cameraDirection = cameraRotate * cameraDirection;
		}
		if (keysHeld[5]) {
			glm::mat4 cameraRotate = glm::mat4();
			cameraRotate = glm::rotate(cameraRotate, (time / 1000.f) * glm::radians(rotateSpeed), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));
			cameraDirection = cameraRotate * cameraDirection;
		}
		if (keysHeld[6]) {
			glm::mat4 cameraRotate = glm::mat4();
			cameraRotate = glm::rotate(cameraRotate, (time / 1000.f) * glm::radians(-rotateSpeed), right);
			cameraDirection = cameraRotate * cameraDirection;
		}
		if (keysHeld[7]) {
			glm::mat4 cameraRotate = glm::mat4();
			cameraRotate = glm::rotate(cameraRotate, (time / 1000.f) * glm::radians(-rotateSpeed), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));
			cameraDirection = cameraRotate * cameraDirection;
		}
		if (keysHeld[8]) {
			cameraPos += cameraUp * run * -moveSpeed * (time / 1000.f);
		}
		if (keysHeld[9]) {
			cameraPos += cameraUp * run * moveSpeed * (time / 1000.f);
		}
		if (keysHeld[1]) {
			cameraPos.x += run * right.x * -moveSpeed * (time / 1000.f);
			cameraPos.y += run * right.y * -moveSpeed * (time / 1000.f);
			cameraPos.z += run * right.z * -moveSpeed * (time / 1000.f);
		}
		if (keysHeld[0]) {
			cameraPos += cameraDirection * run * moveSpeed * (time / 1000.f);
		}
		if (keysHeld[2]) {
			cameraPos += cameraDirection * run * -moveSpeed * (time / 1000.f);
		}
		if (keysHeld[3]) {
			cameraPos.x += run * right.x * moveSpeed * (time / 1000.f);
			cameraPos.y += run * right.y * moveSpeed * (time / 1000.f);
			cameraPos.z += run * right.z * moveSpeed * (time / 1000.f);
		}

		cameraPos.x = std::max(-9.9f, cameraPos.x);
		cameraPos.x = std::min(9.9f, cameraPos.x);

		cameraPos.y = std::max(-0.49f, cameraPos.y);
		cameraPos.y = std::min(9.49f, cameraPos.y);

		cameraPos.z = std::max(-9.9f, cameraPos.z);
		cameraPos.z = std::min(9.9f, cameraPos.z);


		matModel = glm::rotate(matModel, (float)time / 1000.f * glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 matGround = glm::mat4();
		matView = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z) + glm::vec3(cameraDirection.x, cameraDirection.y, cameraDirection.z), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));



		ticksSinceRender += time;
		if (ticksSinceRender >= renderInterval) {

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			
			glUniform3f(uniCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);

			
			glBindVertexArray(vao[0]);

			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(matView));
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(matModel));

			glBindTexture(GL_TEXTURE_2D, textures[0]);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


			glBindVertexArray(vao[1]);

			glBindTexture(GL_TEXTURE_2D, textures[1]);
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(matGround));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			
			model_cube->Draw();

			ticksSinceRender = 0;
			SDL_GL_SwapWindow(window);
		}
		

		
		if (time > 0)
			t_start = SDL_GetTicks();

	}

	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}