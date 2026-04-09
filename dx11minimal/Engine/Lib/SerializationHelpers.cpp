#include "SerializationMacros.h"
#include "../Types/Point3d.h"

namespace SerializationHelpers {
    // int
    template<>
    rapidjson::Value SerializeValue(const int& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value);
    }

    template<>
    void DeserializeValue(int& value, const rapidjson::Value& jsonValue) {
        if (jsonValue.IsInt()) value = jsonValue.GetInt();
    }

    // float
    template<>
    rapidjson::Value SerializeValue(const float& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value);
    }

    template<>
    void DeserializeValue(float& value, const rapidjson::Value& jsonValue) {
        if (jsonValue.IsFloat()) value = jsonValue.GetFloat();
    }

    // double
    template<>
    rapidjson::Value SerializeValue(const double& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value);
    }

    template<>
    void DeserializeValue(double& value, const rapidjson::Value& jsonValue) {
        if (jsonValue.IsDouble()) value = jsonValue.GetDouble();
    }

    // bool
    template<>
    rapidjson::Value SerializeValue(const bool& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value);
    }

    template<>
    void DeserializeValue(bool& value, const rapidjson::Value& jsonValue) {
        if (jsonValue.IsBool()) value = jsonValue.GetBool();
    }

    // string
    template<>
    rapidjson::Value SerializeValue(const std::string& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value.c_str(), allocator);
    }

    template<>
    void DeserializeValue(std::string& value, const rapidjson::Value& jsonValue) {
        if (jsonValue.IsString()) value = jsonValue.GetString();
    }

    // point3d
    template<>
    rapidjson::Value SerializeValue(const point3d& value, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("x", value.x, allocator);
        obj.AddMember("y", value.y, allocator);
        obj.AddMember("z", value.z, allocator);
        return obj;
    }

    template<>
    void DeserializeValue(point3d& value, const rapidjson::Value& jsonValue) {
        if (jsonValue.IsObject()) {
            if (jsonValue.HasMember("x")) value.x = jsonValue["x"].GetFloat();
            if (jsonValue.HasMember("y")) value.y = jsonValue["y"].GetFloat();
            if (jsonValue.HasMember("z")) value.z = jsonValue["z"].GetFloat();
        }
    }
}