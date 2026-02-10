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

	void ProcessParticle(Entity* entity, Transform& worldTransform) const;
	static void EmitNewParticles(Entity* entity, const Transform& worldTransform, ParticleEmitter* emitter);
	static double CalculateEmitDelta(const ParticleEmitter& emitter);
	static void AddEmitterOrientation(Entity* entity, Transform& transform);
	static int CalculateEmitCount(const ParticleEmitter& emitter, double elapsedTime, double emitDelta);
	static OrientationComponent ApplySpread(OrientationComponent orientation, const ParticleEmitter& emitter);
	static float GenerateRandomAngle(float spread);
	static double CalculateParticleStartTime(const ParticleEmitter& emitter, double emitDelta, int particleIndex);
	static void CreateParticle(const Transform& worldTransform, ParticleEmitter* emitter,
	                           const OrientationComponent& direction,
	                           double startTime);
	static void CreateReversedParticle(const Transform& worldTransform, ParticleEmitter* emitter,
	                                   const OrientationComponent& direction, double startTime);
	static void CreateNormalParticle(const Transform& worldTransform, ParticleEmitter* emitter,
	                                 const OrientationComponent& direction, double startTime);
	static void UpdateEmitTiming(ParticleEmitter* emitter, double emitDelta, int emittedCount);
	static void UpdateExistingParticles(ParticleEmitter* emitter);

	void RenderParticles(const ParticleEmitter* emitter) const;
	static void SetupShaders(const ParticleEmitter* emitter);
	static void SetupConstantBuffers();
	static void SetupParticleInfo(const ParticleEmitter* emitter);
	static void SetupInputAssembler(const ParticleEmitter* emitter);
	static OrientationComponent CalculateEmitOrientation(const OrientationComponent& baseOrientation, EmitDirection emitDir);
};

#endif