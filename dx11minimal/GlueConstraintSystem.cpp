#include "glueConstraintSystem.h"



GlueConstraintSystem::GlueConstraintSystem()
{
}


void GlueConstraintSystem::Initialize()
{
	entityStorage = Singleton::GetInstance<EntityStorage>();
}


void GlueConstraintSystem::Shutdown()
{
	if (entityStorage)
		entityStorage = 0;
}


void GlueConstraintSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity = entities[i];

		if (!IsEntityValid(entity))
			continue;

		Transform* transform = entity->GetComponent<Transform>();
		GlueConstraint* glueConstraint = entity->GetComponent<GlueConstraint>();
		if (transform != nullptr && glueConstraint != nullptr && glueConstraint->active) {

			Transform worldTrasform = GetWorldTransform(entity);

			int i = 0;
			while (i < glueConstraint->gluedEntities.size()) {
				int id = glueConstraint->gluedEntities[i];

				Entity* gluedEntity = entityStorage->GetEntityById(id);
				if (!IsEntityValid(gluedEntity)) {
					glueConstraint->gluedEntities.erase(glueConstraint->gluedEntities.begin() + i);
					continue;
				}

				Transform* gluedTransform = gluedEntity->GetComponent<Transform>();
				if (gluedTransform == nullptr) {
					glueConstraint->gluedEntities.erase(glueConstraint->gluedEntities.begin() + i);
					continue;
				}

				Transform* upperTransform = gluedEntity->GetUpperComponent<Transform>();
				//Transform delta = GetTransformDelta(upperTransform, upperTransform);

				gluedTransform->position = worldTrasform.position;
				gluedTransform->mRotation = worldTrasform.mRotation;
				gluedTransform->scale = worldTrasform.scale;

				i++;
			}
				
		}

	}
}