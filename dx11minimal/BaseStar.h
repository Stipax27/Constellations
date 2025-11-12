#include "Explosion.cpp"
#include "world.h"
#include "Transform.cpp"
#include "timer.h"

class BaseStar 
{
private:

	DWORD LastTime;

public:
	World *m_World;
	Entity * m_entity;
	float speed;
	float damage;
	float rad;

	void Init(World*, Entity*);
	void Flash();
	void CoronalEjection();
	void SunWind();
	
};

