#include "Explosion.cpp"
#include "world.h"
#include "Transform.cpp"
#include "timer.h"
#include <cstdlib> // для rand()
#include <cmath> 

class BaseStar 
{
private:


public:
	World *m_World;
	Entity * m_entity;
	float speed;
	float damage;
	float rad;
	DWORD LastTime;
	vector<Entity*> effect;

	void Init(World*, Entity*);
	void Flash();
	void CoronalEjection();
	void SunWind();
	void FartingEffect();
	void LifeTimeParticl();
};

