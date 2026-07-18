#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include "../ECS_Base/entity.h"
#include "../ECS_Base/component.h"

namespace serializer {
	extern std::unordered_map<std::string, std::function<nlohmann::json(Component*)>> components;

	void Initialize();
}

namespace deserializer {
	extern std::unordered_map<std::string, std::function<void(Entity*, const nlohmann::json&)>> components;

	void Initialize();
}