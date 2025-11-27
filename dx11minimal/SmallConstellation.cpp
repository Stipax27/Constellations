#include "SmallConstellation.h"

void SmallConstellation::Init(World* World, Entity* entity, Entity* target) {
    m_World = World;
    m_entity = entity;
    m_target = target;
    LastTime = timer::currentTime;
}

void SmallConstellation::Volley() {

    for (int i = 0; i < m_entity->GetComponent<Constellation>()->stars.size(); i++) 
    {
        point3d startPos;
        startPos.x = m_entity->GetComponent<Constellation>()->stars[i].x;
        startPos.y = m_entity->GetComponent<Constellation>()->stars[i].y;
        startPos.z = m_entity->GetComponent<Constellation>()->stars[i].z;
        startPos += m_entity->GetComponent<Transform>()->position;

        point3d targetPoint = m_target->GetComponent<Transform>()->position;
        point3d distance = startPos - targetPoint;

        point3d direction = targetPoint - startPos;
        direction = direction.normalized();

        Entity* volleyEntity = m_World->CreateEntity();

        Projectile* projectile;
        projectile = volleyEntity->AddComponent<Projectile>();
        projectile->direction = direction;
        float speed = 50;
        projectile->speed = speed + ((rand() % 50 - 25) / 100.0f * speed);;
        projectile->startPosition = startPos;

        projectile->spiralMovement = true;
       
        float spiralSpeed = 12.46f;
        projectile->spiralSpeed = spiralSpeed + ((rand() % 50 - 25) / 100.0f * spiralSpeed);
        projectile->startPosition = startPos;
        float spiralRadius = 4.0f;
        projectile->spiralRadius = spiralRadius + ((rand() % 50 - 25) / 100.0f * spiralRadius);

        point3d temporaryVector;
        if (fabs(direction.y) < 0.9f) temporaryVector = point3d(0, 1, 0);
        else temporaryVector = point3d(1, 0, 0);
        point3d spiralVector1 = temporaryVector.cross(direction);
        spiralVector1.normalized();
        projectile->spiralVector1 = spiralVector1;
        point3d spiralVector2 = spiralVector1.cross(direction);
        spiralVector2.normalized();
        projectile->spiralVector2 = spiralVector2;

        Explosion* chargeExplosion = volleyEntity->AddComponent<Explosion>();
        chargeExplosion->max_radius = 1.0f;

        Transform* throwTransform = volleyEntity->AddComponent<Transform>();
        throwTransform->position = startPos;
    }   
}

void SmallConstellation::Lattice() {

    point3d startPos = m_entity->GetComponent<Transform>()->position;
    point3d targetPoint = m_target->GetComponent<Transform>()->position;
    point3d distance = startPos - targetPoint;

    point3d direction = targetPoint - startPos;
    direction = direction.normalized();

    Entity* latticeEntity = m_World->CreateEntity();

    Projectile* projectile;
    projectile = latticeEntity->AddComponent<Projectile>();
    projectile->direction = direction;
    projectile->speed = 40;

    Constellation* constellation;
    constellation = latticeEntity->AddComponent<Constellation>();
    constellation->stars =
    {
        point3d(0, 0,- 2), 

        point3d(-3, 0, 0), 
        point3d(0, 3, 0),
        point3d(3, 0, 0),            
        point3d(0, -3, 0),

        point3d(-2, 2, 0),
        point3d(2, 2, 0),
        point3d(2, -2, 0),
        point3d(-2, -2, 0),

        point3d(-1.5, 0, -1),
        point3d(-1, 1, -1),
        point3d(0, 1.5, -1),
        point3d(1, 1, -1),
        point3d(1.5, 0, -1),
        point3d(1, -1, -1),
        point3d(0, -1.5, -1),
        point3d(-1, -1, -1),

    };
    constellation->links = 
    {
        {0, 1}, {0, 2}, {0, 3}, {0, 4},

        {0, 5}, {0, 6}, {0, 7}, {0, 8},

        {1, 5}, {5, 2}, {2, 6}, {6, 3}, {3, 7}, {7, 4}, {4, 8}, {8, 1},

        {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14}, {14, 15}, {15, 16}, {16, 9}

    };

    Transform* latticeTransform = latticeEntity->AddComponent<Transform>();
    latticeTransform->position = startPos;

    point3d currentLookVector = latticeTransform->GetLookVector();
    point3d rotationAxis = currentLookVector.cross(direction).normalized();
    DirectX::XMVECTOR rotationAxisVector = DirectX::XMVectorSet(rotationAxis.x, rotationAxis.y, rotationAxis.z, 0.0f);
    float angleBetweenVectors = acosf(currentLookVector.dot(direction));
    DirectX::XMVECTOR quaternionRotation = DirectX::XMQuaternionRotationAxis(rotationAxisVector, angleBetweenVectors);

    latticeTransform->qRotation = DirectX::XMQuaternionMultiply(latticeTransform->qRotation, quaternionRotation);
    latticeTransform->mRotation = DirectX::XMMatrixRotationQuaternion(latticeTransform->qRotation);
}
