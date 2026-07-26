#include "CollisionManagerClass.h"

using namespace std;

std::map<TypePair, CollisionFn> CollisionManagerClass::collisionMap;



CollisionManagerClass::CollisionManagerClass(const CollisionManagerClass& other)
{
}


CollisionManagerClass::CollisionManagerClass()
{
    RegisterCollision(Collider::Type::Sphere, Collider::Type::Sphere, sphere_vs_sphere);
    RegisterCollision(Collider::Type::Sphere, Collider::Type::Plane, sphere_vs_plane);
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


void CollisionManagerClass::RegisterCollision(Collider::Type t1, Collider::Type t2, CollisionFn fn) {
    if (t1 < t2)
        collisionMap[TypePair{ t1, t2 }] = fn;
    else
        collisionMap[TypePair{ t2, t1 }] = fn;
}


CollisionResult CollisionManagerClass::ResolveCollision(const Transform& t1, const Collider* c1,
    const Transform& t2, const Collider* c2)
{
    TypePair key{ c1->type, c2->type };
    if (key.a > key.b) std::swap(key.a, key.b);
    auto it = collisionMap.find(key);
    if (it != collisionMap.end()) {
        // Если исходный порядок не совпадает с упорядоченным, меняем аргументы
        if (c1->type > c2->type) {
            return it->second(t2, c2, t1, c1);
        }
        else {
            return it->second(t1, c1, t2, c2);
        }
    }
    return CollisionResult();
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

CollisionResult CollisionManagerClass::sphere_vs_sphere(
    const Transform& t1, const Collider* c1,
    const Transform& t2, const Collider* c2)
{
    const SphereCollider* s1 = static_cast<const SphereCollider*>(c1);
    const SphereCollider* s2 = static_cast<const SphereCollider*>(c2);

    CollisionResult result = CollisionResult();
    point3d vector = t1.position - t2.position;
    float magnitude = vector.magnitude();

    if (magnitude < s1->radius + s2->radius) {
        result.collided = true;
        result.normal = vector.normalized();
        result.position = t2.position + result.normal * s2->radius;
        result.distance = (s1->radius + s2->radius) - magnitude;
    }
    return result;
}

CollisionResult CollisionManagerClass::sphere_vs_plane(
    const Transform& t1, const Collider* c1,
    const Transform& t2, const Collider* c2)
{
    const SphereCollider* sphere = static_cast<const SphereCollider*>(c1);
    const PlaneCollider* plane = static_cast<const PlaneCollider*>(c2);

    CollisionResult result;
    point3d center = t1.position;
    point3d planePos = t2.position;
    point3d normal = plane->normal.normalized();

    // Расстояние от центра сферы до плоскости (со знаком)
    float dist = (center - planePos).dot(normal);

    // Если сфера пересекает плоскость (или касается)
    if (dist < sphere->radius) {
        // Точка на плоскости, ближайшая к центру сферы
        point3d contactPoint = center - normal * dist;

        // Проверка попадания в диск (если радиус > 0)
        if (plane->radius > 0.0f) {
            float distToCenter = (contactPoint - planePos).magnitude();
            if (distToCenter > plane->radius) {
                return result; // вне диска 
            }
        }

        result.collided = true;
        result.normal = normal;
        result.position = contactPoint;
        result.distance = sphere->radius - dist;
    }
    return result;
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