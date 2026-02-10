#ifndef _SPRITE_SYSTEM_H_
#define _SPRITE_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"
#include "SpriteCluster.h"
#include "Constellation.h"
#include "Star.h"
#include "PointCloud.h"
#include "ParticleEmitter.h"
#include "Beam.h"

#include "frustumclass.h"
#include "Explosion.cpp"
#include "Components/Orientation.h"


class SpriteSystem : public System
{
public:
	SpriteSystem(FrustumClass*);
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);

private:
	FrustumClass* frustum;

private:
	XMMATRIX GetWorldMatrix(Transform);
	void UpdateWorldMatrix(Transform);

	static void ProcessParticle(Entity* entity, Transform& worldTransform);
	static void EmitNewParticles(Entity* entity, const Transform& worldTransform, ParticleEmitter* emitter);
	static double CalculateEmitDelta(const ParticleEmitter& emitter);
	static void AddEmitterOrientation(Entity* entity, Transform& transform);
	static int CalculateEmitCount(const ParticleEmitter& emitter, double elapsedTime, double emitDelta);
	static Orientation ApplySpread(Orientation orientation, const ParticleEmitter& emitter);
	static float GenerateRandomAngle(float spread);
	static double CalculateParticleStartTime(const ParticleEmitter& emitter, double emitDelta, int particleIndex);
	static void CreateParticle(const Transform& worldTransform, ParticleEmitter* emitter,
	                           const Orientation& direction,
	                           double startTime);
	static void CreateReversedParticle(const Transform& worldTransform, ParticleEmitter* emitter,
	                                   const Orientation& direction, double startTime);
	static void CreateNormalParticle(const Transform& worldTransform, ParticleEmitter* emitter,
	                                 const Orientation& direction, double startTime);
	static void UpdateEmitTiming(ParticleEmitter* emitter, double emitDelta, int emittedCount);
	static void UpdateExistingParticles(ParticleEmitter* emitter);

	static void RenderParticles(const ParticleEmitter* emitter);
	static void SetupShaders(const ParticleEmitter* emitter);
	static void SetupConstantBuffers();
	static void SetupParticleInfo(const ParticleEmitter* emitter);
	static void SetupInputAssembler(const ParticleEmitter* emitter);
	static Orientation CalculateEmitOrientation(const Orientation& baseOrientation, EmitDirection emitDir);
};

#endif