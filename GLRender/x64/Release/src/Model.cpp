#include "Model.h"



Model::Model(const std::string& filename, const std::string& texturefile, GLint modelTransform, GLint shaderProgram)
{
	mModelTransform = modelTransform;
	mShaderProgram = shaderProgram;
	std::string error;

	mScale = glm::vec3(2, 2, 2);
	mPosition = glm::vec3(0, 3, 0);
	mRotation = glm::vec3(0, 0, 0);


	matTransform = glm::rotate(glm::mat4(), glm::radians(mRotation.x), glm::vec3(1, 0, 0));
	matTransform = glm::rotate(matTransform, glm::radians(mRotation.y), glm::vec3(0, 1, 0));
	matTransform = glm::rotate(matTransform, glm::radians(mRotation.z), glm::vec3(0, 0, 1));

	matTransform = glm::scale(matTransform, mScale);

	matTransform = glm::translate(matTransform, mPosition);

	bool ret = tinyobj::LoadObj(mShapes, mMaterials, error, filename.c_str());

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

	std::ofstream bufferOutput;
	bufferOutput.open("bufferOutput.txt", std::ios::out);

	for (int i = 0; i < mShapes.size(); ++i) {

		float* tempIndices = (float*)malloc(sizeof(float) * mShapes.at(i).mesh.indices.size());
		mIndexData.push_back(tempIndices);
		for (int j = 0; j < mShapes.at(i).mesh.indices.size(); ++j) {
			tempIndices[j] = mShapes.at(i).mesh.indices.at(j);
		}

		float* tempVertices = (float*) malloc(sizeof(float) * 8 * mShapes.at(i).mesh.positions.size() / 3);
		mVertexData.push_back(tempVertices);

		for (int j = 0; j < mShapes.at(i).mesh.positions.size(); j += 3) {
			
			tempVertices[j / 3 * 8] = mShapes.at(i).mesh.positions.at(j);
			tempVertices[j/3*8 + 1] = mShapes.at(i).mesh.positions.at(j + 1);
			tempVertices[j/3*8 + 2] = mShapes.at(i).mesh.positions.at(j + 2);
			tempVertices[j/3*8 + 5] = mShapes.at(i).mesh.normals.at(j);
			tempVertices[j/3*8 + 6] = mShapes.at(i).mesh.normals.at(j + 1);
			tempVertices[j/3*8 + 7] = mShapes.at(i).mesh.normals.at(j + 2);
			
		}
		for (int j = 0; j < mShapes.at(i).mesh.texcoords.size(); j += 2) {
			
			tempVertices[j/2*8 + 3] = mShapes.at(i).mesh.texcoords.at(j);
			tempVertices[j/2*8 + 4] = mShapes.at(i).mesh.texcoords.at(j + 1);
		
		}

		glGenVertexArrays(1, &mVao[i]);
		glBindVertexArray(mVao[i]);

		glGenBuffers(1, &mVbo[i]);
		glBindBuffer(GL_ARRAY_BUFFER, mVbo[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * mShapes.at(i).mesh.positions.size() / 3, mVertexData.at(i), GL_STATIC_DRAW);

		glGenBuffers(1, &mEbo[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * mShapes.at(i).mesh.indices.size(), mIndexData.at(i), GL_STATIC_DRAW);

		bufferOutput << "mVao: " << mVao[i] << std::endl;
		bufferOutput << "mVbo: " << mVbo[i] << std::endl;
		bufferOutput << "mEbo: " << mEbo[i] << std::endl;
		
		
		

		GLint posAttrib = glGetAttribLocation(mShaderProgram, "position");
		GLint texAttrib = glGetAttribLocation(mShaderProgram, "inTexCoords");
		GLint normAttrib = glGetAttribLocation(mShaderProgram, "normal");

		
		glVertexAttribPointer(glGetAttribLocation(mShaderProgram, "position"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glVertexAttribPointer(glGetAttribLocation(mShaderProgram, "inTexCoords"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(glGetAttribLocation(mShaderProgram, "normal"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

		glEnableVertexAttribArray(glGetAttribLocation(mShaderProgram, "position"));
		glEnableVertexAttribArray(glGetAttribLocation(mShaderProgram, "inTexCoords"));
		glEnableVertexAttribArray(glGetAttribLocation(mShaderProgram, "normal"));
	}

	bufferOutput.close();

	glGenTextures(1, &mTextureID);

	glBindTexture(GL_TEXTURE_2D, mTextureID);

	int width, height;
	mTexture = SOIL_load_image(texturefile.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mTexture);
	
}


Model::~Model()
{
	SOIL_free_image_data(mTexture);
}

void Model::Draw() {
	
	glUniformMatrix4fv(mModelTransform, 1, GL_FALSE, glm::value_ptr(matTransform));

	std::ofstream outputFile;
	outputFile.open("testOutput.txt", std::ios::out);

	if (!outputFile.good()) {
		exit(-1);
	}

	for (int i = 0; i < mShapes.size(); ++i) {
		outputFile << "Shape number " << i << std::endl;
		outputFile << "Shader program: " << mModelTransform << std::endl;
		
		for (int j = 0; j < mShapes.at(0).mesh.positions.size(); j += 3) {
			outputFile << "Vertex: " << (mVertexData.at(i))[j/3*8] << " " << (mVertexData.at(i))[j / 3 * 8 + 1] << " " << (mVertexData.at(i))[j / 3 * 8 + 2] << std::endl;
			outputFile << "Normal: " << (mVertexData.at(i))[j / 3 * 8 + 5] << " " << (mVertexData.at(i))[j / 3 * 8 + 6] << " " << (mVertexData.at(i))[j / 3 * 8 + 7] << std::endl;
			outputFile << "Texture Coordinates: " << (mVertexData.at(i))[j / 3 * 8 + 3] << " " << (mVertexData.at(i))[j / 3 * 8 + 4] << std::endl;
		}

		glBindVertexArray(mVao[i]);
		glBindTexture(GL_TEXTURE_2D, mTextureID);
		glDrawElements(GL_TRIANGLES, mShapes.at(i).mesh.indices.size(), GL_UNSIGNED_INT, 0);
	}
	
	outputFile.close();

}