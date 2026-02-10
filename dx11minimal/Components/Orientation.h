#pragma once
#include "../Point3d.h"
#include "../component.h"
#include "../Transform.h"
#include <DirectXMath.h>

class OrientationComponent : public Component {
public:
    point3d forward;
    point3d right;
    point3d up;

    OrientationComponent() = default;

    OrientationComponent(const point3d& f, const point3d& r, const point3d& u)
        : forward(f), right(r), up(u) {}

    void SetFromTransform(Transform& transform) {
        forward = transform.GetLookVector();
        right = transform.GetRightVector();
        up = transform.GetUpVector();
    }

    void SetFromDirections(const point3d& f, const point3d& r, const point3d& u) {
        forward = f;
        right = r;
        up = u;
    }

    void Orthogonalize() {
        forward = forward.normalized();
        right = (right - forward * forward.dot(right)).normalized();
        up = forward.cross(right);
    }
};