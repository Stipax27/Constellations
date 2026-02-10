#pragma once
#include "../Point3d.h"
#include "../component.h"
#include "../Transform.h"
#include <DirectXMath.h>

class Orientation : public Component {
public:
    point3d forward;
    point3d right;
    point3d up;

    Orientation() = default;

    Orientation(const point3d& f, const point3d& r, const point3d& u)
        : forward(f), right(r), up(u) {}

    void SetFromTransform(Transform& transform);

    void SetFromDirections(const point3d& f, const point3d& r, const point3d& u);

    void Orthogonalize();
};