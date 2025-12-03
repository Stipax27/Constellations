#include "Explosion.cpp"
#include "world.h"
#include "Transform.cpp"
#include "Projectile.cpp"
#include "PhysicBody.cpp"
#include "Constellation.cpp"
#include "timer.h"
#include <cstdlib>
#include <cmath> 

class SmallConstellation
{
private:


public:
	World* m_World;
	Entity* m_entity;
	Entity* m_target;
	DWORD LastTime;
	vector<Entity*> Volleys;
	vector<Entity*> Lattices;

	void Init(World*, Entity*, Entity*);
	void VolleyStart();
	void VolleyUpdate(float deltaTime);
	void LatticeStart();	
	void LatticeUpdate(float deltaTime);
};

