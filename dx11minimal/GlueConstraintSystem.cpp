#include "glueConstraintSystem.h"



GlueConstraintSystem::GlueConstraintSystem()
{
}


void GlueConstraintSystem::Initialize()
{
}


void GlueConstraintSystem::Shutdown()
{
}


void GlueConstraintSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity = entities[i];
		if (IsEntityValid(entity)) {
			Transform* transform = entity->GetComponent<Transform>();
			GlueConstraint* glueConstraint = entity->GetComponent<GlueConstraint>();
			if (transform != nullptr && glueConstraint != nullptr && glueConstraint->active) {

				for (int id : glueConstraint->gluedEntities) {

				}
				
			}
		}
	}
}