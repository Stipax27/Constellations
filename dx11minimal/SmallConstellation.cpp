#include "SmallConstellation.h"

void SmallConstellation::Init(World* World, Entity* entity, Entity* target) {
    m_World = World;
    m_entity = entity;
    m_target = target;
    LastTime = timer::currentTime;
    
    Constellation* constellation = m_entity->GetComponent<Constellation>();

    LastStarsPositions = m_entity->GetComponent<Constellation>()->stars;
    FutureStarsPositions = m_entity->GetComponent<Constellation>()->stars;
}

void SmallConstellation::VolleyStart() {

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

        Explosion* chargeExplosion = volleyEntity->AddComponent<Explosion>();
        chargeExplosion->max_radius = 1.0f;

        Transform* volleyTransform = volleyEntity->AddComponent<Transform>();
        volleyTransform->position = startPos;

        VolleyElement* element = new VolleyElement;
        
        float speed = 50;
        element->speed = speed + ((rand() % 50 - 25) / 100.0f * speed);;
        element->startPosition = startPos;
        
        element->spiralMovement = true;
        
        float spiralSpeed = 12.46f;
        element->spiralSpeed = spiralSpeed + ((rand() % 50 - 25) / 100.0f * spiralSpeed);
        element->startPosition = startPos;
        float spiralRadius = 0.0f;
        element->spiralRadius = spiralRadius + ((rand() % 50 - 25) / 100.0f * spiralRadius);
        float radiusIncreaseSpeed = 2.5f;
        element->radiusIncreaseSpeed = radiusIncreaseSpeed + ((rand() % 50 - 25) / 100.0f * radiusIncreaseSpeed);
        
        point3d temporaryVector;
        if (fabs(direction.y) < 0.9f) temporaryVector = point3d(0, 1, 0);
        else temporaryVector = point3d(1, 0, 0);
        point3d spiralVector1 = temporaryVector.cross(direction);
        spiralVector1.normalized();
        element->spiralVector1 = spiralVector1;
        point3d spiralVector2 = spiralVector1.cross(direction);
        spiralVector2.normalized();
        element->spiralVector2 = spiralVector2;

        element->entity = volleyEntity;
        element->direction = direction;
        VolleyElements.push_back(element);

    }   
}
void SmallConstellation::VolleyUpdate(float deltaTime) {

    for (auto* element : VolleyElements) {

        Transform* elementTransform = element->entity->GetComponent<Transform>();
        
        //elementTransform->position += element->direction * element->speed * deltaTime;
        
        element->time += deltaTime;
        
        float distanceTraveled = element->speed * element->time;
        
        float angleTraveled = element->time * element->spiralSpeed;
        float localX = cos(angleTraveled) * element->spiralRadius;
        float localY = sin(angleTraveled) * element->spiralRadius;
        
        elementTransform->position = element->startPosition +
            element->direction * distanceTraveled +
            (element->spiralVector1 * localX +
                element->spiralVector2 * localY);
        
        element->spiralRadius += element->radiusIncreaseSpeed * deltaTime; 
    }
}

void SmallConstellation::LatticeStart() {

    point3d startPos = m_entity->GetComponent<Transform>()->position;
    point3d targetPoint = m_target->GetComponent<Transform>()->position;
    point3d distance = startPos - targetPoint;

    point3d direction = targetPoint - startPos;
    direction = direction.normalized();

    Entity* latticeEntity = m_World->CreateEntity();

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

    SetLookVector(latticeTransform, direction);

    Lattice* lattice = new Lattice;
    lattice->entity = latticeEntity; 
    lattice->speed = 50;
    lattice->direction = latticeTransform->GetLookVector();
    Lattices.push_back(lattice);
}

void SmallConstellation::LatticeUpdate(float deltaTime) {
    for (auto* lattice : Lattices)
    {
        Transform* latticeTransform = lattice->entity->GetComponent<Transform>();
        latticeTransform->position += lattice->direction * lattice->speed * deltaTime;
    }
}

void SmallConstellation::TransformationStart() {
    FutureStarsPositions = {
        point3d(0, 1, 0),
        point3d(1, 1, 0),
        point3d(1, 0, 0),
        point3d(0, 0, 0),
    
        point3d(2, 1, 0),
        point3d(3, 1, 0),
        point3d(3, 0, 0),
        point3d(2, 0, 0),
    
        point3d(0, -2, 0),
        point3d(1, -1, 0),
        point3d(2, -1, 0),
        point3d(3, -2, 0),
    };
}

void SmallConstellation::TransformationUpdate(float deltaTime) {

    Constellation* constellation = m_entity->GetComponent<Constellation>();
    if (FutureStarsPositions != constellation->stars)
    {
        for (size_t i = 0; i < constellation->stars.size(); i++) {
            constellation->stars[i] = constellation->stars[i].lerp(FutureStarsPositions[i], 0.1f); //Нужно переделать, отталкиваясь от deltaTime
            //constellation->stars[i] = LastStarsPositions[i].lerp(FutureStarsPositions[i], нужноРассчитыватьДинамически);
        }

    }
}