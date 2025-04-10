#ifndef _DEPTH_FBO_HG_
#define _DEPTH_FBO_HG_

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>       // for the LASER quaternion lookAt
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class cDepthCubeMap
{
public:
	cDepthCubeMap() :
		depthMapFBO_ID(0),
		depthCubemapTex_ID(0),
		width(-1), height(-1) {};

	GLuint depthCubemapTex_ID, depthMapFBO_ID, colorCubemap_ID;

	GLint width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;
	GLuint m_depth;

	glm::mat4 shadowProj;
	std::vector<glm::mat4> shadowTransforms;

	// Inits the FBP
	bool init(int width, int height, std::string& error);
	bool shutdown(void);

	void setupShadowMatrices(GLuint shadowShaderProgram, glm::vec3 lightPos, float near, float far);

	bool initTest(int width, int height, std::string& error);

	void renderDepthFBO(GLuint shadowShaderProgram, glm::vec3 lightPos, float far);
	//void renderDepthFromLightPers(GLuint shadowShaderProgram, glm::vec3 lightPos);
	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string& error);

	void clearDepthBuffer(void);
};

#endif