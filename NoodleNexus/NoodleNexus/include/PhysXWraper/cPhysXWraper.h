#pragma once

//#ifndef _DEBUG
//#define NDEBUG
//#endif // !_DEBUG
//
//#ifdef _DEBUG
//#define OH_NO_
//#endif

// Forward declare for pointer to implementation
class cPhysXWraper_Imp;

#include "cPhysicsObjectTypes.h"

#include <vector>

class cPhysXWraper
{
public:
	cPhysXWraper();
	~cPhysXWraper();

	void initPhysics(bool interactive, std::string PVD_Host_Address = "127.0.0.1");
	void cleanupPhysics(bool interactive);

	void AddBox(std::string name, glm::vec3 halfExtents, glm::vec3 position, glm::vec4 EulerOrientation, float mass);
	void AddSphere(std::string name, float radius, glm::vec3 position, glm::vec4 EulerOrientation, float mass);
	void AddCapsule(std::string name, glm::vec3 position, glm::vec4 EulerOrientation, float mass);


	void update(void);

	void getSceneActors(std::vector<cPhysicsObjectTypes>& vecPhysActors);


	// HACK: Remove this later
	void HACK_Snippet_ShootBall(void);

private:
	cPhysXWraper_Imp* m_pImp = nullptr;
};

