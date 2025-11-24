#include "SmallConstellation.h"

void SmallConstellation::Init(World* World, Entity* entity, Entity* target) {
    m_World = World;
    m_entity = entity;
    m_target = target;
    LastTime = timer::currentTime;
}

void SmallConstellation::Volley() {

    point3d startPos = m_entity->GetComponent<Transform>()->position;
    point3d endPos = m_target->GetComponent<Transform>()->position;
    point3d distance = startPos - endPos;
    float totalDistance = distance.magnitude();

    point3d direction = endPos - startPos;
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
    Explosion* throwExplosion = throwEntity->AddComponent<Explosion>();
    
    throwExplosion->max_radius = 1.0f;
}
