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
#include "collider.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Entity
////////////////////////////////////////////////////////////////////////////////
class Entity
{
public:
	string name;
	Entity* parent;
	float timeScale = 1.0f;

public:
	Entity();
	Entity(const Entity&);
	~Entity();

	template <typename T>
	T* AddComponent()
	{
		static_assert(is_base_of<Component, T>::value, "T must inherit from Component");

		T* component = new T;
		components[typeid(T)] = component;

		RegisterInHierarchy<T>(component);

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

	template <typename T>
	T* GetComponentInAncestor()
	{
		T* it = GetComponent<T>();
		if (it == nullptr && parent != nullptr) {
			return parent->GetComponentInAncestor<T>();
		}
		return it;
	}

	template <typename T>
	pair<Entity*, T*> GetAncestorWithComponent()
	{
		T* it = GetComponent<T>();
		if (it == nullptr && parent != nullptr) {
			return parent->GetAncestorWithComponent<T>();
		}

		if (it != nullptr) {
			return { this, it };
		}
		else {
			return { nullptr, nullptr };
		}
	}

	template <typename T>
	T* GetComponents()
	{
		//components.
	}

	template <typename Base>
	Base* GetFirstComponentOfBase()
	{
		auto it = baseToComponents.find(typeid(Base));
		if (it != baseToComponents.end() && !it->second.empty()) {
			return static_cast<Base*>(it->second.front());
		}
		return nullptr;
	}

	template <typename Base>
	vector<Base*> GetAllComponentsOfBase()
	{
		vector<Base*> result;
		auto it = baseToComponents.find(typeid(Base));

		if (it != baseToComponents.end()) {
			for (Component* comp : it->second) {
				result.push_back(static_cast<Base*>(comp));
			}
		}

		return result;
	}

	void Destroy();
	bool IsDeleting();

	int GetId();

	void AddChild(Entity*);
	Entity* GetChildByName(string, bool);
	vector<Entity*> GetChildrenByName(string, bool);
	vector<Entity*> GetChildren(bool);

	void SetActive(bool);
	bool IsActive();

private:
	int id;
	bool active = true;
	bool deleted = false;

	unordered_map<type_index, Component*> components;
	unordered_map<type_index, vector<Component*>> baseToComponents;
	vector<Entity*> children;

private:
	template <typename T>
	void RegisterInHierarchy(Component* component)
	{
		baseToComponents[typeid(T)].push_back(component);

		if (std::is_base_of<Collider, T>::value) {
			baseToComponents[typeid(Collider)].push_back(component);
		}

		baseToComponents[typeid(Component)].push_back(component);
	}
};

#endif