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
#include "../BasicComponents/collider.h"

class Entity;
class EntityStorage;
void NotifyEntityComponentAdded(Entity* entity, const std::type_index& componentType);
void NotifyEntityComponentRemoved(Entity* entity, const std::type_index& componentType);
void NotifyEntityDestroyed(Entity* entity);

////////////////////////////////////////////////////////////////////////////////
// Class name: Entity
////////////////////////////////////////////////////////////////////////////////
class Entity
{
public:
	std::string name;
	double localTime;

public:
	Entity();
	Entity(const Entity&);
	~Entity();

	template <typename T>
	T* AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

		T* component = new T;
		components[typeid(T)] = component;

		RegisterInHierarchy<T>(component);
		NotifyEntityComponentAdded(this, typeid(T));

		return component;
	}

	template <typename T>
	void RemoveComponent()
	{
		T* component = GetComponent<T>();
		if (component != nullptr)
		{
			NotifyEntityComponentRemoved(this, typeid(T));
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
		return GetComponent<T>() != nullptr;
	}

	bool HasComponent(const std::type_index& componentType) const
	{
		return components.find(componentType) != components.end();
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
	std::pair<Entity*, T*> GetAncestorWithComponent()
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
	T* GetUpperComponent()
	{
		T* it = GetComponent<T>();
		if (it != nullptr && parent != nullptr) {
			T* upperIt = parent->GetUpperComponent<T>();
			if (upperIt != nullptr) {
				return upperIt;
			}
		}
		return it;
	}

	const std::unordered_map<std::type_index, Component*>& GetComponents()
	{
		return components;
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
	std::vector<Base*> GetAllComponentsOfBase()
	{
		std::vector<Base*> result;
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

	void SetId(int);
	int GetId();

	void SetParent(Entity*);
	Entity* GetParent();

	void AddChild(Entity*);
	Entity* GetChildByName(std::string name, bool recursive = false);
	std::vector<Entity*> GetChildrenByName(std::string name, bool recursive = false);
	std::vector<Entity*> GetChildren(bool recursive = false);

	void SetActive(bool);
	bool IsActive();
	bool IsLocalActive();

	void SetTimeScale(float);
	float GetTimeScale();
	float GetLocalTimeScale();

	void SetOwnerStorage(EntityStorage* storage);
	EntityStorage* GetOwnerStorage();

private:
	int id = -1;
	bool active = true;
	bool deleted = false;

	float timeScale = 1.0f;
	Entity* parent;
	EntityStorage* ownerStorage = nullptr;

	std::unordered_map<std::type_index, Component*> components;
	std::unordered_map<std::type_index, std::vector<Component*>> baseToComponents;
	std::vector<Entity*> children;

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