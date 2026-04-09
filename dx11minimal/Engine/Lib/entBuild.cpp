#include "entBuild.h"
#include "../../Vendors/rapidjson-1.1.0/include/rapidjson/document.h"


Entity* entBuild::Create(const char* json)
{
	rapidjson::Document doc;

	doc.Parse(json);

	return new Entity;
}