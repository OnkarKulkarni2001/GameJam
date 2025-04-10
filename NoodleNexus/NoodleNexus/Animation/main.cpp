#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>
#include <fstream>
#include <sstream>

#include "Shader.h"
#include "Camera.h"
#include "Animator.h"
#include "Model.h"
#include "../cCharacter.h"

#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastTime = 0.0f;

Animator* g_pAnimator = nullptr;
cCharacter* g_pCharacter = nullptr;

// Global or class scope
static bool wasSpacePressed = false;  // Tracks previous Space key state
static bool isJumping = false;        // Tracks if jump animation is active

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My Skeletal Animation", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialized GLAD" << std::endl;
		return -1;
	}


	//stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	//Shader ourShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");


	Model ourModel("res/Swat/Swat.dae");

	Animation idleAnimation("res/Swat/Breathing Idle.dae", &ourModel);
	Animation leftWalkAnimation("res/Swat/Left Strafe Walking.dae", &ourModel);
	Animation rightWalkAnimation("res/Swat/Right Strafe Walking.dae", &ourModel);
	Animation runAnimation("res/Swat/Standard Run.dae", &ourModel);
	Animation walkBackAnimation("res/Swat/Walking_Backwards.dae", &ourModel);
	Animation walkAnimation("res/Swat/Walking.dae", &ourModel);
	Animation jumpAnimation("res/Swat/Jumping.dae", &ourModel);

	jumpAnimation.SetLooping(&jumpAnimation, false);

	Animator animator(&idleAnimation);

	g_pAnimator = &animator;

	cCharacter character(ourModel);

	g_pCharacter = &character;

	character.AddAnimation(&idleAnimation);			// 0
	character.AddAnimation(&leftWalkAnimation);		// 1
	character.AddAnimation(&rightWalkAnimation);	// 2
	character.AddAnimation(&runAnimation);			// 3
	character.AddAnimation(&walkBackAnimation);		// 4
	character.AddAnimation(&walkAnimation);			// 5
	character.AddAnimation(&jumpAnimation);			// 6

	g_pCharacter->currentAnimation = animator.GetCurrentAnimation();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		processInput(window);
		
		animator.UpdateAnimation(deltaTime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//ourShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		std::vector<glm::mat4> transforms = animator.GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
		{
			ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		}

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		std::stringstream ssTitle;
		//std::string currentAnimation;
		if (g_pCharacter->currentAnimation == &idleAnimation) {
			ssTitle << "CurrentAnimation: Idle";
		}
		if (g_pCharacter->currentAnimation == &walkAnimation && g_pAnimator->GetCurrentAnimationSpeed() == 1.0f) {
			ssTitle << "CurrentAnimation: Walk";
		}
		if (g_pCharacter->currentAnimation == &leftWalkAnimation) {
			ssTitle << "CurrentAnimation: Left";
		}
		if (g_pCharacter->currentAnimation == &rightWalkAnimation) {
			ssTitle << "CurrentAnimation: Right";
		}
		if (g_pCharacter->currentAnimation == &walkBackAnimation) {
			ssTitle << "CurrentAnimation: Back";
		}
		if (g_pCharacter->currentAnimation == &walkAnimation && g_pAnimator->GetCurrentAnimationSpeed() > 1.0f) {
			ssTitle << "CurrentAnimation: Run";
		}
		if (g_pCharacter->currentAnimation == &jumpAnimation) {
			ssTitle << "CurrentAnimation: Jump";
		}

		glfwSetWindowTitle(window, ssTitle.str().c_str());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	// Check WASD and Shift states
	bool wPressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	bool aPressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	bool sPressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	bool dPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	// WASD and Shift animations - play while keys are held
	if (wPressed && !isJumping) {
		g_pCharacter->PlayAnimation(g_pCharacter->vecAnimations[5]);
	}
	else if (aPressed && !isJumping) {
		g_pCharacter->PlayAnimation(g_pCharacter->vecAnimations[1]);
	}
	else if (sPressed && !isJumping) {
		g_pCharacter->PlayAnimation(g_pCharacter->vecAnimations[4]);
	}
	else if (dPressed && !isJumping) {
		g_pCharacter->PlayAnimation(g_pCharacter->vecAnimations[2]);
	}
	else if (shiftPressed && !isJumping) {
		g_pCharacter->PlayAnimation(g_pCharacter->vecAnimations[5], 2.0f);
	}
	else {
		// Play idle animation when no WASD or Shift keys are pressed
		if (!isJumping) {  // Only play idle if not jumping
			g_pCharacter->PlayAnimation(g_pCharacter->vecAnimations[0]);
		}
	}

	// Jump animation - trigger on key press
	bool isSpacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	bool jumpTriggered = isSpacePressed && !wasSpacePressed;

	if (jumpTriggered && !isJumping) {
		g_pCharacter->PlayAnimation(g_pCharacter->vecAnimations[6]);
		isJumping = true;  // Mark jump as active
	}

	// Check if jump animation has completed
	if (isJumping && g_pAnimator->GetCurrentAnimation() == g_pCharacter->vecAnimations[6] &&
		g_pAnimator->GetCurrentTime() >= g_pCharacter->vecAnimations[6]->GetDuration()) {
		isJumping = false;  // Reset jump state when animation completes
	}

	// Update previous Space key state
	wasSpacePressed = isSpacePressed;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
