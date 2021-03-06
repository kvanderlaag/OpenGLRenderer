#include "Model.h"
#include <algorithm>



Model::Model(const std::string& filename, GLint modelTransform, GLint shaderProgram, GLint shadowShaderProgram, GLuint shadowMap, bool flipNormals)
{
	mModelTransform = modelTransform;
	mShaderProgram = shaderProgram;
	mShadowShaderProgram = shadowShaderProgram;
	mShadowMap = shadowMap;
	std::string error;

	mScale = glm::vec3(1, 1, 1);
	mPosition = glm::vec3(0, 0, 0);
	mRotation = glm::vec3(0, 0, 0);


	calculateTransform();

	bool ret = tinyobj::LoadObj(mShapes, mMaterials, error, filename.c_str(), nullptr, 3u);

	if (!error.empty()) {
		std::cerr << error << std::endl;
	}

	if (!ret) {
		std::cerr << "Error loading model file " << filename << std::endl;
		exit(-11);
	}

	mVao = (GLuint*) malloc(sizeof(GLuint) * mShapes.size());
	mVbo = (GLuint*) malloc(sizeof(GLuint) * mShapes.size());
	mEbo = (GLuint*) malloc(sizeof(GLuint) * mShapes.size());
	mMaterialIDs = (GLuint**)malloc(sizeof(GLuint*) * mShapes.size());
	mMaterialStructs = (struct material*) malloc(sizeof(struct material) * std::max((int) mMaterials.size(), 1));

	int textures = 0;
	for (int i = 0; i < mMaterials.size(); ++i) {
		if (!mMaterials.at(i).diffuse_texname.empty())
			++textures;
	}

	mTextures = (GLuint*)malloc(sizeof(GLuint) * textures);

	char* outputstring = (char*)malloc(sizeof(char) * 100);
	sprintf(outputstring, "Materials: %i\n", mMaterials.size());
	OutputDebugString(outputstring);

	if (mMaterials.size() == 0) {
		struct material m;
		m.ka = glm::vec3(0.1f, 0.1f, 0.1f);
		m.kd = glm::vec3(0.5f, 0.0f, 0.0f);
		m.ks = glm::vec3(1.0f, 1.0f, 1.0f);
		m.d = 1.0f;
		m.ns = 200.f;
		mMaterialStructs[0] = m;
	}

	for (int i = 0; i < mShapes.size(); ++i) {
		for (int j = 0; j < mShapes.at(i).mesh.indices.size(); j += 3) {
			int temp = mShapes.at(i).mesh.indices.at(j + 2);
			mShapes.at(i).mesh.indices.at(j + 2) = mShapes.at(i).mesh.indices.at(j);
			mShapes.at(i).mesh.indices.at(j) = temp;
		}
	}

	for (int i = 0; i < mMaterials.size(); ++i) {
		struct material m;
		m.ka = glm::vec3(mMaterials.at(i).ambient[0], mMaterials.at(i).ambient[1], mMaterials.at(i).ambient[2]);
		m.kd = glm::vec3(mMaterials.at(i).diffuse[0], mMaterials.at(i).diffuse[1], mMaterials.at(i).diffuse[2]);
		m.ks = glm::vec3(mMaterials.at(i).specular[0], mMaterials.at(i).specular[1], mMaterials.at(i).specular[2]);
		m.d = mMaterials.at(i).dissolve;
		m.ns = mMaterials.at(i).shininess;
		if (!mMaterials.at(i).diffuse_texname.empty()) {
			glGenTextures(1, &mTextures[i]);
			glBindTexture(GL_TEXTURE_2D, mTextures[i]);

			int width, height;
			unsigned char* tempTexture = SOIL_load_image(mMaterials.at(i).diffuse_texname.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempTexture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
			SOIL_free_image_data(tempTexture);
			m.texture = mTextures[i];
		}
		mMaterialStructs[i] = m;

	}

	for (int i = 0; i < mShapes.size(); ++i) {

		GLuint* tempIndices = (GLuint*)malloc(sizeof(float) * mShapes.at(i).mesh.indices.size());
		mIndexData.push_back(tempIndices);
		for (int j = 0; j < mShapes.at(i).mesh.indices.size(); ++j) {
			tempIndices[j] = mShapes.at(i).mesh.indices.at(j);
		}

		struct vertexData* tempVertices = (struct vertexData*) malloc(sizeof(struct vertexData) * mShapes.at(i).mesh.positions.size() / 3);
		mVertexData.push_back(tempVertices);

		mMaterialIDs[i] = (GLuint*)malloc(sizeof(GLuint) * mShapes.at(i).mesh.positions.size() / 3);

		for (int j = 0; j < mShapes.at(i).mesh.positions.size(); j += 3) {
			mMaterialIDs[i][j/3] = mShapes.at(i).mesh.material_ids.at(0);
			tempVertices[j/3].position.x = mShapes.at(i).mesh.positions.at(j);
			tempVertices[j/3].position.y = mShapes.at(i).mesh.positions.at(j + 1);
			tempVertices[j/3].position.z = mShapes.at(i).mesh.positions.at(j + 2);
			if (mShapes.at(i).mesh.normals.size() > j) {
				if (flipNormals) {
					tempVertices[j / 3].normal.x = -mShapes.at(i).mesh.normals.at(j);
					tempVertices[j / 3].normal.y = -mShapes.at(i).mesh.normals.at(j + 1);
					tempVertices[j / 3].normal.z = -mShapes.at(i).mesh.normals.at(j + 2);
				}
				else {
					tempVertices[j / 3].normal.x = mShapes.at(i).mesh.normals.at(j);
					tempVertices[j / 3].normal.y = mShapes.at(i).mesh.normals.at(j + 1);
					tempVertices[j / 3].normal.z = mShapes.at(i).mesh.normals.at(j + 2);
				}
			}
			else {
				tempVertices[j / 3].normal = glm::vec3(0, 1, 0);
			}
			
		}
		for (int j = 0; j < mShapes.at(i).mesh.texcoords.size(); j += 2) {
			
			tempVertices[j/2].uv.x = mShapes.at(i).mesh.texcoords.at(j);
			tempVertices[j/2].uv.y = mShapes.at(i).mesh.texcoords.at(j + 1);
		
		}

		glGenVertexArrays(1, &mVao[i]);
		glBindVertexArray(mVao[i]);

		glGenBuffers(1, &mVbo[i]);
		glBindBuffer(GL_ARRAY_BUFFER, mVbo[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(struct vertexData) * mShapes.at(i).mesh.positions.size() / 3, mVertexData.at(i), GL_STATIC_DRAW);

		glGenBuffers(1, &mEbo[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mShapes.at(i).mesh.indices.size(), mIndexData.at(i), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);

	}
	
}


Model::~Model()
{
	free(mVao);
	free(mVbo);
	free(mEbo);
	free(mTextures);
	for (int i = 0; i < mShapes.size(); ++i) {
		free(mMaterialIDs[i]);
	}
	free(mMaterialIDs);
	
	free(mMaterialStructs);
}

void Model::Draw(glm::mat4& viewProj) {
	
	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mModelTransform, 1, GL_FALSE, glm::value_ptr(matTransform));

	glm::mat4 matWVP = viewProj * matTransform;

	glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(mNormalMat));
	glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "worldViewProj"), 1, GL_FALSE, glm::value_ptr(matWVP));


	for (int i = 0; i < mShapes.size(); ++i) {

		glUniform3f(glGetUniformLocation(mShaderProgram, "ka"), mMaterialStructs[0].ka.x, mMaterialStructs[0].ka.y, mMaterialStructs[0].ka.z);
		glUniform3f(glGetUniformLocation(mShaderProgram, "ks"), mMaterialStructs[0].ks.x, mMaterialStructs[0].kd.s, mMaterialStructs[0].ks.z);
		glUniform3f(glGetUniformLocation(mShaderProgram, "kd"), mMaterialStructs[0].kd.x, mMaterialStructs[0].kd.y, mMaterialStructs[0].kd.z);
		glUniform1f(glGetUniformLocation(mShaderProgram, "ns"), mMaterialStructs[0].ns);

		glBindVertexArray(mVao[i]);
		glActiveTexture(GL_TEXTURE1);
		if (mMaterials.size() > 0) {
			glBindTexture(GL_TEXTURE_2D, mTextures[mMaterialIDs[i][0]]);
		}
		glDrawElements(GL_TRIANGLES, mShapes.at(i).mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}

void Model::DrawShadows(glm::mat4& viewProj) {

	glUseProgram(mShadowShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(mShadowShaderProgram, "smodel"), 1, GL_FALSE, glm::value_ptr(matTransform));

	glm::mat4 matWVP = viewProj * matTransform;

	glUniformMatrix4fv(glGetUniformLocation(mShadowShaderProgram, "swvp"), 1, GL_FALSE, glm::value_ptr(matWVP));


	for (int i = 0; i < mShapes.size(); ++i) {
		glBindVertexArray(mVao[i]);
		glDrawElements(GL_TRIANGLES, mShapes.at(i).mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}

void Model::calculateTransform() {

	matTransform = glm::mat4();

	matTransform = glm::translate(matTransform, mPosition);

	matTransform = glm::rotate(matTransform, glm::radians(mRotation.x), glm::vec3(1, 0, 0));
	matTransform = glm::rotate(matTransform, glm::radians(mRotation.y), glm::vec3(0, 1, 0));
	matTransform = glm::rotate(matTransform, glm::radians(mRotation.z), glm::vec3(0, 0, 1));

	matTransform = glm::scale(matTransform, mScale);

	mNormalMat = glm::inverse(glm::scale(glm::mat4(), mScale));

	mNormalMat = glm::rotate(mNormalMat, glm::radians(mRotation.x), glm::vec3(1, 0, 0));
	mNormalMat= glm::rotate(mNormalMat, glm::radians(mRotation.y), glm::vec3(0, 1, 0));
	mNormalMat = glm::rotate(mNormalMat, glm::radians(mRotation.z), glm::vec3(0, 0, 1));

	

}

void Model::rotate(glm::vec3 degrees) {
	mRotation += degrees;
	calculateTransform();
}

void Model::translate(glm::vec3 movement) {
	mPosition += movement;
	calculateTransform();
}

void Model::scale(glm::vec3 scale) {
	mScale = scale;
	calculateTransform();
}