#pragma once

#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>

struct cParticle
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 accel = glm::vec3(0.0f);
	float timeTillDeath = -1.0f;
};

class cParticleEmitter_2
{
public:
	cParticleEmitter_2();
	~cParticleEmitter_2();

	void SetLocationXYZ(glm::vec3 newLocation);
	glm::vec3 GetLocation(void);

	void SetMaximumNumberOfParticles(unsigned int maxParticles);

	// If it's on, then these number of particles will
	//	be constantly created
	// NOTE: This will DELETE all existing particles
	void SetNumberOfParticlesPerFrame(unsigned int numParticlesPerUpdate);

	// This creates these particles once, at one time
	void CreateParticlesImmediately(unsigned int numParticles);

	void SetSourceLocation(glm::vec3 positionXYZ);
	void SetInitalVelocity(glm::vec3 minXYZ, glm::vec3 maxXYZ);
	void TurnOn(void);
	void TurnOff(void);
	bool bIsOn(void);

	void Update(double deltaTime);

	void GetLiveParticles(std::vector< cParticle* >& vec_Particles);

	unsigned int GetNumberOfLiveParticles(void);

	//	b = GetSomething();
	//	GetSomething(&b)
		// There is now a "move operator"
		// But there are some costs, namely that 
		//	you have to sell your soul to the C++ committe
	//	std::vector< cParticle* >&& GetLiveParticles(void);

		// Not good: copying large vector via stack
	//	std::vector< cParticle* > GetLiveParticles(void);
		// See this is managed languages. Never do this in C or C++
	//	std::vector< cParticle* >& GetLiveParticles(void);
		// Might be OK, but:
		//	- likely calls new inside (slow)
		//  - who deletes it??
	//	std::vector< cParticle* >* GetLiveParticles(void);

private:

//	std::vector< cParticle* > vec_Particles;
	// 
	// I'm not usuing a vector here so that I'm not tempted 
	//	to use the methods on the vector, NOT because you 
	//	'need' to use this dynamic array.
	// (i.e. you could use a vector, or whatever container)
	// 
	// The idea is that AREN'T changing the SIZE of the container.
	//
	cParticle* m_pParticles = NULL;
	//
	static const unsigned int DEFAULT_MAX_NUM_PARTICLES = 15'000;
	unsigned int m_MaxNumberOfParticles = DEFAULT_MAX_NUM_PARTICLES;
	//
	// This is only called with SetMaximumNumberOfParticles()
	// (or the c'tor) You aren't calling this regularly
	void m_ResizeContainer(void);

	glm::vec3 m_PositionXYZ = glm::vec3(0.0f);
	glm::vec3 m_minVelocityXYZ = glm::vec3(0.0f);
	glm::vec3 m_maxVelocityXYZ = glm::vec3(0.0f);
	unsigned int m_numParticlesPerUpdate = 0;

	bool m_bIsEnabled = false;

	void m_CreateParticles(unsigned int numParticles);

	float getRandomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}
	glm::vec3 getRandom_vec3(glm::vec3 min, glm::vec3 max)
	{
		return glm::vec3(
			this->getRandomFloat(min.x, max.x),
			this->getRandomFloat(min.y, max.y),
			this->getRandomFloat(min.z, max.z));
	}
};
