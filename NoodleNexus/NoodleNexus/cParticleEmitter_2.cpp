#include "cParticleEmitter_2.h"

cParticleEmitter_2::cParticleEmitter_2()
{
	this->m_ResizeContainer();
}

cParticleEmitter_2::~cParticleEmitter_2()
{


}

void cParticleEmitter_2::SetMaximumNumberOfParticles(unsigned int maxParticles)
{
	this->m_MaxNumberOfParticles = maxParticles;
}


void cParticleEmitter_2::SetSourceLocation(glm::vec3 positionXYZ)
{
	this->m_PositionXYZ = positionXYZ;
	return;
}

void cParticleEmitter_2::SetInitalVelocity(glm::vec3 minXYZ, glm::vec3 maxXYZ)
{
	this->m_minVelocityXYZ = minXYZ;
	this->m_maxVelocityXYZ = maxXYZ;
	return;
}

void cParticleEmitter_2::SetNumberOfParticlesPerFrame(unsigned int numParticlesPerUpdate)
{
	this->m_numParticlesPerUpdate = numParticlesPerUpdate;
	return;
}

void cParticleEmitter_2::Update(double deltaTime)
{
	// Every tick, create X number of particles and add to the vector
	if (this->m_bIsEnabled)
	{
		this->m_CreateParticles(this->m_numParticlesPerUpdate);
	}

	for (unsigned int index = 0; index != this->m_MaxNumberOfParticles; index++)
	{
		cParticle* pTestParticle = &(this->m_pParticles[index]);

		if (pTestParticle->timeTillDeath > 0.0f)
		{
			pTestParticle->timeTillDeath -= (float)deltaTime;

			// Euler integration
			pTestParticle->velocity += pTestParticle->accel * (float)deltaTime;

			pTestParticle->position += pTestParticle->velocity * (float)deltaTime;
		}
	}//for (unsigned int index





	return;
}

void cParticleEmitter_2::m_ResizeContainer(void)
{
	// Delete all the old particles
	delete[] this->m_pParticles;

	// Allocate a new one
	this->m_pParticles = new cParticle[this->m_MaxNumberOfParticles];

	return;
}


void cParticleEmitter_2::m_CreateParticles(unsigned int numParticles)
{
	// Rather than creating (new) and adding them to the vector,
	//	we are going to scan through the array of possible particles
	//	and "revive" and "dead" ones we find

	unsigned int numParticlesToCreate = numParticles;
	unsigned int particleIndex = 0;
	do
	{
		// is this particle dead? 
		// NOTE: Be careful about the "&" 
		// If you don't do this (or use a pointer, etc.)
		//	then you are working with the COPY, the actual particle.
		cParticle& testParticle = this->m_pParticles[particleIndex];

		if (testParticle.timeTillDeath <= 0.0f)
		{
			// Yes. So "revive" it
			testParticle.velocity = this->getRandom_vec3(this->m_minVelocityXYZ, this->m_maxVelocityXYZ);

			float minLife = 3.0f;
			float maxLife = 10.0f;
			testParticle.timeTillDeath = this->getRandomFloat(minLife, maxLife);
			testParticle.position = this->m_PositionXYZ;
			testParticle.accel = glm::vec3(0.0f, -1.0f, 0.0f);

			numParticlesToCreate--;
		}

		particleIndex++;

	} while ( (numParticlesToCreate > 0) && (particleIndex < this->m_MaxNumberOfParticles) );


	return;
}



void cParticleEmitter_2::CreateParticlesImmediately(unsigned int numParticles)
{
	this->m_CreateParticles(numParticles);
	return;
}


void cParticleEmitter_2::GetLiveParticles(std::vector< cParticle* >& vec_Particles)
{
	vec_Particles.clear();

	// Copy any "live" particles to this vector we passed in
	for (unsigned int index = 0; index != this->m_MaxNumberOfParticles; index++)
	{
		cParticle* pTestParticle = &(this->m_pParticles[index]);

		if (pTestParticle->timeTillDeath > 0.0f)
		{
			// It's alive
			vec_Particles.push_back(pTestParticle);
		}
	}

	return;
}

unsigned int cParticleEmitter_2::GetNumberOfLiveParticles(void)
{
	unsigned int numParticles = 0;

	for (unsigned int index = 0; index != this->m_MaxNumberOfParticles; index++)
	{
		cParticle* pTestParticle = &(this->m_pParticles[index]);

		if (pTestParticle->timeTillDeath > 0.0f)
		{
			// It's alive
			numParticles++;
		}
	}
	return numParticles;
}

void cParticleEmitter_2::SetLocationXYZ(glm::vec3 newLocation)
{
	this->m_PositionXYZ = newLocation;
	return;
}

glm::vec3 cParticleEmitter_2::GetLocation(void)
{
	return this->m_PositionXYZ;
}

void cParticleEmitter_2::TurnOn(void)
{
	this->m_bIsEnabled = true;
	return;
}

void cParticleEmitter_2::TurnOff(void)
{
	this->m_bIsEnabled = false;
	return;
}
bool cParticleEmitter_2::bIsOn(void)
{
	return this->m_bIsEnabled;
}