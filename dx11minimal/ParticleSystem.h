#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include "ParticleObject.h"
#include "timer.h"


class ParticleSystem
{
public:
	void Update(float deltaTime)
	{
		float curTime = timer::GetCounter() / 1000;

		for (int i = 0; i < objects.size(); i++)
		{
			ParticleObject* object = objects[i];

			if (object->active)
			{
				int j = 0;

				while (j < object->particles.size())
				{
					Particle* particle = object->particles[j];

					if (curTime - particle->startTime > object->lifetimeMax)
					{
						object->particles.erase(object->particles.begin() + j);
						delete particle;
					}
					else
					{
						j++;
					}
				}

				Particle* particle = new Particle(object->position, curTime);
				object->particles.push_back(particle);
			}

		}
	}

private:
	bool active = false;
	vector<ParticleObject*> objects;
};

#endif