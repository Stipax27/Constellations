#include "componentSerializers.h"
#include "../../ecsHeaders.h"

using json = nlohmann::json;

std::unordered_map<std::string, std::function<nlohmann::json(Component*)>> serializers;

serializers[typeid(Transform).name()] = [](Component* comp) {
    auto* c = static_cast<Transform*>(comp);
    nlohmann::json j = *c;
    return j;
    };