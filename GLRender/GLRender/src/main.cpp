#define GLEW_STATIC

#include <Windows.h>
//#include <ShellScalingApi.h>
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
//#pragma comment(lib, "Shcore.lib")
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

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

struct CameraDirection
{
	GLenum CubemapFace;
	glm::vec3 Target;
	glm::vec3 Up;
};

CameraDirection gCameraDirections[6] =
{
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f) },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
};


/* LoadShader
	
	- Loads a shader from a specified text file.

*/
GLchar* LoadShader(const std::string& shaderFile) {
	std::ifstream file{ shaderFile.c_str() };

	if (!file.good()) {
		return nullptr;
	}

	std::string file_contents{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

	GLchar* shader = (GLchar*)malloc(file_contents.length() + 1);
	memset(shader, 0, (sizeof(GLchar) * file_contents.length() + 1));

	memcpy(shader, file_contents.c_str(), file_contents.length());

	file.close();

	return shader;
}

void GLAPIENTRY DebugCallbackGL(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "%s\n", message);
	DebugBreak();
}

/* 
	
	Main program body

*/
int main(int argc, char** argv) {

	//SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

	SDL_Init(SDL_INIT_VIDEO);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	SDL_Window* window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	glDebugMessageCallback(DebugCallbackGL, 0);
#endif

	glEnable(GL_DEPTH_TEST);

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

	GLchar* shadowVertexShader = LoadShader("src/shadow_vertex.glsl");

	if (shadowVertexShader == NULL) {
		SDL_GL_DeleteContext(context);
		SDL_Quit();
		return -1;
	}

	GLchar* shadowFragmentShader = LoadShader("src/shadow_fragment.glsl");

	if (shadowFragmentShader == NULL) {
		SDL_GL_DeleteContext(context);
		SDL_Quit();
		return -1;
	}

	GLuint glVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(glVertexShader, 1, &vertexShader, NULL);
	glCompileShader(glVertexShader);

	GLint length;
	glGetShaderiv(glVertexShader, GL_INFO_LOG_LENGTH, &length);
	unsigned char* log = (unsigned char*)malloc(length);

	glGetShaderInfoLog(glVertexShader, 200, &length, (GLchar*)log);

	std::ofstream vertShaderCompileLog;
	vertShaderCompileLog.open("vertexShader.txt", std::ios::out);
	vertShaderCompileLog << log;
	vertShaderCompileLog.close();
	free(log);

	GLint status;
	glGetShaderiv(glVertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		return -2;
	}

	free(vertexShader);


	GLuint glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glFragmentShader, 1, &fragmentShader, NULL);
	glCompileShader(glFragmentShader);

	glGetShaderiv(glFragmentShader, GL_INFO_LOG_LENGTH, &length);
	log = (unsigned char*)malloc(length);

	glGetShaderInfoLog(glFragmentShader, 200, &length, (GLchar*) log);

	std::ofstream fragShaderCompileLog;
	fragShaderCompileLog.open("fragShader.txt", std::ios::out);
	fragShaderCompileLog << log;
	fragShaderCompileLog.close();
	free(log);

	glGetShaderiv(glFragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		return -3;
	}

	free(fragmentShader);

	GLuint glShadowVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(glShadowVertexShader, 1, &shadowVertexShader, NULL);
	glCompileShader(glShadowVertexShader);

	glGetShaderiv(glShadowVertexShader, GL_INFO_LOG_LENGTH, &length);
	log = (unsigned char*)malloc(length);

	glGetShaderInfoLog(glShadowVertexShader, 200, &length, (GLchar*)log);

	std::ofstream shadowVertexShaderCompileLog;
	shadowVertexShaderCompileLog.open("shadowVertexShader.txt", std::ios::out);
	shadowVertexShaderCompileLog << log;
	shadowVertexShaderCompileLog.close();
	free(log);

	glGetShaderiv(glShadowVertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		return -3;
	}

	free(shadowVertexShader);

	GLuint glShadowFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glShadowFragmentShader, 1, &shadowFragmentShader, NULL);
	glCompileShader(glShadowFragmentShader);

	glGetShaderiv(glShadowFragmentShader, GL_INFO_LOG_LENGTH, &length);
	log = (unsigned char*)malloc(length);

	glGetShaderInfoLog(glShadowFragmentShader, 200, &length, (GLchar*)log);

	std::ofstream vertexFragShaderCompileLog;
	vertexFragShaderCompileLog.open("shadowFragShader.txt", std::ios::out);
	vertexFragShaderCompileLog << log;
	vertexFragShaderCompileLog.close();
	free(log);

	glGetShaderiv(glShadowFragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		return -3;
	}

	free(shadowFragmentShader);

	GLuint shaderProgram = glCreateProgram();
	
	glAttachShader(shaderProgram, glVertexShader);
	glAttachShader(shaderProgram, glFragmentShader);
	glLinkProgram(shaderProgram);
	

	GLuint shadowShaderProgram = glCreateProgram();

	glAttachShader(shadowShaderProgram, glShadowVertexShader);
	glAttachShader(shadowShaderProgram, glShadowFragmentShader);
	glLinkProgram(shadowShaderProgram);


	glm::vec4 cameraPos(0, 0.75, 2.0, 1);
	glm::vec4 cameraDirection(0, 0, -1, 1);
	glm::vec4 cameraUp(0, 1, 0, 1);

	glm::vec3 lightPos(-5, 5, 5);
	glm::vec4 lightColor(1, 1, 1, 1);
	glm::float32 lightIntensity = 350.0f;

	glm::vec3 lightPos2(2, 2, -3);
	glm::vec4 lightColor2(0, 0, 1, 1);
	glm::float32 lightIntensity2 = 150.0f;

	glm::mat4 matModel, matView, matProj, matShadowProj;
	matModel = glm::translate(glm::mat4(), glm::vec3(0.0, 0.5, 0.0));
	matModel = glm::scale(matModel, glm::vec3(2.0, 2.0, 2.0));
	matView = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z) + glm::vec3(cameraDirection.x, cameraDirection.y, cameraDirection.z), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));
	matProj = glm::perspective(glm::radians(65.f), aspect, 0.1f, 50.f);
	matShadowProj = glm::perspective(glm::radians(90.f), 1.0f, 0.1f, 50.f);

	glUseProgram(shadowShaderProgram);
	glUniform3f(glGetUniformLocation(shadowShaderProgram, "cameraPosition"), lightPos.x, lightPos.y, lightPos.z);
	

	glUseProgram(shaderProgram);
	GLint uniCameraPos = glGetUniformLocation(shaderProgram, "cameraPosition");
	glUniform3f(uniCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform1i(glGetUniformLocation(shaderProgram, "mtexture"), 1);
	

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(matProj));

	/* Shadow stuff */
	
	// Create the FBO
	GLuint fbo;
	
	glGenFramebuffers(1, &fbo);
	
	// Create the depth texture
	GLuint depth;
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create the cube map
	GLuint shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (GLuint i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, SCREEN_WIDTH, SCREEN_WIDTH, 0, GL_RED, GL_FLOAT, NULL);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);

	// Disable reads from the color buffer
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FB error, status: " << Status << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	/* End of shadow init stuff */


	/* Some garbage program init stuff. Mostly timing for rendering,
		some rudimentary keyboard input stuff..
	*/
	
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

	int blend = 0;


	/* Let's generate some models. */

	Model companion_cube("crate2.obj", uniModel, shaderProgram, shadowShaderProgram, shadowMap);
	companion_cube.translate(glm::vec3(3, 0.5, 2));
	
	Model quake_crate("crate.obj", uniModel, shaderProgram, shadowShaderProgram, shadowMap);
	quake_crate.translate(glm::vec3(-3, 0.5, -2));

	Model room("room.obj", uniModel, shaderProgram, shadowShaderProgram, shadowMap);
	room.translate(glm::vec3(0, 4.5, 0));


	/* Main program loop */
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
				case SDLK_TAB:
					blend++;
					if (blend > 2) { // change this if you want to, for instance, render the shadow map.
						blend = 0;
					}
					glUniform1i(glGetUniformLocation(shaderProgram, "blend"), blend);
					break;
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
				case SDLK_LEFTBRACKET:
					lightPos.y += 0.05;
					lightPos.y = std::min(9.3f, lightPos.y);
					break;
				case SDLK_RIGHTBRACKET:
					lightPos.y -= 0.05;
					lightPos.y = std::max(-0.3f, lightPos.y);
					break;
				case SDLK_EQUALS:
					lightIntensity += 5;
					break;
				case SDLK_MINUS:
					lightIntensity -= 5;
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
		companion_cube.rotate(glm::vec3(0, time / 1000.f * 90.f, 0));
		quake_crate.rotate(glm::vec3(0, time / 1000.f * -90.f, 0));
		glm::mat4 matGround = glm::mat4();
		matView = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z) + glm::vec3(cameraDirection.x, cameraDirection.y, cameraDirection.z), glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));

		ticksSinceRender += time;
		/* If it's been more than 16ms since we rendered, render the scene */
		if (ticksSinceRender >= renderInterval) {
			/* Shadow pass */
			glUseProgram(shadowShaderProgram);
			glCullFace(GL_FRONT);

			glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(matShadowProj));

			glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

			glUniform3f(glGetUniformLocation(shadowShaderProgram, "cameraPosition"), lightPos.x, lightPos.y, lightPos.z);

			for (GLuint i = 0; i < 6; i++) {
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gCameraDirections[i].CubemapFace, shadowMap, 0);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


				glm::mat4 shadowCamera = glm::lookAt(lightPos, lightPos + gCameraDirections[i].Target, gCameraDirections[i].Up);

				glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(shadowCamera));

				//room.DrawShadows();
				companion_cube.DrawShadows();
				quake_crate.DrawShadows();

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			}
			

			/* Color pass */
			glUseProgram(shaderProgram);
			glCullFace(GL_BACK);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
			glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 2);
			
			glUniform3fv(glGetUniformLocation(shaderProgram, "lightPosition"), 1, glm::value_ptr(lightPos));
			glUniform4fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
			glUniform1fv(glGetUniformLocation(shaderProgram, "lightIntensity"), 1, &lightIntensity);

			glUniform3f(uniCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);
			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(matView));
			glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(matProj));
			
			room.Draw();
			companion_cube.Draw();
			quake_crate.Draw();

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