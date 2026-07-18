#include "serializer.h"
#include "../../ecsHeaders.h"

using namespace std;
using json = nlohmann::json;

//////////////////////////////////////////////////////////////////////////////////

unordered_map<string, function<json(Component*)>> serializer::components;

void serializer::Initialize() {

    //BASIC COMPONENTS

    components[typeid(Transform).name()] = [](Component* comp) {
        auto* c = static_cast<Transform*>(comp);
        json j = *c;
        j["type"] = typeid(Transform).name();
        return j;
        };

    components[typeid(Transform2D).name()] = [](Component* comp) {
        auto* c = static_cast<Transform2D*>(comp);
        json j = *c;
        j["type"] = typeid(Transform2D).name();
        return j;
        };

    //PHYSIC

    components[typeid(PhysicBody).name()] = [](Component* comp) {
        auto* c = static_cast<PhysicBody*>(comp);
        json j = *c;
        j["type"] = typeid(PhysicBody).name();
        return j;
        };

    components[typeid(RotatingBody).name()] = [](Component* comp) {
        auto* c = static_cast<RotatingBody*>(comp);
        json j = *c;
        j["type"] = typeid(RotatingBody).name();
        return j;
        };

    components[typeid(SphereCollider).name()] = [](Component* comp) {
        auto* c = static_cast<SphereCollider*>(comp);
        json j = *c;
        j["type"] = typeid(SphereCollider).name();
        return j;
        };

    components[typeid(PlaneCollider).name()] = [](Component* comp) {
        auto* c = static_cast<PlaneCollider*>(comp);
        json j = *c;
        j["type"] = typeid(PlaneCollider).name();
        return j;
        };

    components[typeid(GravityPoint).name()] = [](Component* comp) {
        auto* c = static_cast<GravityPoint*>(comp);
        json j = *c;
        j["type"] = typeid(GravityPoint).name();
        return j;
        };

    // DELAYED DESTROY

    components[typeid(DelayedDestroy).name()] = [](Component* comp) {
        auto* c = static_cast<DelayedDestroy*>(comp);
        json j = *c;
        j["type"] = typeid(DelayedDestroy).name();
        return j;
        };

    // COMBAT

    components[typeid(Health).name()] = [](Component* comp) {
        auto* c = static_cast<Health*>(comp);
        json j = *c;
        j["type"] = typeid(Health).name();
        return j;
        };

    components[typeid(SingleDamager).name()] = [](Component* comp) {
        auto* c = static_cast<SingleDamager*>(comp);
        json j = *c;
        j["type"] = typeid(SingleDamager).name();
        return j;
        };

    components[typeid(MultiDamager).name()] = [](Component* comp) {
        auto* c = static_cast<MultiDamager*>(comp);
        json j = *c;
        j["type"] = typeid(MultiDamager).name();
        return j;
        };

    components[typeid(RayDamager).name()] = [](Component* comp) {
        auto* c = static_cast<RayDamager*>(comp);
        json j = *c;
        j["type"] = typeid(RayDamager).name();
        return j;
        };

    components[typeid(DamageBlocker).name()] = [](Component* comp) {
        auto* c = static_cast<DamageBlocker*>(comp);
        json j = *c;
        j["type"] = typeid(DamageBlocker).name();
        return j;
        };

    // SOUND

    components[typeid(SoundPlayer).name()] = [](Component* comp) {
        auto* c = static_cast<SoundPlayer*>(comp);
        json j = *c;
        j["type"] = typeid(SoundPlayer).name();
        return j;
        };

    // RENDER

    components[typeid(Star).name()] = [](Component* comp) {
        auto* c = static_cast<Star*>(comp);
        json j = *c;
        j["type"] = typeid(Star).name();
        return j;
        };

    components[typeid(SpriteCluster).name()] = [](Component* comp) {
        auto* c = static_cast<SpriteCluster*>(comp);
        json j = *c;
        j["type"] = typeid(SpriteCluster).name();
        return j;
        };

    components[typeid(PointCloud).name()] = [](Component* comp) {
        auto* c = static_cast<PointCloud*>(comp);
        json j = *c;
        j["type"] = typeid(PointCloud).name();
        return j;
        };

    components[typeid(Constellation).name()] = [](Component* comp) {
        auto* c = static_cast<Constellation*>(comp);
        json j = *c;
        j["type"] = typeid(Constellation).name();
        return j;
        };

    components[typeid(Beam).name()] = [](Component* comp) {
        auto* c = static_cast<Beam*>(comp);
        json j = *c;
        j["type"] = typeid(Beam).name();
        return j;
        };

    components[typeid(StarClay).name()] = [](Component* comp) {
        auto* c = static_cast<StarClay*>(comp);
        json j = *c;
        j["type"] = typeid(StarClay).name();
        return j;
        };

    components[typeid(ParticleEmitter).name()] = [](Component* comp) {
        auto* c = static_cast<ParticleEmitter*>(comp);
        json j = *c;
        j["type"] = typeid(ParticleEmitter).name();
        return j;
        };

    components[typeid(Nebula).name()] = [](Component* comp) {
        auto* c = static_cast<Nebula*>(comp);
        json j = *c;
        j["type"] = typeid(Nebula).name();
        return j;
        };
}

//////////////////////////////////////////////////////////////////////////////////

unordered_map<string, function<void(Entity*, const json&)>> deserializer::components;

void deserializer::Initialize() {

    //BASIC COMPONENTS

    components[typeid(Transform).name()] = [](Entity* entity, const json& data) {
        Transform* c = entity->AddComponent<Transform>();
        *c = data.get<Transform>();
        };

    components[typeid(Transform2D).name()] = [](Entity* entity, const json& data) {
        Transform2D* c = entity->AddComponent<Transform2D>();
        *c = data.get<Transform2D>();
        };

    //PHYSIC

    components[typeid(PhysicBody).name()] = [](Entity* entity, const json& data) {
        PhysicBody* c = entity->AddComponent<PhysicBody>();
        *c = data.get<PhysicBody>();
        };

}