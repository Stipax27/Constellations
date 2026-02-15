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

	void ProcessParticle(Entity*, Transform&);
	void AddEmitterOrientation(Entity*, Transform&);
	void EmitNewParticles(Entity*, const Transform&, ParticleEmitter*);
	double CalculateEmitDelta(const ParticleEmitter&);
	int CalculateEmitCount(const ParticleEmitter&, double, double);
	Orientation ApplySpread(Orientation, const ParticleEmitter&);
	float GenerateRandomAngle(float spread);
	double CalculateParticleStartTime(Entity*, const ParticleEmitter&, double, int);
	void CreateParticle(Entity*, const Transform&, ParticleEmitter*, const Orientation&, double);
	void CreateReversedParticle(const Transform&, ParticleEmitter*, const Orientation&, double);
	void CreateNormalParticle(const Transform&, ParticleEmitter*, const Orientation&, double);
	void UpdateEmitTiming(Entity*, ParticleEmitter*, double, int);
	void UpdateExistingParticles(Entity*, ParticleEmitter*);
	void RenderParticles(Entity*, const ParticleEmitter*);
	void SetupShaders(const ParticleEmitter*);
	void SetupConstantBuffers(Entity*);
	void SetupParticleInfo(Entity*, const ParticleEmitter*);
	void SetupInputAssembler(const ParticleEmitter*);
	Orientation CalculateEmitOrientation(const Orientation&, EmitDirection);
};

#endif