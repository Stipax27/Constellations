#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "../ECS_Base/component.h"
#include "../Types/Point3d.h"
#include <DirectXMath.h>


struct Transform : Component
{
    point3d position = point3d();
    point3d scale = point3d(1.0f, 1.0f, 1.0f);
    DirectX::XMMATRIX mRotation = DirectX::XMMatrixIdentity();

    point3d GetRightVector() const;
    point3d GetUpVector() const;
    point3d GetLookVector() const;

    Transform& operator=(const Transform&);
    Transform operator+(const Transform&);
    Transform& operator+=(const Transform&);

    void to_json(rapidjson::Value& componentObj, rapidjson::Document::AllocatorType& allocator) override;
    void from_json(rapidjson::Value& componentObj, rapidjson::Document::AllocatorType& allocator) override;
};

// Custom serialization for Transform (Without using NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE)
inline void to_json(nlohmann::json& j, const Transform& t) {
    DirectX::XMFLOAT4X4 tempMatrix;
    DirectX::XMStoreFloat4x4(&tempMatrix, t.mRotation);

    j = nlohmann::json{
        {"active", t.active},
        {"position", nlohmann::json{{"x", t.position.x}, {"y", t.position.y}, {"z", t.position.z}}},
        {"scale", nlohmann::json{{"x", t.scale.x}, {"y", t.scale.y}, {"z", t.scale.z}}},
        {"mRotation", nlohmann::json{
            {"m11", tempMatrix.m[0][0]}, {"m12", tempMatrix.m[0][1]}, {"m13", tempMatrix.m[0][2]}, {"m14", tempMatrix.m[0][3]},
            {"m21", tempMatrix.m[1][0]}, {"m22", tempMatrix.m[1][1]}, {"m23", tempMatrix.m[1][2]}, {"m24", tempMatrix.m[1][3]},
            {"m31", tempMatrix.m[2][0]}, {"m32", tempMatrix.m[2][1]}, {"m33", tempMatrix.m[2][2]}, {"m34", tempMatrix.m[2][3]},
            {"m41", tempMatrix.m[3][0]}, {"m42", tempMatrix.m[3][1]}, {"m43", tempMatrix.m[3][2]}, {"m44", tempMatrix.m[3][3]}
        }}
    };
}

inline void from_json(const nlohmann::json& j, Transform& t) {
    j.at("active").get_to(t.active);

    const auto& pos = j.at("position");
    pos.at("x").get_to(t.position.x);
    pos.at("y").get_to(t.position.y);
    pos.at("z").get_to(t.position.z);

    const auto& sc = j.at("scale");
    sc.at("x").get_to(t.scale.x);
    sc.at("y").get_to(t.scale.y);
    sc.at("z").get_to(t.scale.z);

    const auto& rot = j.at("mRotation");
    DirectX::XMFLOAT4X4 tempMatrix;

    rot.at("m11").get_to(tempMatrix.m[0][0]);
    rot.at("m12").get_to(tempMatrix.m[0][1]);
    rot.at("m13").get_to(tempMatrix.m[0][2]);
    rot.at("m14").get_to(tempMatrix.m[0][3]);
    rot.at("m21").get_to(tempMatrix.m[1][0]);
    rot.at("m22").get_to(tempMatrix.m[1][1]);
    rot.at("m23").get_to(tempMatrix.m[1][2]);
    rot.at("m24").get_to(tempMatrix.m[1][3]);
    rot.at("m31").get_to(tempMatrix.m[2][0]);
    rot.at("m32").get_to(tempMatrix.m[2][1]);
    rot.at("m33").get_to(tempMatrix.m[2][2]);
    rot.at("m34").get_to(tempMatrix.m[2][3]);
    rot.at("m41").get_to(tempMatrix.m[3][0]);
    rot.at("m42").get_to(tempMatrix.m[3][1]);
    rot.at("m43").get_to(tempMatrix.m[3][2]);
    rot.at("m44").get_to(tempMatrix.m[3][3]);

    t.mRotation = DirectX::XMLoadFloat4x4(&tempMatrix);
}


#endif