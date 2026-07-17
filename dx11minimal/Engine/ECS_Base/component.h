////////////////////////////////////////////////////////////////////////////////
// Filename: component.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COMPONENT_H_
#define _COMPONENT_H_

//////////////
// INCLUDES //
//////////////
#include<string>
#include "../Lib/logging.h"

#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/document.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/writer.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/stringbuffer.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/prettywriter.h"

#include <nlohmann/json.hpp>

////////////////////////////////////////////////////////////////////////////////
// Struct name: Component
////////////////////////////////////////////////////////////////////////////////
struct Component
{
	bool active = true;

	virtual ~Component() = default;

	virtual void to_json(rapidjson::Value& componentObj, rapidjson::Document::AllocatorType& allocator) { Log("WARNING! Empty component serialize realization!\n"); };
	virtual void from_json(rapidjson::Value& componentObj, rapidjson::Document::AllocatorType& allocator) { Log("WARNING! Empty component deserialize realization!\n"); };
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Component, active)

#endif