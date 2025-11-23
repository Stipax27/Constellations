#include "SmallConstellation.h"

void SmallConstellation::Init(World* World, Entity* entity, Entity* target) {
    m_World = World;
    m_entity = entity;
    m_target = target;
    LastTime = timer::currentTime;
}

void SmallConstellation::Throw() {

    point3d startPos = m_entity->GetComponent<Transform>()->position;
    point3d endPos = m_target->GetComponent<Transform>()->position;
    point3d distance = startPos - endPos;
    distance.magnitude();

    Entity* throwEntity = m_World->CreateEntity();

    Explosion* throwExplosion = throwEntity->AddComponent<Explosion>();
    
    throwExplosion->explosionType = Explosion::Type::DEFAULT;
    throwExplosion->max_radius = 0.3f;
    throwExplosion->speed = 0.2f;
    throwExplosion->duration = 1.5f;
    throwExplosion->intensity = 0.8f;
    //
}
