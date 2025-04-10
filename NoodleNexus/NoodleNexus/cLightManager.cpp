#include "cLightManager.h"
#include <float.h>
#include <sstream>	// String stream



cLightManager::cLightManager()
{
	// Clear the lights to a known default state
	for (unsigned int index = 0; index != cLightManager::NUMBEROFLIGHTS; index++)
	{
		this->theLights[index].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		// White coloured light
		this->theLights[index].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		// White specular highlight colour, NOT shiny (4th value of 1.0f)
		this->theLights[index].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		// Reasonable attenuation
		this->theLights[index].atten.x = 0.0f;		// x = constant
		this->theLights[index].atten.y = 0.1f;		// y = linear
		this->theLights[index].atten.z = 0.01f;		// z = quadratic
		this->theLights[index].atten.w = FLT_MAX;	// w = DistanceCutOff (FLT_MAX = infinite)

		this->theLights[index].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		// 0 = pointlight
		// 1 = spot light
		// 2 = directional light
		this->theLights[index].param1.x = 0.0f;		// x = lightType
		this->theLights[index].param1.y = 0.0f;		// y = inner angle
		this->theLights[index].param1.z = 0.0f;		// z = outer angle
		this->theLights[index].param1.w = 1.0f;		// (TDB) "To Be Determined"

		this->theLights[index].param2.x = 0.0f;		// x = 0 for off, 1 for on
		this->theLights[index].param2.y = 0.0f;		// (TDB) 
		this->theLights[index].param2.z = 0.0f;		// (TDB) 
		this->theLights[index].param2.w = 0.0f;		// (TDB) 
	}//for (unsigned int index
}

void cLightManager::loadUniformLocations(GLuint shaderProgram)
{
//	GLint light_00_position_UL = glGetUniformLocation(shaderProgram, "theLights[0].position");
	// and so on...

	//this->theLights[0].position_UL = glGetUniformLocation(shaderProgram, "theLights[0].position");
	//this->theLights[0].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[0].diffuse");
	//this->theLights[0].specular_UL = glGetUniformLocation(shaderProgram, "theLights[0].specular");
	//this->theLights[0].atten_UL = glGetUniformLocation(shaderProgram, "theLights[0].atten");
	//this->theLights[0].direction_UL = glGetUniformLocation(shaderProgram, "theLights[0].direction");
	//this->theLights[0].param1_UL = glGetUniformLocation(shaderProgram, "theLights[0].param1");
	//this->theLights[0].param2_UL = glGetUniformLocation(shaderProgram, "theLights[0].param2");


	//this->theLights[1].position_UL = glGetUniformLocation(shaderProgram, "theLights[1].position");
	//this->theLights[1].diffuse_UL = glGetUniformLocation(shaderProgram, "theLights[1].diffuse");
	//this->theLights[1].specular_UL = glGetUniformLocation(shaderProgram, "theLights[1].specular");
	//this->theLights[1].atten_UL = glGetUniformLocation(shaderProgram, "theLights[1].atten");
	//this->theLights[1].direction_UL = glGetUniformLocation(shaderProgram, "theLights[1].direction");
	//this->theLights[1].param1_UL = glGetUniformLocation(shaderProgram, "theLights[1].param1");
	//this->theLights[1].param2_UL = glGetUniformLocation(shaderProgram, "theLights[1].param2");

	for (int i = 0; i < NUMBEROFLIGHTS; i++) {
		std::string UL_Name = "theLights[" + std::to_string(i) + "].position";
		this->theLights[i].position_UL = glGetUniformLocation(shaderProgram, UL_Name.c_str());

		UL_Name = "theLights[" + std::to_string(i) + "].diffuse";
		this->theLights[i].diffuse_UL = glGetUniformLocation(shaderProgram, UL_Name.c_str());

		UL_Name = "theLights[" + std::to_string(i) + "].specular";
		this->theLights[i].specular_UL = glGetUniformLocation(shaderProgram, UL_Name.c_str());

		UL_Name = "theLights[" + std::to_string(i) + "].atten";
		this->theLights[i].atten_UL = glGetUniformLocation(shaderProgram, UL_Name.c_str());

		UL_Name = "theLights[" + std::to_string(i) + "].direction";
		this->theLights[i].direction_UL = glGetUniformLocation(shaderProgram, UL_Name.c_str());

		UL_Name = "theLights[" + std::to_string(i) + "].param1";
		this->theLights[i].param1_UL = glGetUniformLocation(shaderProgram, UL_Name.c_str());

		UL_Name = "theLights[" + std::to_string(i) + "].param2";
		this->theLights[i].param2_UL = glGetUniformLocation(shaderProgram, UL_Name.c_str());
	}

	// TODO: the rest of the lights... (2, 3, etc.)

	return;
}


void cLightManager::updateShaderWithLightInfo(void)
{
	// Called every frame, updating the light information in the shader


	//glUniform4f(light_00_position_UL, this->theLights[0].position.x,
	//	this->theLights[0].position.y,
	//	this->theLights[0].position.z,
	//	1.0f);

	for (unsigned int index = 0; index != cLightManager::NUMBEROFLIGHTS; index++)
	{
		if (this->theLights[index].param2.x == 1) {
			glUniform4f(
				this->theLights[index].position_UL,
				this->theLights[index].position.x,
				this->theLights[index].position.y,
				this->theLights[index].position.z,
				this->theLights[index].position.w);

			glUniform4f(
				this->theLights[index].diffuse_UL,
				this->theLights[index].diffuse.x,
				this->theLights[index].diffuse.y,
				this->theLights[index].diffuse.z,
				this->theLights[index].diffuse.w);

			glUniform4f(
				this->theLights[index].specular_UL,
				this->theLights[index].specular.x,
				this->theLights[index].specular.y,
				this->theLights[index].specular.z,
				this->theLights[index].specular.w);

			glUniform4f(
				this->theLights[index].atten_UL,
				this->theLights[index].atten.x,
				this->theLights[index].atten.y,
				this->theLights[index].atten.z,
				this->theLights[index].atten.w);

			glUniform4f(
				this->theLights[index].direction_UL,
				this->theLights[index].direction.x,
				this->theLights[index].direction.y,
				this->theLights[index].direction.z,
				this->theLights[index].direction.w);

			glUniform4f(
				this->theLights[index].param1_UL,
				this->theLights[index].param1.x,
				this->theLights[index].param1.y,
				this->theLights[index].param1.z,
				this->theLights[index].param1.w);

			glUniform4f(
				this->theLights[index].param2_UL,
				this->theLights[index].param2.x,
				this->theLights[index].param2.y,
				this->theLights[index].param2.z,
				this->theLights[index].param2.w);
		}
	}//for (unsigned int index...
	return;
}

//void cLightManager::PassShadowMapsToShader()
//{
//	for (int i = 0; i < NUMBEROFLIGHTS; i++) {
//		// Create the light's view matrix
//		glm::mat4 lightViewMatrix = glm::lookAt(this->theLights[i].position, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
//
//		// Create a projection matrix (Orthogonal for directional light or Perspective for point light)
//		glm::mat4 lightProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f); // for directional light
//		// Or for point light:
//		glm::mat4 lightProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
//
//		// Create a combined light-space matrix
//		glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
//
//		// Set up the shadow map pass using this matrix
//		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
//		glClear(GL_DEPTH_BUFFER_BIT);
//
//		// Set the shader for the shadow pass (just need depth output, no color)
//		glUseProgram(shadowShaderID);
//		glUniformMatrix4fv(glGetUniformLocation(shadowShaderID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//
//		// Draw the scene here, just for depth information (without lighting)
//	}
//}

std::string cLightManager::sLight::getState(void)
{
	std::stringstream ssLightState;

	// "position 922.3  8882.9   22.3"
	ssLightState << "position "
		<< this->position.x << " "
		<< this->position.y << " "
		<< this->position.z << std::endl;
	// And so on...

	// return as a string
	return ssLightState.str();
}

bool cLightManager::sLight::loadState(std::string stateString)
{
	std::stringstream ssLightState;
	ssLightState << stateString;

	std::string discard;
	ssLightState >> discard;		// "position"
	ssLightState >> this->position.x;
	ssLightState >> this->position.y;
	ssLightState >> this->position.z;

	// And so on...

	// If it worked
	return true;
}
