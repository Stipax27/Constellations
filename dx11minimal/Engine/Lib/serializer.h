#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include "../ECS_Base/component.h"

namespace serializer {
	extern std::unordered_map<std::string, std::function<nlohmann::json(Component*)>> components;

	void Initialize();
}