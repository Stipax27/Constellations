#include "collisionSystem.h"

using namespace std;

// ------------------------------------------------------------------
// Вспомогательная функция для получения указателя на любой коллайдер
// ------------------------------------------------------------------
static Collider* GetColliderFromEntity(Entity* entity)
{
    if (auto* s = entity->GetComponent<SphereCollider>()) return s;
    if (auto* p = entity->GetComponent<PlaneCollider>())   return p;
    // если есть SurfaceCollider, добавьте:
    // if (auto* su = entity->GetComponent<SurfaceCollider>()) return su;
    return nullptr;
}

// ------------------------------------------------------------------
// Конструктор / деструктор / инициализация
// ------------------------------------------------------------------
CollisionSystem::CollisionSystem()
{
    collisionManager = Singleton::GetInstance<CollisionManagerClass>();
}

void CollisionSystem::Initialize()
{
}

void CollisionSystem::Shutdown()
{
    if (collisionManager) {
        collisionManager = 0;
    }
}

// ------------------------------------------------------------------
// Главный цикл обработки коллизий
// ------------------------------------------------------------------
void CollisionSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
    // 1. Собираем все сущности с любым коллайдером
    std::vector<Entity*> allEntities;

    auto spheres = entityStorage.GetEntitiesWithComponent<SphereCollider>();
    allEntities.insert(allEntities.end(), spheres.begin(), spheres.end());

    auto planes = entityStorage.GetEntitiesWithComponent<PlaneCollider>();
    allEntities.insert(allEntities.end(), planes.begin(), planes.end());

    // Если есть SurfaceCollider, раскомментируйте:
    // auto surfaces = entityStorage.GetEntitiesWithComponent<SurfaceCollider>();
    // allEntities.insert(allEntities.end(), surfaces.begin(), surfaces.end());

    // Удаляем дубликаты (если сущность имеет несколько коллайдеров)
    std::sort(allEntities.begin(), allEntities.end());
    allEntities.erase(std::unique(allEntities.begin(), allEntities.end()), allEntities.end());

    // 2. Внешний цикл
    for (Entity* entity1 : allEntities)
    {
        if (!IsEntityValid(entity1))
            continue;

        Transform* transform1 = entity1->GetUpperComponent<Transform>();
        PhysicBody* physicBody1 = entity1->GetComponentInAncestor<PhysicBody>();
        Collider* collider1 = GetColliderFromEntity(entity1);

        if (transform1 == nullptr || collider1 == nullptr || !collider1->active)
            continue;

        collider1->collisions.clear();
        Transform worldTransform1 = GetWorldTransform(entity1);

        // 3. Внутренний цикл
        for (Entity* entity2 : allEntities)
        {
            if (entity2 == entity1 || !IsEntityValid(entity2) || (entity1->GetTimeScale() == 0.0f && entity2->GetTimeScale() == 0.0f))
                continue;

            Collider* collider2 = GetColliderFromEntity(entity2);
            if (collider2 == nullptr || !collider2->active)
                continue;

            // Проверка таблицы групп
            if (!CollisionFilter::collisionTable[(int)collider1->collisionGroup][(int)collider2->collisionGroup])
                continue;

            Transform worldTransform2 = GetWorldTransform(entity2);

            // 4. Универсальный вызов через карту функций
            CollisionResult result = CollisionManagerClass::ResolveCollision(worldTransform1, collider1, worldTransform2, collider2);

            if (result.collided)
            {
                // Запись информации о коллизии
                CollisionInfo info;
                info.entityId = entity2->GetId();
                info.normal = result.normal;
                info.position = result.position;
                info.distance = result.distance;
                collider1->collisions.push_back(info);

                // 5. Разрешение физики (упрощённое, без radius)
                if (physicBody1 != nullptr && physicBody1->active && collider1->isTouchable && collider2->isTouchable)
                {
                    // Сдвиг по нормали на глубину проникновения
                    transform1->position += result.normal * result.distance;

                    // Отражение скорости (упругий отскок)
                    point3d nVel = physicBody1->velocity.normalized();
                    if (physicBody1->velocity.magnitude() > 0.001f)
                    {
                        point3d reflected = nVel - 2.0f * nVel.dot(result.normal) * result.normal;
                        physicBody1->velocity = reflected * physicBody1->velocity.magnitude();
                    }
                }

                // 6. Обработка урона (оригинальный код, без изменений)
                pair<Entity*, Health*> hres = entity1->GetAncestorWithComponent<Health>();
                if (hres.first != nullptr && hres.second->active)
                {
                    pair<Entity*, SingleDamager*> sdres = entity2->GetAncestorWithComponent<SingleDamager>();
                    if (sdres.first != nullptr && sdres.second->active && sdres.second->target == hres.second->fraction
                        && find(sdres.second->entityFilter.begin(), sdres.second->entityFilter.end(), hres.first->GetId()) == sdres.second->entityFilter.end())
                    {
                        DamageUnit unit = DamageUnit(sdres.second->damageType, sdres.second->damage);
                        hres.second->damageQueue.push_back(unit);

                        sdres.second->entityFilter.push_back(hres.first->GetId());
                        if (sdres.second->maxHitCount > 0 && sdres.second->entityFilter.size() >= sdres.second->maxHitCount)
                        {
                            if (sdres.second->destroyable) {
                                sdres.first->Destroy();
                            }
                            sdres.first->RemoveComponent<SingleDamager>();
                        }
                    }

                    pair<Entity*, MultiDamager*> mdres = entity2->GetAncestorWithComponent<MultiDamager>();
                    if (mdres.first != nullptr && mdres.second->active && mdres.second->target == hres.second->fraction
                        && timer::currentTime - mdres.second->lastDamageTime >= mdres.second->interval)
                    {
                        DamageUnit unit = DamageUnit(mdres.second->damageType, mdres.second->damage);
                        hres.second->damageQueue.push_back(unit);

                        mdres.second->lastDamageTime = timer::currentTime;
                        mdres.second->repeatCount++;

                        if (mdres.second->repeats >= 0 && mdres.second->repeatCount >= mdres.second->repeats)
                        {
                            if (mdres.second->destroyable) {
                                mdres.first->Destroy();
                            }
                            mdres.first->RemoveComponent<MultiDamager>();
                        }
                    }
                }
            }
        }
    }
}

//void CollisionSystem::Update(EntityStorage& entityStorage, float deltaTime)
//{
//	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<SphereCollider>();
//
//	for (Entity* entity1 : entities)
//	{
//		if (!IsEntityValid(entity1))
//			continue;
//		
//		Transform* transform1 = entity1->GetUpperComponent<Transform>();
//		PhysicBody* physicBody1 = entity1->GetComponentInAncestor<PhysicBody>();
//		SphereCollider* collider1 = entity1->GetComponent<SphereCollider>();
//		if (transform1 == nullptr || collider1 == nullptr || !collider1->active)
//			continue;
//
//		collider1->collisions.clear();
//		Transform worldTransform1 = GetWorldTransform(entity1);
//		size_t size = entities.size();
//
//		for (Entity* entity2 : entities)
//		{
//			if (entity2 == entity1 || !IsEntityValid(entity2) || (entity1->GetTimeScale() == 0.0f && entity2->GetTimeScale() == 0.0f))
//				continue;
//
//			SphereCollider* collider2 = entity2->GetComponent<SphereCollider>();
//			if (collider2 == nullptr || !collider2->active || !CollisionFilter::collisionTable[(int)collider1->collisionGroup][(int)collider2->collisionGroup])
//				continue;
//
//			/*TypePair key{ std::type_index(typeid(collider1)), std::type_index(typeid(collider2)) };
//
//			auto it = collisionMap.find(key);
//			if (it != collisionMap.end()) {
//				CollisionResult res = it->second(transform1, collider1, transform2, collider2);
//				if (res.collided) {
//
//				}
//			}*/
//			// hz zachem eto seichas no ostavlyu na vsyakiy sluchay
//
//			Transform worldTransform1 = GetWorldTransform(entity1);
//			Transform worldTransform2 = GetWorldTransform(entity2);
//
//			CollisionResult result = CollisionManagerClass::ResolveCollision(worldTransform1, collider1, worldTransform2, collider2);
//			if (result.collided) {
//				CollisionInfo info = CollisionInfo();
//				info.entityId = entity2->GetId();
//				info.normal = result.normal;
//				info.position = result.position;
//				info.distance = result.distance;
//				collider1->collisions.push_back(info);
//
//				//transform1->position += planeCollider->normal * (sphereCollider->radius - distance);
//
//				PhysicBody* physicBody2 = entity2->GetComponentInAncestor<PhysicBody>();
//				if (((physicBody1 != nullptr && physicBody1->active) || (physicBody2 != nullptr && physicBody2->active))) {
//
//					if (collider1->isTouchable && collider2->isTouchable && physicBody1 != nullptr && physicBody1->active) {
//						point3d nVel = physicBody1->velocity.normalized();
//						if (collider1->softness == 0 && collider2->softness == 0) {
//							transform1->position += result.normal * result.distance;
//							physicBody1->velocity = (nVel + result.normal * result.normal.dot(-nVel)) * physicBody1->velocity.magnitude();
//						}
//						else {
//							float softnessTotal = max(0.01f, collider1->softness + collider2->softness);
//							float penetrationRatio = result.distance / (collider1->radius + collider2->radius);
//
//							float pushStrength = 5.0f / softnessTotal; // Настройте этот коэффициент
//							float velocityPush = pushStrength * penetrationRatio;
//
//							// Ограничиваем максимальное изменение скорости
//							//velocityPush = min(velocityPush, 10.0f);
//
//							physicBody1->velocity += result.normal * velocityPush;
//
//							// Дополнительно: небольшое позиционное выталкивание для мягкости
//							//float positionPush = 0.3f * penetrationRatio / softnessTotal;
//							//transform1->position += result.normal * positionPush;
//						}
//					}
//
//				}
//
//				pair<Entity*, Health*> hres = entity1->GetAncestorWithComponent<Health>();
//				if (hres.first != nullptr && hres.second->active) {
//
//					pair<Entity*, SingleDamager*> sdres = entity2->GetAncestorWithComponent<SingleDamager>();
//					if (sdres.first != nullptr && sdres.second->active && sdres.second->target == hres.second->fraction && find(sdres.second->entityFilter.begin(), sdres.second->entityFilter.end(), hres.first->GetId()) == sdres.second->entityFilter.end()) {
//
//						DamageUnit unit = DamageUnit(sdres.second->damageType, sdres.second->damage);
//						hres.second->damageQueue.push_back(unit);
//
//						sdres.second->entityFilter.push_back(hres.first->GetId());
//						if (sdres.second->maxHitCount > 0 && sdres.second->entityFilter.size() >= sdres.second->maxHitCount) {
//							if (sdres.second->destroyable) {
//								sdres.first->Destroy();
//							}
//							sdres.first->RemoveComponent<SingleDamager>();
//						}
//					}
//
//					pair<Entity*, MultiDamager*> mdres = entity2->GetAncestorWithComponent<MultiDamager>();
//					if (mdres.first != nullptr && mdres.second->active && mdres.second->target == hres.second->fraction && timer::currentTime - mdres.second->lastDamageTime >= mdres.second->interval) {
//											
//						DamageUnit unit = DamageUnit(mdres.second->damageType, mdres.second->damage);
//						hres.second->damageQueue.push_back(unit);
//
//						mdres.second->lastDamageTime = timer::currentTime;
//						mdres.second->repeatCount++;
//
//						if (mdres.second->repeats >= 0 && mdres.second->repeatCount >= mdres.second->repeats) {
//							if (mdres.second->destroyable) {
//								mdres.first->Destroy();
//							}
//							mdres.first->RemoveComponent<MultiDamager>();
//						}
//					}
//
//				}
//			}
//
//		}
//
//	}
//}