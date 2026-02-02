#ifndef SURFACECOLLISION_H
#define SURFACECOLLISION_H

#include "point3d.h"

// Forward declaration
struct SurfaceCollider;

bool findClosestPointOnSurface(const point3d& playerPos,
    const SurfaceCollider& surface,
    point3d& outClosestPoint,
    float& outDistance,
    int maxIterations = 20,
    float collisionThreshold = 2.0f);

#endif
