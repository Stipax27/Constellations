#include "BaseStar.h"

void BaseStar::Init(World* World , Entity* Entity) {
	m_World = World;
	m_entity = Entity;
}

void BaseStar::Flash()
{
	Entity* entity = m_World->CreateEntity();
	Explosion* explosion = m_entity->AddComponent<Explosion>();
	Transform* transform = m_entity->AddComponent<Transform>();
	transform->position = m_entity->GetComponent<Transform>()->position;
	
}
void BaseStar::CoronalEjection() 
{

}
void BaseStar::SunWind() 
{


}