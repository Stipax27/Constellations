////////////////////////////////////////////////////////////////////////////////
// Filename: EntityStorage.cpp
////////////////////////////////////////////////////////////////////////////////
#include "entityStorage.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <typeinfo>

#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/document.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/writer.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/stringbuffer.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/prettywriter.h"

#include <nlohmann/json.hpp>

#include "../Lib/class_name.h"
#include "../Lib/serializer.h"
#include "../Lib/logging.h"

using namespace std;
using namespace rapidjson;
using json = nlohmann::json;

#define SAVE_DIRECTORY "..\\dx11minimal\\Resourses\\EntitySaves\\"
#define EXTENSION ".json"

/////////////////////////////////////////////////////////////////

static string ReadFileToString(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return "";

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static void WriteStringToFile(const string& filepath, const string& content) {
    ofstream file(filepath);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

template<typename T>
static json SerializeComponent(T* component) {
    json j;
    to_json(j, *component);
    return j;
}

// Десериализация компонента (универсальная)
template<typename T>
static void DeserializeComponent(T* component, const json& j) {
    from_json(j, *component);
}

static json SerializeEntity(Entity* entity) {
    json obj;

    obj["name"] = entity->name;
    obj["active"] = entity->IsLocalActive();
    obj["timeScale"] = entity->GetLocalTimeScale();

    // Component serialization
    json componentsArray = json::array();
    for (const auto& pair : entity->GetComponents()) {
        Component* component = pair.second;

        //const std::type_info& type = typeid(*component);
        //componentObj["type"] = type.name(); // или class_name<decltype(component)>().c_str()

        const std::string& typeName = typeid(*component).name();
        auto it = serializer::components.find(typeName);

        if (it != serializer::components.end()) {
            componentsArray.push_back(it->second(component));
        }
        else {
            Log("Missing component serializer in fabrica. Name: ");
            Log(typeName.c_str());
            Log("\n");
        }
    }
    obj["components"] = componentsArray;

    // Children serialization
    json childrenArray = json::array();
    for (Entity* child : entity->GetChildren()) {
        childrenArray.push_back(SerializeEntity(child));
    }
    obj["children"] = childrenArray;

    return obj;
}

static void DeserializeEntity(Entity* entity, const json& j, EntityStorage* storage) {
    if (!j.is_object()) return;

    // Базовые свойства
    if (j.contains("name") && j["name"].is_string()) {
        entity->name = j["name"].get<string>();
    }

    if (j.contains("active") && j["active"].is_boolean()) {
        entity->SetActive(j["active"].get<bool>());
    }

    if (j.contains("timeScale") && j["timeScale"].is_number()) {
        entity->SetTimeScale(j["timeScale"].get<float>());
    }

    // Загружаем компоненты
    if (j.contains("components") && j["components"].is_array()) {
        const auto& componentsArray = j["components"];

        for (const json& compObj : componentsArray) {

        }
    }

    // Загружаем дочерние Entity
    if (j.contains("children") && j["children"].is_array()) {
        const auto& childrenArray = j["children"];

        for (const auto& childJson : childrenArray) {
            Entity* child = storage->CreateEntity("Child", entity);
            DeserializeEntity(child, childJson, storage);
        }
    }
}


/////////////////////////////////////////////////////////////////


EntityStorage::EntityStorage()
{
}


EntityStorage::EntityStorage(const EntityStorage& other)
{
}


EntityStorage::~EntityStorage()
{
}


void EntityStorage::Initialize()
{
    serializer::Initialize();
}


void EntityStorage::Shutdown()
{
	componentEntityCaches.clear();
	if (entities.size() > 0) {
		for (int i = 0; i < entities.size(); i++)
		{
			Entity* entity = entities[i];
			delete entity;
		}
		entities.clear();
	}
}


Entity* EntityStorage::CreateEntity(string Name, Entity* Parent)
{
	Entity* entity = new Entity;
	entity->name = Name;
	entity->SetId(entityCount++);
	entity->localTime = timer::currentTime;
	entity->SetOwnerStorage(this);

	if (Parent != nullptr) {
		Parent->AddChild(entity);
	}

	entities.push_back(entity);

	return entity;
}


Entity* EntityStorage::CloneEntity(Entity* entity)
{
    Entity* clone = CreateEntity(entity->name, entity->GetParent());

    /*for (const auto& pair : entity->GetComponents()) {
        auto* component = pair.second;
    }*/

    return clone;
}


Entity* EntityStorage::GetEntityByName(string Name)
{
	for (Entity* entity : entities) {
		if (entity->name == Name) {
			return entity;
		}
	}

    return nullptr;
}


vector<Entity*> EntityStorage::GetAllEntitiesByName(string Name)
{
	vector<Entity*> array;
	for (Entity* entity : entities) {
		if (entity->name == Name) {
			array.push_back(entity);
		}
	}

	return array;
}


Entity* EntityStorage::GetEntityById(int id)
{
	for (Entity* entity : entities) {
		if (entity->GetId() == id) {
			return entity;
		}
	}

    return nullptr;
}


const vector<Entity*>& EntityStorage::GetEntitiesWithComponent(const type_index& componentType)
{
	auto cacheIt = componentEntityCaches.find(componentType);
	if (cacheIt == componentEntityCaches.end()) {
		cacheIt = componentEntityCaches.emplace(componentType, ComponentEntityCache{}).first;
	}

	ComponentEntityCache& cache = cacheIt->second;
	if (!cache.isBuilt) {
		cache.entities.clear();

		for (Entity* entity : entities) {
			if (entity != nullptr && !entity->IsDeleting() && entity->HasComponent(componentType)) {
				cache.entities.push_back(entity);
			}
		}

		cache.isBuilt = true;
	}

	return cache.entities;
}


void EntityStorage::SaveEntityToFile(Entity* entity, const string& filename) {
    if (!entity) {
        Log("ERROR: Cannot save null entity\n");
        return;
    }

    try {
        // Создаем директорию, если её нет
        filesystem::create_directories(SAVE_DIRECTORY);

        json j = SerializeEntity(entity);

        string fullPath = string(SAVE_DIRECTORY) + filename + EXTENSION;

        ofstream file(fullPath);
        if (!file.is_open()) {
            Log(("ERROR: Cannot open file for writing: " + fullPath + "\n").c_str());
            return;
        }

        file << j.dump(4);
        file.close();

        Log(("Entity saved to: " + fullPath + "\n").c_str());
    }
    catch (const json::exception& e) {
        Log(("JSON serialization error: " + string(e.what()) + "\n").c_str());
    }
    catch (const exception& e) {
        Log(("Error saving entity: " + string(e.what()) + "\n").c_str());
    }
}

Entity* EntityStorage::LoadEntityFromFile(const string& filename) {
    string fullPath = string(SAVE_DIRECTORY) + filename + EXTENSION;

    try {
        if (!filesystem::exists(fullPath)) {
            Log(("ERROR: File not found: " + fullPath + "\n").c_str());
            return nullptr;
        }

        ifstream file(fullPath);
        if (!file.is_open()) {
            Log(("ERROR: Cannot open file for reading: " + fullPath + "\n").c_str());
            return nullptr;
        }

        json j;
        file >> j;
        file.close();

        if (!j.is_object()) {
            Log("ERROR: Invalid JSON format\n");
            return nullptr;
        }

        Entity* entity = CreateEntity();
        DeserializeEntity(entity, j, this);

        Log(("Entity loaded from: " + fullPath + "\n").c_str());
        return entity;

    }
    catch (const json::parse_error& e) {
        Log(("JSON parse error: " + string(e.what()) + "\n").c_str());
        return nullptr;
    }
    catch (const exception& e) {
        Log(("Error loading entity: " + string(e.what()) + "\n").c_str());
        return nullptr;
    }
}


void EntityStorage::CleanMem()
{
	for (size_t i = 0; i < entities.size(); )
	{
		Entity* entity = entities[i];
		if (entity->IsDeleting())
		{
			OnEntityDestroyed(entity);
			delete entity;

			entities[i] = entities.back();
			entities.pop_back();
		}
		else
		{
			i++;
		}
	}
}

void EntityStorage::OnEntityComponentAdded(Entity* entity, const type_index& componentType)
{
	auto cacheIt = componentEntityCaches.find(componentType);
	if (cacheIt == componentEntityCaches.end() || !cacheIt->second.isBuilt) {
		return;
	}

	cacheIt->second.isBuilt = false;
}

void EntityStorage::OnEntityComponentRemoved(Entity* entity, const type_index& componentType)
{
	auto cacheIt = componentEntityCaches.find(componentType);
	if (cacheIt == componentEntityCaches.end() || !cacheIt->second.isBuilt) {
		return;
	}

	cacheIt->second.isBuilt = false;
}

void EntityStorage::OnEntityDestroyed(Entity* entity)
{
	for (auto& cachePair : componentEntityCaches) {
		if (cachePair.second.isBuilt) {
			cachePair.second.isBuilt = false;
		}
	}
}
