#ifndef _PHYSICBODY_H_
#define _PHYSICBODY_H_

#include "../../ECS_Base/component.h"
#include "../../Types/Point3d.h"
#include <DirectXMath.h>

struct PhysicBody : Component
{
	float airFriction = 1.0f;
	float mass = 1.0f;

	point3d velocity = point3d();
	point3d acceleration = point3d();

	DirectX::XMMATRIX mAngVelocity = DirectX::XMMatrixIdentity();

	bool useGravity = true;
	bool preciseMovement = false;
};

// Custom serialization for PhysicBody (Without using NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE)
inline void to_json(nlohmann::json& j, const PhysicBody& t) {
    DirectX::XMFLOAT4X4 tempMatrix;
    DirectX::XMStoreFloat4x4(&tempMatrix, t.mAngVelocity);

    j = nlohmann::json{
        {"active", t.active},
        {"airFriction", t.airFriction},
        {"mass", t.mass},

        {"velocity", nlohmann::json{{"x", t.velocity.x}, {"y", t.velocity.y}, {"z", t.velocity.z}}},
        {"acceleration", nlohmann::json{{"x", t.acceleration.x}, {"y", t.acceleration.y}, {"z", t.acceleration.z}}},

        {"mAngVelocity", nlohmann::json{
            {"m11", tempMatrix.m[0][0]}, {"m12", tempMatrix.m[0][1]}, {"m13", tempMatrix.m[0][2]}, {"m14", tempMatrix.m[0][3]},
            {"m21", tempMatrix.m[1][0]}, {"m22", tempMatrix.m[1][1]}, {"m23", tempMatrix.m[1][2]}, {"m24", tempMatrix.m[1][3]},
            {"m31", tempMatrix.m[2][0]}, {"m32", tempMatrix.m[2][1]}, {"m33", tempMatrix.m[2][2]}, {"m34", tempMatrix.m[2][3]},
            {"m41", tempMatrix.m[3][0]}, {"m42", tempMatrix.m[3][1]}, {"m43", tempMatrix.m[3][2]}, {"m44", tempMatrix.m[3][3]}
        }},

        {"useGravity", t.useGravity},
        {"preciseMovement", t.preciseMovement}
    };
}

inline void from_json(const nlohmann::json& j, PhysicBody& t) {
    j.at("active").get_to(t.active);
    j.at("airFriction").get_to(t.airFriction);
    j.at("mass").get_to(t.mass);

    const auto& velocity = j.at("velocity");
    velocity.at("x").get_to(t.velocity.x);
    velocity.at("y").get_to(t.velocity.y);
    velocity.at("z").get_to(t.velocity.z);

    const auto& acceleration = j.at("acceleration");
    acceleration.at("x").get_to(t.acceleration.x);
    acceleration.at("y").get_to(t.acceleration.y);
    acceleration.at("z").get_to(t.acceleration.z);

    const auto& mAngVelocity = j.at("mAngVelocity");
    DirectX::XMFLOAT4X4 tempMatrix;

    mAngVelocity.at("m11").get_to(tempMatrix.m[0][0]);
    mAngVelocity.at("m12").get_to(tempMatrix.m[0][1]);
    mAngVelocity.at("m13").get_to(tempMatrix.m[0][2]);
    mAngVelocity.at("m14").get_to(tempMatrix.m[0][3]);
    mAngVelocity.at("m21").get_to(tempMatrix.m[1][0]);
    mAngVelocity.at("m22").get_to(tempMatrix.m[1][1]);
    mAngVelocity.at("m23").get_to(tempMatrix.m[1][2]);
    mAngVelocity.at("m24").get_to(tempMatrix.m[1][3]);
    mAngVelocity.at("m31").get_to(tempMatrix.m[2][0]);
    mAngVelocity.at("m32").get_to(tempMatrix.m[2][1]);
    mAngVelocity.at("m33").get_to(tempMatrix.m[2][2]);
    mAngVelocity.at("m34").get_to(tempMatrix.m[2][3]);
    mAngVelocity.at("m41").get_to(tempMatrix.m[3][0]);
    mAngVelocity.at("m42").get_to(tempMatrix.m[3][1]);
    mAngVelocity.at("m43").get_to(tempMatrix.m[3][2]);
    mAngVelocity.at("m44").get_to(tempMatrix.m[3][3]);

    t.mAngVelocity = DirectX::XMLoadFloat4x4(&tempMatrix);
}

#endif