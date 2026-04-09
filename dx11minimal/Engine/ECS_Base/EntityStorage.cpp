////////////////////////////////////////////////////////////////////////////////
// Filename: EntityStorage.cpp
////////////////////////////////////////////////////////////////////////////////
#include "entityStorage.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/document.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/writer.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/stringbuffer.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/prettywriter.h"

using namespace rapidjson;

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

static Value Serialize(Entity* entity, Document::AllocatorType& allocator) {
    Value obj(kObjectType);

    obj.AddMember("name", Value(entity->name.c_str(), allocator), allocator);
    obj.AddMember("active", entity->IsLocalActive(), allocator);
    obj.AddMember("timeScale", entity->GetLocalTimeScale(), allocator);

    // Сериализуем компоненты
    //Value componentsArray(kArrayType);

    //for (const auto& pair : components) {
    //    Component* component = pair.second;

    //    Value componentObj(kObjectType);
    //    componentObj.AddMember("type", Value(component->GetTypeName().c_str(), allocator), allocator);

    //    // Получаем данные компонента в виде JSON
    //    Value componentData = component->Serialize(allocator);
    //    componentObj.AddMember("data", componentData, allocator);

    //    componentsArray.PushBack(componentObj, allocator);
    //}

    //obj.AddMember("components", componentsArray, allocator);

    // Рекурсивно сериализуем дочерние Entity
    Value childrenArray(kArrayType);
    for (Entity* child : entity->GetChildren()) {
        childrenArray.PushBack(Serialize(child, allocator), allocator);
    }
    obj.AddMember("children", childrenArray, allocator);

    return obj;
}

static void Deserialize(Entity* entity, const Value& jsonObj) {
    if (!jsonObj.IsObject()) return;

    if (jsonObj.HasMember("name") && jsonObj["name"].IsString()) {
        entity->name = jsonObj["name"].GetString();
    }

    if (jsonObj.HasMember("active") && jsonObj["active"].IsBool()) {
        entity->SetActive(jsonObj["active"].GetBool());
    }

    if (jsonObj.HasMember("timeScale") && jsonObj["timeScale"].IsDouble()) {
        entity->SetTimeScale(jsonObj["timeScale"].GetFloat());
    }

    // Загружаем компоненты
    if (jsonObj.HasMember("components") && jsonObj["components"].IsArray()) {
        const Value& componentsArray = jsonObj["components"];

        for (SizeType i = 0; i < componentsArray.Size(); i++) {
            const Value& compObj = componentsArray[i];

            if (compObj.HasMember("type") && compObj["type"].IsString()) {
                string componentType = compObj["type"].GetString();

                // Здесь нужно создать компонент нужного типа через фабрику
                // Component* newComponent = ComponentFactory::Create(componentType);
                // if (newComponent != nullptr) {
                //     newComponent->Deserialize(compObj["data"]);
                //     components[type_index(typeid(*newComponent))] = newComponent;
                // }
            }
        }
    }

    // Загружаем дочерние Entity
    if (jsonObj.HasMember("children") && jsonObj["children"].IsArray()) {
        const Value& childrenArray = jsonObj["children"];

        for (SizeType i = 0; i < childrenArray.Size(); i++) {
            Entity* child = new Entity();
            Deserialize(child, childrenArray[i]);
            child->SetParent(entity);
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


//void EntityStorage::RemoveEntityByObject(Entity* object)
//{
//	for (int i = 0; i < entities.size(); i++) {
//		Entity* entity = entities[i];
//		if (entity == object) {
//			entity->Destroy();
//			entities.erase(entities.begin() + i);
//			break;
//		}
//	}
//}


Entity* EntityStorage::GetEntityByName(string Name)
{
	for (Entity* entity : entities) {
		if (entity->name == Name) {
			return entity;
		}
	}
}


vector<Entity*> EntityStorage::GetEntitiesByName(string Name)
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
    Document doc;
    doc.SetObject();
    Document::AllocatorType& allocator = doc.GetAllocator();

    Value serialized = Serialize(entity, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    serialized.Accept(writer);

    string fullPath = SAVE_DIRECTORY + filename + EXTENSION;
    WriteStringToFile(fullPath, buffer.GetString());
}

Entity* EntityStorage::LoadEntityFromFile(const string& filename) {
    Entity* entity = CreateEntity();

    string fullPath = SAVE_DIRECTORY + filename + EXTENSION;
    string jsonStr = ReadFileToString(fullPath);
    if (jsonStr.empty()) return nullptr;

    Document doc;
    doc.Parse(jsonStr.c_str());

    if (!doc.HasParseError() && doc.IsObject()) {
        Deserialize(entity, doc);
    }

    return entity;
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
