#pragma once
struct point3d {
    float x, y, z;

    bool operator==(const point3d& other) const {
        return fabs(x - other.x) < 0.001f &&
            fabs(y - other.y) < 0.001f &&
            fabs(z - other.z) < 0.001f;
    }

    bool operator!=(const point3d& other) const {
        return !(*this == other);
    }
};