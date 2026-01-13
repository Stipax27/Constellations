////////////////////////////////////////////////////////////////////////////////
// Filename: Entity.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Entity.h"

Entity::Entity()
{
}


Entity::Entity(const Entity& other)
{
}


Entity::~Entity()
{
}


void Entity::Destroy() {
	for (int i = 0; i < children.size(); i++) {
		children[i]->Destroy();
	}
	deleted = true;
}


bool Entity::IsDeleting() {
	return deleted;
}


int Entity::GetId() {
	return id;
}


void Entity::AddChild(Entity* Child) {
	children.push_back(Child);
}


Entity* Entity::GetChildByName(string Name, bool Recursive = false) {
	for (int i = 0; i < children.size(); i++) {
		Entity* child = children[i];
		if (child->name == Name) {
			return child;
		}
		else if (Recursive) {
			Entity* descendant = child->GetChildByName(Name, Recursive);
			if (descendant != nullptr)
			{
				return descendant;
			}
		}
	}
}


vector<Entity*> Entity::GetChildrenByName(string Name, bool Recursive = false) {
	vector<Entity*> array;
	for (int i = 0; i < children.size(); i++) {
		Entity* child = children[i];
		if (child->name == Name) {
			array.push_back(child);
		}
		else if (Recursive) {
			vector<Entity*> descendants = child->GetChildrenByName(Name, Recursive);
			if (descendants.size() > 0)
			{
				for (int i = 0; i < descendants.size(); i++) {
					array.push_back(descendants[i]);
				}
			}
		}
	}

	return array;
}


vector<Entity*> Entity::GetChildren(bool Recursive = false) {
	vector<Entity*> array;
	for (int i = 0; i < children.size(); i++) {
		Entity* child = children[i];
		array.push_back(child);

		if (Recursive) {
			vector<Entity*> descendants = child->GetChildren(Recursive);
			if (descendants.size() > 0)
			{
				for (int i = 0; i < descendants.size(); i++) {
					array.push_back(descendants[i]);
				}
			}
		}
	}

	return array;
}


void Entity::SetActive(bool mode) {
	active = mode;
}


bool Entity::IsActive() {
	if (active && !deleted) {
		if (parent != nullptr) {
			return parent->IsActive();
		}

		return true;
	}

	return false;
}