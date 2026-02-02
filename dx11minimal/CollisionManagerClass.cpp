#include "CollisionManagerClass.h"


CollisionManagerClass::CollisionManagerClass(const CollisionManagerClass& other)
{
}


CollisionManagerClass::CollisionManagerClass()
{
}


CollisionManagerClass::~CollisionManagerClass()
{
}


void CollisionManagerClass::Initialize(EntityStorage* _EntityStorage)
{
    entityStorage = _EntityStorage;
	//collisionMap[{typeid(SphereCollider), typeid(SphereCollider)}] = sphere_vs_sphere;
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
		result.distance = (c1->radius + c2->radius) - magnitude;
	}

	return result;
}


RaycastResult CollisionManagerClass::Raycast(const RayInfo& ray)
{
	RaycastResult closestHit;

	for (Entity* entity : entityStorage->entities) {
		RaycastResult hit;

        SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();
        if (sphereCollider != nullptr && sphereCollider->active) {
            if (raycast_sphere(ray, GetWorldTransform(entity), sphereCollider, hit) && hit.distance < closestHit.distance) {
                closestHit.hit = true;
                closestHit.distance = hit.distance;
                closestHit.position = hit.position;
                closestHit.normal = hit.normal;
                closestHit.collider = sphereCollider;
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