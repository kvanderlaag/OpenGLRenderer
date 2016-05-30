#pragma once
#include <Windows.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL.h"
#include "tiny_obj_loader.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

struct vertexData {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

class Model
{
public:
	Model(const std::string& filename, const std::string& texturefile, GLint modelTransform, GLint shaderProgram);
	~Model();
	void Draw();

private:
	GLuint *mVbo, *mEbo, *mVao;
	GLuint mTextureID;

	std::vector<tinyobj::shape_t> mShapes;
	std::vector<tinyobj::material_t> mMaterials;
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;

	std::vector<float*> mVertexData;
	std::vector<float*> mIndexData;

	int mTextureWidth, mTextureHeight;
	unsigned char* mTexture;

	GLint mModelTransform;
	GLint mShaderProgram;

	glm::mat4 matTransform;


	
	
};

