#include "cDepthCubeMap.h"
#include <vector>
#include <iostream>
#include "cBasicFlyCamera/cBasicFlyCamera.h"

extern GLuint program;
void RenderSceneDepth(GLuint shadowShaderProgram, float ratio);
void RenderScene(GLuint program, glm::mat4 matProjection, glm::mat4 matView, float ratio, glm::vec3 eyeLocation);
// OpenGL debug callback implementation
//void GLAPIENTRY MyDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
//	GLsizei length, const GLchar* message, const void* userParam) {
//	std::cout << "OpenGL Debug Message: " << message << std::endl;
//}
extern cBasicFlyCamera* g_pFlyCamera;

bool cDepthCubeMap::init(int width, int height, std::string& error)
{

	this->width = width;
	this->height = height;
	bool bFrameBufferIsGoodToGo = true;
	GLenum glError = glGetError();

	// Create the depth buffer
	//glGenTextures(1, &m_depth);
	//glBindTexture(GL_TEXTURE_2D, m_depth);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glBindTexture(GL_TEXTURE_2D, 0);

	// Create the cube map
	glGenTextures(1, &depthCubemapTex_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapTex_ID);

	//float* pArray = new float[this->width * this->height];
	//for (int i = 0; i < this->width * this->height; i++) {
	//	pArray[i] = 1.0f;
	//}

	//GLfloat one = 1.0f;

	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		
		//glClearTexSubImage(depthCubemapTex_ID, 0, 0, 0, 0, this->width, this->height, 1, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	//glClearTexImage(depthCubemapTex_ID, 0, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, &one);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);	// OpenGL 4.1, maybe

	// If there's an OpenGL error, output the error state
	glError = glGetError();
	if (glError != GL_NO_ERROR) {
		error = "OpenGL error line48: " + std::to_string(glError);
		bFrameBufferIsGoodToGo = false;
	}


	// If there's an OpenGL error, output the error state
	glError = glGetError();
	if (glError != GL_NO_ERROR) {
		error = "OpenGL error line57: " + std::to_string(glError);
		bFrameBufferIsGoodToGo = false;
	}

	//glClear(GL_DEPTH_BUFFER_BIT);
	// Create the FBO
	glGenFramebuffers(1, &depthMapFBO_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_ID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthCubemapTex_ID, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->m_depth, 0);
	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);

	// Disable reads from the color buffer
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		bFrameBufferIsGoodToGo = true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		bFrameBufferIsGoodToGo = false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		bFrameBufferIsGoodToGo = false;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		error = "GL_FRAMEBUFFER_UNSUPPORTED";
		bFrameBufferIsGoodToGo = false;
		break;

	default:
		error = "Framebuffer incomplete: Unknown status " + std::to_string(status);
		bFrameBufferIsGoodToGo = false;
		break;
	}

	// If there's an OpenGL error, output the error state
	glError = glGetError();
	if (glError != GL_NO_ERROR) {
		error = "OpenGL error line97: " + std::to_string(glError);
		bFrameBufferIsGoodToGo = false;
	}

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return bFrameBufferIsGoodToGo;
}

void cDepthCubeMap::setupShadowMatrices(GLuint shadowShaderProgram, glm::vec3 lightPos, float near, float far)
{
	float aspect = (float)this->width / (float)this->height;
	//float near = 1.0f;
	//float far = 25.0f; // Adjust based on your scene
	shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);
	//shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, far);
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // +X
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // -X
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0))); // +Y
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0))); // -Y
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0))); // +Z
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))); // -Z

}

void cDepthCubeMap::renderDepthFBO(GLuint shadowShaderProgram, glm::vec3 lightPos, float far)
{
	glViewport(0, 0, this->width, this->height);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_ID);
	// Set uniforms (lightPos, far_plane, shadowMatrices)
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthCubemapTex_ID, 0);

	//glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);

	//GLfloat one = 1.0f;
	//for (unsigned int i = 0; i < 6; i++) {

	//}
	//glClearTexImage(depthCubemapTex_ID, 0, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, &one);

	//for (int face = 0; face < 6; ++face) {
	//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapTex_ID, face);
	glClear(GL_DEPTH_BUFFER_BIT);
	//}


	glUseProgram(shadowShaderProgram);

	// Pass to shader
	for (int i = 0; i < 6; i++) {
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depthCubemapTex_ID, 0);
		glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &shadowTransforms[i][0][0]);
		//glUniform1i(glGetUniformLocation(shadowShaderProgram, "currentFace"), i);
	}
	glUniform3fv(glGetUniformLocation(shadowShaderProgram, "lightPos"), 1, &lightPos[0]);
	glUniform1f(glGetUniformLocation(shadowShaderProgram, "far_plane"), far);
	RenderSceneDepth(shadowShaderProgram, (float)this->width / (float)this->height);


	/*glm::mat4 matProjection = glm::mat4(1.0f);
	glm::mat4 matView = glm::mat4(1.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

	matView = glm::lookAt(
		::g_pFlyCamera->getEyeLocation(),
		::g_pFlyCamera->getTargetLocation(),
		upVector);

	matProjection = glm::perspective(0.6f,
		(float)this->width/(float)this->height,
		1.0f,
		50'000.0f);

	RenderScene(shadowShaderProgram, matProjection, matView, (float)this->width / (float)this->height, g_pFlyCamera->getEyeLocation());*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//bool cDepthCubeMap::init(int width, int height, std::string& error)
//{
//
//	this->width = width;
//	this->height = height;
//	bool bFrameBufferIsGoodToGo = true;
//
//	/*GLenum glError = glGetError();
//	if (glError != GL_NO_ERROR) {
//		error = "OpenGL error line45: " + std::to_string(glError);
//		bFrameBufferIsGoodToGo = false;
//		return bFrameBufferIsGoodToGo;
//	}*/
//	//unsigned int depthMapFBO;
//	glGenFramebuffers(1, &depthMapFBO_ID);
//
//	//unsigned int depthCubemap;
//	glGenTextures(1, &depthCubemapTex_ID);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapTex_ID);
//
//	// Allocate storage for all 6 faces
//	for (unsigned int i = 0; i < 6; ++i) {
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
//			this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	}
//
//	// Set texture parameters
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	// Attach cube map to FBO
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_ID);
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapTex_ID, 0);
//	//glDrawBuffer(GL_NONE); // No color output
//	//glReadBuffer(GL_NONE);
//
//	/*if (glError != GL_NO_ERROR) {
//		error = "OpenGL error line59: " + std::to_string(glError);
//		bFrameBufferIsGoodToGo = false;
//		return bFrameBufferIsGoodToGo;
//	}*/
//	//glEnable(GL_DEBUG_OUTPUT);
//	//glDebugMessageCallback(MyDebugCallback, nullptr);
//	//glDisable(GL_DEBUG_OUTPUT);
//
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//
//	switch (status) {
//	case GL_FRAMEBUFFER_COMPLETE:
//		bFrameBufferIsGoodToGo = true;
//		break;
//
//	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
//		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
//		error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	case GL_FRAMEBUFFER_UNSUPPORTED:
//		error = "GL_FRAMEBUFFER_UNSUPPORTED";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	default:
//		error = "Framebuffer incomplete: Unknown status " + std::to_string(status);
//		bFrameBufferIsGoodToGo = false;
//		break;
//	}
//
//	// If there's an OpenGL error, output the error state
//	GLenum glError = glGetError();
//	if (glError != GL_NO_ERROR) {
//		error = "OpenGL error line97: " + std::to_string(glError);
//		bFrameBufferIsGoodToGo = false;
//	}
//
//	// Point back to default frame buffer
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	return bFrameBufferIsGoodToGo;
//}

//bool cDepthCubeMap::init(int width, int height, std::string& error)
//{
//
//	this->width = width;
//	this->height = height;
//	bool bFrameBufferIsGoodToGo = true;
//
//	/*GLenum glError = glGetError();
//	if (glError != GL_NO_ERROR) {
//		error = "OpenGL error line45: " + std::to_string(glError);
//		bFrameBufferIsGoodToGo = false;
//		return bFrameBufferIsGoodToGo;
//	}*/
//	//unsigned int depthMapFBO;
//	//glGenFramebuffers(1, &depthMapFBO_ID);
//	glCreateFramebuffers(1, &depthMapFBO_ID);
//	//unsigned int depthCubemap;
//	//glGenTextures(1, &depthCubemapTex_ID);
//	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &depthCubemapTex_ID); // DSA way
//	//glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapTex_ID);
//	glTextureStorage2D(depthCubemapTex_ID, 1, GL_DEPTH_COMPONENT32F, this->width, this->height);
//	// Allocate storage for all 6 faces
//	/*for (unsigned int i = 0; i < 6; ++i) {
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F,
//			this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	}*/
//
//	// Set texture parameters
//	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//	glTextureParameteri(depthCubemapTex_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTextureParameteri(depthCubemapTex_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTextureParameteri(depthCubemapTex_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTextureParameteri(depthCubemapTex_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTextureParameteri(depthCubemapTex_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	// Attach cube map to FBO
//	//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_ID);
//	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapTex_ID, 0);
//	glNamedFramebufferTexture(depthMapFBO_ID, GL_DEPTH_ATTACHMENT, depthCubemapTex_ID, 0);
//	//glDrawBuffer(GL_NONE); // No color output
//	//glReadBuffer(GL_NONE);
//	glNamedFramebufferDrawBuffer(depthMapFBO_ID, GL_NONE);
//	glNamedFramebufferReadBuffer(depthMapFBO_ID, GL_NONE);
//
//
//	/*if (glError != GL_NO_ERROR) {
//		error = "OpenGL error line59: " + std::to_string(glError);
//		bFrameBufferIsGoodToGo = false;
//		return bFrameBufferIsGoodToGo;
//	}*/
//	//glEnable(GL_DEBUG_OUTPUT);
//	//glDebugMessageCallback(MyDebugCallback, nullptr);
//	//glDisable(GL_DEBUG_OUTPUT);
//
//	if (glCheckNamedFramebufferStatus(depthMapFBO_ID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//		error = "Framebuffer not complete!";
//		bFrameBufferIsGoodToGo = false;
//		return bFrameBufferIsGoodToGo;
//	}
//
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//
//	switch (status) {
//	case GL_FRAMEBUFFER_COMPLETE:
//		bFrameBufferIsGoodToGo = true;
//		break;
//
//	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
//		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
//		error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	case GL_FRAMEBUFFER_UNSUPPORTED:
//		error = "GL_FRAMEBUFFER_UNSUPPORTED";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	default:
//		error = "Framebuffer incomplete: Unknown status " + std::to_string(status);
//		bFrameBufferIsGoodToGo = false;
//		break;
//	}
//
//	// If there's an OpenGL error, output the error state
//	GLenum glError = glGetError();
//	if (glError != GL_NO_ERROR) {
//		error = "OpenGL error line97: " + std::to_string(glError);
//		bFrameBufferIsGoodToGo = false;
//	}
//
//	// Point back to default frame buffer
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	return bFrameBufferIsGoodToGo;
//}

bool cDepthCubeMap::shutdown(void)
{
	return false;
}



//bool cDepthCubeMap::initTest(int width, int height, std::string& error)
//{
//	unsigned int colorCubemap;
//	glGenTextures(1, &colorCubemap);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, colorCubemap);
//	for (unsigned int i = 0; i < 6; ++i) {
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
//	}
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	// Attach to FBO
//	glGenFramebuffers(1, &depthMapFBO_ID);
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_ID);
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorCubemap, 0); // Color attachment
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapTex_ID, 0);  // Depth attachment
//	glDrawBuffer(GL_COLOR_ATTACHMENT0); // Enable color output
//
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	bool bFrameBufferIsGoodToGo = true;
//	switch (status) {
//	case GL_FRAMEBUFFER_COMPLETE:
//		bFrameBufferIsGoodToGo = true;
//		break;
//
//	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
//		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
//		error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	case GL_FRAMEBUFFER_UNSUPPORTED:
//		error = "GL_FRAMEBUFFER_UNSUPPORTED";
//		bFrameBufferIsGoodToGo = false;
//		break;
//
//	default:
//		error = "Framebuffer incomplete: Unknown status " + std::to_string(status);
//		bFrameBufferIsGoodToGo = false;
//		break;
//	}
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	return bFrameBufferIsGoodToGo;
//}

bool cDepthCubeMap::initTest(int width, int height, std::string& error)
{
	this->width = width;
	this->height = height;

	if (width <= 0 || height <= 0) {
		error = "Invalid dimensions";
		return false;
	}
	GLint maxSize;
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxSize);
	if (width > maxSize || height > maxSize) {
		error = "Exceeds GL_MAX_CUBE_MAP_TEXTURE_SIZE (" + std::to_string(maxSize) + ")";
		return false;
	}

	// Clear any prior errors
	while (glGetError() != GL_NO_ERROR) {}

	// Depth cube map with glTexStorage2D
	glGenTextures(1, &depthCubemapTex_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapTex_ID);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_DEPTH_COMPONENT24, width, height); // 1 level, no mipmaps
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		error = "Depth texture storage failed: OpenGL error " + std::to_string(err);
		glDeleteTextures(1, &depthCubemapTex_ID);
		depthCubemapTex_ID = 0;
		return false;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Color cube map with glTexStorage2D
	glGenTextures(1, &colorCubemap_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, colorCubemap_ID);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, width, height); // 1 level, no mipmaps
	err = glGetError();
	if (err != GL_NO_ERROR) {
		error = "Color texture storage failed: OpenGL error " + std::to_string(err);
		glDeleteTextures(1, &depthCubemapTex_ID);
		glDeleteTextures(1, &colorCubemap_ID);
		depthCubemapTex_ID = 0;
		colorCubemap_ID = 0;
		return false;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Set up FBO
	glGenFramebuffers(1, &depthMapFBO_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_ID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorCubemap_ID, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapTex_ID, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		error = "Framebuffer incomplete: Status " + std::to_string(status);
		glDeleteTextures(1, &depthCubemapTex_ID);
		glDeleteTextures(1, &colorCubemap_ID);
		glDeleteFramebuffers(1, &depthMapFBO_ID);
		depthCubemapTex_ID = 0;
		colorCubemap_ID = 0;
		depthMapFBO_ID = 0;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}



//void cDepthCubeMap::renderDepthFromLightPers(GLuint shadowShaderProgram, glm::vec3 lightPos)
//{
//
//
//	float near_plane = 1.0f, far_plane = 1000.0f;
//	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
//
//	std::vector<glm::mat4> shadowTransforms = {
//		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)), // +X
//		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)), // -X
//		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)), // +Y
//		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)), // -Y
//		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)), // +Z
//		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0))  // -Z
//	};
//
//	glViewport(0, 0, width, height);
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_ID);
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//	glUseProgram(shadowShaderProgram);
//	glUniform3f(glGetUniformLocation(shadowShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
//	glUniform1f(glGetUniformLocation(shadowShaderProgram, "far_plane"), far_plane);
//
//	for (int i = 0; i < 6; ++i) {
//		std::string name = "shadowMatrices[" + std::to_string(i) + "]";
//		GLint shadowMatrices_UL = glGetUniformLocation(shadowShaderProgram, name.c_str());
//		glUniformMatrix4fv(shadowMatrices_UL, 1, GL_FALSE, (const GLfloat*)&shadowTransforms[i]);
//	}
//
//	RenderSceneDepth(shadowShaderProgram); // Render scene from light's POV
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//}

bool cDepthCubeMap::reset(int width, int height, std::string& error)
{
	return false;
}

void cDepthCubeMap::clearDepthBuffer(void)
{
	glClear(GL_DEPTH_BUFFER_BIT);
}
