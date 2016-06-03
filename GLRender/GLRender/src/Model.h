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

struct material {
	glm::vec3 ka;
	glm::vec3 kd;
	glm::vec3 ks;
	GLfloat ns;
	GLfloat d;
	GLuint texture;
};

class Model
{
public:
	Model(const std::string& filename, GLint modelTransform, GLint shaderProgram, GLint shadowShaderProgram, GLuint shadowMap);
	~Model();
	void Draw();
	void DrawShadows();

	void rotate(glm::vec3 degrees);
	void translate(glm::vec3 movement);
	void scale(glm::vec3 scale);

private:
	GLuint *mVbo, *mEbo, *mVao;
	GLuint *mShadowVbo, *mShadowEbo, *mShadowVao;
	GLuint mShadowMap;

	GLuint mTextureID;

	GLuint* mTextures;
	GLuint** mMaterialIDs;
	struct material* mMaterialStructs;

	std::vector<tinyobj::shape_t> mShapes;
	std::vector<tinyobj::material_t> mMaterials;
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;

	std::vector<struct vertexData*> mVertexData;
	std::vector<GLuint*> mIndexData;

	void calculateTransform();

	int mTextureWidth, mTextureHeight;
	unsigned char* mTexture;

	GLint mModelTransform;
	GLint mShaderProgram;
	GLint mShadowShaderProgram;

	glm::mat4 matTransform;
	glm::mat4 mNormalMat;

	
	
};

