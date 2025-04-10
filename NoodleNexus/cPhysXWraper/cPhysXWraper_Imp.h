#pragma once

#include <string>
#include <vector>

#include "cPhysicsObjectTypes.h"

class cPhysXWraper_Imp
{
public:
	cPhysXWraper_Imp();
	~cPhysXWraper_Imp();

	void initPhysics(bool interactive, std::string PVD_Host_Address);
	void cleanupPhysics(bool interactive);


	void update(void);

	void getSceneActors(std::vector<cPhysicsObjectTypes>& vecPhysActors);


	// HACK: Remove his later
	void HACK_Snippet_ShootBall(void);
private:

};



