#include "componentutils.h"

#include "../Compute/Combat/Health.h"
#include "../UI/Text/TextLabel.h"
#include "../Compute/DelayedDestroy/DelayedDestroy.h"


inline XMFLOAT3 Point3DToXMFLOAT3(const point3d& p) {
    return XMFLOAT3(p.x, p.y, p.z);
}

inline point3d XMFLOAT3ToPoint3D(const XMFLOAT3& v) {
    return point3d(v.x, v.y, v.z);
}

inline XMVECTOR LoadPoint3D(const point3d& p) {
    return XMVectorSet(p.x, p.y, p.z, 0.0f);
}

inline point3d StorePoint3D(FXMVECTOR v) {
    XMFLOAT3 f;
    XMStoreFloat3(&f, v);
    return point3d(f.x, f.y, f.z);
}


XMMATRIX GetWorldMatrix(const Transform& worldTransform) {
	XMMATRIX rotateMatrix = worldTransform.mRotation;
	XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z);

	XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
	XMMATRIX worldMatrix = srMatrix * translateMatrix;

	return XMMatrixTranspose(worldMatrix);
}


Transform GetRelativeTransform(const Transform& parentWorldTransform, const Transform& childWorldTransform) {
    Transform relativeTransform;

    // Относительная позиция
    point3d relativePosition = point3d(
        childWorldTransform.position.x - parentWorldTransform.position.x,
        childWorldTransform.position.y - parentWorldTransform.position.y,
        childWorldTransform.position.z - parentWorldTransform.position.z
    );

    // Обратная матрица поворота
    DirectX::XMMATRIX invParentRotation = DirectX::XMMatrixTranspose(parentWorldTransform.mRotation);

    // Поворачиваем относительную позицию
    DirectX::XMVECTOR posVec = LoadPoint3D(relativePosition);
    posVec = DirectX::XMVector3Transform(posVec, invParentRotation);
    relativeTransform.position = StorePoint3D(posVec);

    // Относительный масштаб
    relativeTransform.scale = point3d(
        childWorldTransform.scale.x / parentWorldTransform.scale.x,
        childWorldTransform.scale.y / parentWorldTransform.scale.y,
        childWorldTransform.scale.z / parentWorldTransform.scale.z
    );

    // Относительный поворот
    relativeTransform.mRotation = DirectX::XMMatrixMultiply(
        childWorldTransform.mRotation,
        invParentRotation
    );

    return relativeTransform;
}


CollisionInfo GetProjectileCollisionInfo(EntityStorage* entityStorage, Entity* projectile)
{
    SphereCollider* sphereCollider = projectile->GetComponent<SphereCollider>();

    if (sphereCollider != nullptr) {
        for (int i = 0; i < sphereCollider->collisions.size(); i++) {
            CollisionInfo& info = sphereCollider->collisions[i];

            Entity* entity = entityStorage->GetEntityById(info.entityId);
            if (!IsEntityValid(entity)) {
                continue;
            }

            Health* health = entity->GetComponentInAncestor<Health>();
            if (health != nullptr && health->active) {
                return sphereCollider->collisions[i];
            }
        }
    }

    return CollisionInfo();
}


void DrawDebugString(std::wstring text, point3d screenPos) {
    EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();

    Entity* entity = entityStorage->CreateEntity("DebugString");

    Transform2D* transform2D = entity->AddComponent<Transform2D>();
    transform2D->position = screenPos;

    TextLabel* textLabel = entity->AddComponent<TextLabel>();
    textLabel->textW = text;
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 900;
    textLabel->fontSizePx = 38;
    textLabel->fontScale = 0.5f;
    textLabel->letterSpacingPx = 1.0f;

    DelayedDestroy* delayedDestroy = entity->AddComponent<DelayedDestroy>();
    delayedDestroy->lifeTime = 1;
}


void DrawUiObject(const Transform2D& transform2D, int vShader) {
    Shaders::vShader(vShader);

    ConstBuf::global[0] = XMFLOAT4(transform2D.position.x, transform2D.position.y, transform2D.position.z, ConstBuf::global[0].w);
    ConstBuf::global[1] = XMFLOAT4(transform2D.scale.x, transform2D.scale.y, 0, 0);
    ConstBuf::global[2] = XMFLOAT4(transform2D.anchorPoint.x, transform2D.anchorPoint.y, transform2D.rotation, 0);

    switch (transform2D.ratio)
    {
    case ScreenAspectRatio::XY:
        ConstBuf::global[1].z = 1;
        ConstBuf::global[1].w = 1;
        break;
    case ScreenAspectRatio::YX:
        ConstBuf::global[1].z = ConstBuf::frame.aspect.x;
        ConstBuf::global[1].w = ConstBuf::frame.aspect.y;
        break;
    case ScreenAspectRatio::XX:
        ConstBuf::global[1].z = 1;
        ConstBuf::global[1].w = ConstBuf::frame.aspect.y;
        break;
    case ScreenAspectRatio::YY:
        ConstBuf::global[1].z = ConstBuf::frame.aspect.x;
        ConstBuf::global[1].w = 1;
        break;
    }

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    ConstBuf::ConstToPixel(5);

    Draw::Drawer(1);
}