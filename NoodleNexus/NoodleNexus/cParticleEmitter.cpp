#include "cParticleEmitter.h"

cParticleEmitter::cParticleEmitter()
{
}

cParticleEmitter::~cParticleEmitter()
{


}

//void cParticleEmitter::SetMaximumNumberOfParticles(unsigned int maxParticles)


void cParticleEmitter::SetSourceLocation(glm::vec3 positionXYZ)
{
	this->m_PositionXYZ = positionXYZ;
	return;
}

void cParticleEmitter::SetInitalVelocity(glm::vec3 minXYZ, glm::vec3 maxXYZ)
{
	this->m_minVelocityXYZ = minXYZ;
	this->m_maxVelocityXYZ = maxXYZ;
	return;
}

void cParticleEmitter::SetNumberOfParticlesPerFrame(unsigned int numParticlesPerUpdate)
{
	this->m_numParticlesPerUpdate = numParticlesPerUpdate;
	return;
}

void cParticleEmitter::Update(double deltaTime)
{
	// Every tick, create X number of particles and add to the vector
	if (this->m_bIsEnabled)
	{
		this->m_CreateParticles(this->m_numParticlesPerUpdate);
	}

	for (cParticle* pCurPart : this->vec_Particles)
	{
		if (pCurPart != NULL)
		{
			pCurPart->timeTillDeath -= (float)deltaTime;

			// Euler integration
			pCurPart->velocity += pCurPart->accel * (float)deltaTime;

			pCurPart->position += pCurPart->velocity * (float)deltaTime;


		}//if (pCurPart != NULL)


	}//for (cParticle* pCurPar


//	// Go through and delete any "dead" particles
//	for (std::vector< cParticle* >::iterator itParticle = this->vec_Particles.begin(); 
//		 itParticle != this->vec_Particles.end(); itParticle++)
//	{
//		// Is it dead?
//		if ((*itParticle)->timeTillDeath <= 0.0f)
//		{
//			delete (*itParticle);
//			// 
//			// Not only is this very slow, it's also "dangerous"
//			//	because the iterator comparison to the end is now messed
//			this->vec_Particles.erase(itParticle);
//		}
//	}


	std::vector< cParticle* > vecCopy = this->vec_Particles;

	this->vec_Particles.clear();

	for (cParticle* pCurPart : vecCopy)
	{
		if (pCurPart->timeTillDeath > 0.0f)
		{
			// Copy it back
			this->vec_Particles.push_back(pCurPart);
		}
		else
		{
			// it's dead
			delete pCurPart;
		}
	}


	return;
}

void cParticleEmitter::m_CreateParticles(unsigned int numParticles)
{

	for (unsigned int count = 0; count != numParticles; count++)
	{
		cParticle* pNewParticle = new cParticle();
		// 
		pNewParticle->velocity = this->getRandom_vec3(this->m_minVelocityXYZ, this->m_maxVelocityXYZ);

		float minLife = 3.0f;
		float maxLife = 10.0f;
		pNewParticle->timeTillDeath = this->getRandomFloat(minLife, maxLife);
		pNewParticle->position = this->m_PositionXYZ;
		pNewParticle->accel = glm::vec3(0.0f, -1.0f, 0.0f);

		this->vec_Particles.push_back(pNewParticle);
	}


	return;
}



void cParticleEmitter::CreateParticlesImmediately(unsigned int numParticles)
{
	this->m_CreateParticles(numParticles);
	return;
}


void cParticleEmitter::GetLiveParticles(std::vector< cParticle* >& vec_Particles)
{
	vec_Particles.clear();

	// Copy any "live" particles to this vector we passed in
	for (cParticle* pTestParticle : this->vec_Particles)
	{
		if (pTestParticle != NULL)
		{
			// Alive?
			if (pTestParticle->timeTillDeath > 0.0f)
			{
				vec_Particles.push_back(pTestParticle);
			}
		}
	}

	return;
}

void cParticleEmitter::SetLocationXYZ(glm::vec3 newLocation)
{
	this->m_PositionXYZ = newLocation;
	return;
}

glm::vec3 cParticleEmitter::GetLocation(void)
{
	return this->m_PositionXYZ;
}

void cParticleEmitter::TurnOn(void)
{
	this->m_bIsEnabled = true;
	return;
}

void cParticleEmitter::TurnOff(void)
{
	this->m_bIsEnabled = false;
	return;
}
bool cParticleEmitter::bIsOn(void)
{
	return this->m_bIsEnabled;
}