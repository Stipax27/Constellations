#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: entity.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ENTITY_H_
#define _ENTITY_H_

//////////////
// INCLUDES //
//////////////
#include <typeindex>
#include <unordered_map>
#include "component.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Entity
////////////////////////////////////////////////////////////////////////////////
class Entity
{
public:
	string name;
	bool active = true;

public:
	Entity();
	Entity(const Entity&);
	~Entity();

	template <typename T>
	T* AddComponent()
	{
		T* component = new T;
		components[typeid(T)] = component;

		return component;
	}

	template <typename T>
	void RemoveComponent()
	{
		T* component = GetComponent<T>();
		if (component != nullptr)
		{
			delete component;
			components.erase(typeid(T));
		}
	}

	template <typename T>
	T* GetComponent()
	{
		auto it = components.find(typeid(T));
		return (it != components.end()) ? static_cast<T*>(it->second) : nullptr;
	}

	template <typename T>
	bool HasComponent()
	{
		T* component = GetComponent<T>();
		if (component != nullptr)
		{
			return true;
		}
		return false;
	}

	void Destroy();

	int GetId();
	Entity* GetParent();

	void AddChild(Entity*);
	Entity* GetChildByName(string, bool);
	vector<Entity*> GetChildrenByName(string, bool);
	vector<Entity*> GetChildren(bool);

private:
	int id;
	Entity* parent;

	unordered_map<type_index, Component*> components;
	vector<Entity*> children;
};

#endif