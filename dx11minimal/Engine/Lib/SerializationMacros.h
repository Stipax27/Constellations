#ifndef _SERIALIZATION_MACROS_H_
#define _SERIALIZATION_MACROS_H_

#include <string>
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/document.h"

// Макрос для объявления методов сериализации в классе
#define DECLARE_SERIALIZABLE(ClassName) \
public: \
    virtual std::string GetTypeName() const override { return #ClassName; } \
    virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override; \
    virtual void Deserialize(const rapidjson::Value& data) override;

// Макрос для начала определения сериализации
#define BEGIN_SERIALIZATION(ClassName) \
    rapidjson::Value ClassName::Serialize(rapidjson::Document::AllocatorType& allocator) { \
        using namespace rapidjson; \
        Value obj(kObjectType); \
        obj.AddMember("__type", Value(GetTypeName().c_str(), allocator), allocator);

// Макрос для сериализации поля
#define SERIALIZE_FIELD(fieldName) \
        obj.AddMember(#fieldName, SerializeValue(fieldName, allocator), allocator);

// Макрос для окончания определения сериализации
#define END_SERIALIZATION() \
        return obj; \
    }

// Макрос для начала десериализации
#define BEGIN_DESERIALIZATION(ClassName) \
    void ClassName::Deserialize(const rapidjson::Value& data) { \
        if (!data.IsObject()) return;

// Макрос для десериализации поля
#define DESERIALIZE_FIELD(fieldName) \
        if (data.HasMember(#fieldName)) { \
            DeserializeValue(fieldName, data[#fieldName]); \
        }

// Макрос для окончания десериализации
#define END_DESERIALIZATION() \
    }

// Вспомогательные функции для разных типов
namespace SerializationHelpers {
    template<typename T>
    rapidjson::Value SerializeValue(const T& value, rapidjson::Document::AllocatorType& allocator);

    template<typename T>
    void DeserializeValue(T& value, const rapidjson::Value& jsonValue);
}

#endif