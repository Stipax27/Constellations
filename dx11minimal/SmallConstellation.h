#include "Explosion.cpp"
#include "world.h"
#include "Transform.cpp"
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

	struct VolleyElement {
		Entity* entity;
		float speed = 10.0f;
		float time = 0.0f;

		bool spiralMovement = false;
		float spiralRadius = 0.0f;
		float spiralSpeed = 0.0f;
		float radiusIncreaseSpeed = 0.0f;
		point3d startPosition;
		point3d direction = point3d(0, 0, 1);
		point3d spiralVector1 = point3d(1, 0, 0);
		point3d spiralVector2 = point3d(0, 1, 0);
	};
	vector<VolleyElement*> VolleyElements;

	struct Lattice {
		Entity* entity;
		point3d direction;
		float speed = 10.0f;
	};
	vector<Lattice*> Lattices;

	//struct TransformationParameter {
	//	point3d LastStarPosition;
	//	point3d CurrentStarPosition;
	//	point3d FutureStarPosition;
	//};
	//vector<TransformationParameter*> TransformationParameters;
	vector<point3d> LastStarsPositions;
	//vector <point3d> CurrentStarsPositions;
	vector <point3d> FutureStarsPositions;
	float TransformationSpeed = 0.5f;
	
	void Init(World*, Entity*, Entity*);
	void VolleyStart();
	void VolleyUpdate(float deltaTime);
	void LatticeStart();	
	void LatticeUpdate(float deltaTime);
	void TransformationStart();
	void TransformationUpdate(float deltaTime);
};

