#include "SmallConstellation.h"

void SmallConstellation::Init(World* World, Entity* entity, Entity* target) {
    m_World = World;
    m_entity = entity;
    m_target = target;
    LastTime = timer::currentTime;
}

void SmallConstellation::Volley() {

    point3d startPos = m_entity->GetComponent<Transform>()->position;
    point3d targetPoint = m_target->GetComponent<Transform>()->position;
    point3d distance = startPos - targetPoint;

    point3d direction = targetPoint - startPos;
    direction = direction.normalized();  

    Entity* throwEntity = m_World->CreateEntity();

    Projectile* projectile;
    projectile = throwEntity->AddComponent<Projectile>();
    projectile->velocity = direction;
    projectile->speed = 50;

	Constellation* constellation;
	constellation = throwEntity->AddComponent<Constellation>();
	constellation->stars = m_entity->GetComponent<Constellation>()->stars;

    Transform* throwTransform = throwEntity->AddComponent<Transform>();
    throwTransform->position = startPos;
    
}

void SmallConstellation::Lattice() {

    point3d startPos = m_entity->GetComponent<Transform>()->position;
    point3d targetPoint = m_target->GetComponent<Transform>()->position;
    point3d distance = startPos - targetPoint;

    point3d direction = targetPoint - startPos;
    direction = direction.normalized();

    Entity* throwEntity = m_World->CreateEntity();

    Projectile* projectile;
    projectile = throwEntity->AddComponent<Projectile>();
    projectile->velocity = direction;
    projectile->speed = 50;

    Constellation* constellation;
    constellation = throwEntity->AddComponent<Constellation>();
    constellation->stars =
    {
        point3d(0, 0, 2), 

        point3d(-3, 0, 0), 
        point3d(0, 3, 0),
        point3d(3, 0, 0),            
        point3d(0, -3, 0),

        point3d(-2, 2, 0),
        point3d(2, 2, 0),
        point3d(2, -2, 0),
        point3d(-2, -2, 0),

        point3d(-1.5, 0, 1),
        point3d(-1, 1, 1),
        point3d(0, 1.5, 1),
        point3d(1, 1, 1),
        point3d(1.5, 0, 1),
        point3d(1, -1, 1),
        point3d(0, -1.5, 1),
        point3d(-1, -1, 1),

    };
    constellation->links = 
    {
        {0, 1}, {0, 2}, {0, 3}, {0, 4},

        {0, 5}, {0, 6}, {0, 7}, {0, 8},

        {1, 5}, {5, 2}, {2, 6}, {6, 3}, {3, 7}, {7, 4}, {4, 8}, {8, 1},

        {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14}, {14, 15}, {15, 16}, {16, 9}

    };



    Transform* throwTransform = throwEntity->AddComponent<Transform>();
    throwTransform->position = startPos;

}
