#include "CollisionManagerClass.h"

using namespace std;


CollisionManagerClass::CollisionManagerClass(const CollisionManagerClass& other)
{
}


CollisionManagerClass::CollisionManagerClass()
{
}


CollisionManagerClass::~CollisionManagerClass()
{
}


void CollisionManagerClass::Initialize()
{
    entityStorage = Singleton::GetInstance<EntityStorage>();
}


void CollisionManagerClass::Shutdown()
{
    if (entityStorage) {
        entityStorage = 0;
    }
}


CollisionResult CollisionManagerClass::sphere_vs_sphere(
	const Transform t1, const SphereCollider* c1,
	const Transform t2, const SphereCollider* c2)
{
	CollisionResult result = CollisionResult();

	point3d vector = t1.position - t2.position;
	float magnitude = vector.magnitude();

	if (magnitude < c1->radius + c2->radius) {
		result.collided = true;
		result.normal = vector.normalized();
        result.position = t2.position + result.normal * c2->radius;
		result.distance = (c1->radius + c2->radius) - magnitude;
	}

	return result;
}


RaycastResult CollisionManagerClass::Raycast(const RayInfo& ray)
{
	RaycastResult closestHit;
	const vector<Entity*>& entities = entityStorage->GetEntitiesWithComponent<SphereCollider>();

	for (Entity* entity : entities) {
		RaycastResult hit;

        SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();
        if (sphereCollider != nullptr && sphereCollider->active 
            && CollisionFilter::collisionTable[(int)sphereCollider->collisionGroup][(int)ray.collisionGroup]
            && (!ray.touchableOnly || sphereCollider->isTouchable))
        {
            if (raycast_sphere(ray, GetWorldTransform(entity), sphereCollider, hit) && hit.distance < closestHit.distance) {
                closestHit.hit = true;
                closestHit.distance = hit.distance;
                closestHit.position = hit.position;
                closestHit.normal = hit.normal;
                closestHit.collider = sphereCollider;
                closestHit.entity = entity;
            }
        }
	}

	return closestHit;
}


bool CollisionManagerClass::raycast_sphere(const RayInfo& ray, const Transform transform, const SphereCollider* sphere, RaycastResult& hit)
{
    point3d oc = ray.origin - transform.position;

    // Квадратное уравнение для пересечения луча и сферы
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - sphere->radius * sphere->radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        hit.hit = false;
        return false;
    }
    
    float sqrtDiscriminant = sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2 * a);
    float t2 = (-b + sqrtDiscriminant) / (2 * a);

    // Выбираем ближайшее положительное пересечение
    float distance = ray.direction.magnitude();
    float t = -1.0f;
    if (t1 > 0 && t1 <= distance) {
        t = t1;
    }
    else if (t2 > 0 && t2 <= distance) {
        t = t2;
    }

    if (t < 0) {
        hit.hit = false;
        return false;
    }

    hit.hit = true;
    hit.distance = t;
    hit.position = ray.origin + ray.direction * t;
    hit.normal = (hit.position - transform.position).normalized();

    return true;
}


RaycastResult CollisionManagerClass::Spherecast(const SphereCastInfo& sphereCast)
{
    RaycastResult closestHit;
    const vector<Entity*>& entities = entityStorage->GetEntitiesWithComponent<SphereCollider>();

    // Нормализуем направление для точных вычислений
    point3d direction = sphereCast.direction.normalized();

    for (Entity* entity : entities) {
        RaycastResult hit;
        SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();

        // Проверяем, активен ли коллайдер и подходит ли по фильтру
        if (sphereCollider != nullptr && sphereCollider->active
            && CollisionFilter::collisionTable[(int)sphereCollider->collisionGroup][(int)sphereCast.collisionGroup]
            && (!sphereCast.touchableOnly || sphereCollider->isTouchable))
        {
            // Используем новый метод проверки пересечения сферы со сферой
            if (spherecast_sphere(sphereCast, GetWorldTransform(entity), sphereCollider, hit)
                && hit.distance < closestHit.distance) {
                closestHit.hit = true;
                closestHit.distance = hit.distance;
                closestHit.position = hit.position;
                closestHit.normal = hit.normal;
                closestHit.collider = sphereCollider;
                closestHit.entity = entity;
            }
        }
    }

    return closestHit;
}

bool CollisionManagerClass::spherecast_sphere(const SphereCastInfo& sphereCast,
    const Transform& transform,
    const SphereCollider* sphere,
    RaycastResult& hit)
{
    // Вектор от центра сферы-цели до начала сферы-луча
    point3d oc = sphereCast.origin - transform.position;
    point3d direction = sphereCast.direction.normalized();

    // Суммарный радиус (сфера луча + сфера цели)
    float combinedRadius = sphereCast.radius + sphere->radius;
    float maxDistance = sphereCast.maxDistance;

    // Квадратное уравнение для пересечения сферы и луча с учетом радиуса сферы-луча
    float a = direction.dot(direction); // Всегда 1 если нормализовано
    float b = 2.0f * oc.dot(direction);
    float c = oc.dot(oc) - combinedRadius * combinedRadius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        hit.hit = false;
        return false;
    }

    float sqrtDiscriminant = sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2 * a);
    float t2 = (-b + sqrtDiscriminant) / (2 * a);

    // Выбираем ближайшее положительное пересечение, но не дальше maxDistance
    float t = -1.0f;
    if (t1 > 0 && t1 <= maxDistance) {
        t = t1;
    }
    else if (t2 > 0 && t2 <= maxDistance) {
        t = t2;
    }

    if (t < 0) {
        hit.hit = false;
        return false;
    }
    // Вычисляем точку пересечения
    point3d hitPoint = sphereCast.origin + direction * t;

    // Вычисляем нормаль (от центра цели к точке попадания)
    point3d normal = (hitPoint - transform.position).normalized();

    hit.hit = true;
    hit.distance = t;
    hit.position = hitPoint;
    hit.normal = normal;

    return true;
}