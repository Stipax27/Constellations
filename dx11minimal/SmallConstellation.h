#include "Explosion.cpp"
#include "world.h"
#include "Transform.cpp"
#include "Projectile.cpp"
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
	float speed;
	float damage;
	float rad;
	DWORD LastTime;
	vector<Entity*> effect;

	void Init(World*, Entity*, Entity*);
	void Volley();
	void Lattice();
};

