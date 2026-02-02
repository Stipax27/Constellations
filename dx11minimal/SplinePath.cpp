// SplinePath.h
#pragma once
#include "Point3d.h"
#include <vector>

class SplinePath {
public:
    struct SplineSegment {
        point3d p0, p1, p2, p3;
        float length;
    };

    std::vector<SplineSegment> segments;
    float totalLength = 0.0f;

    void BuildSpline(const std::vector<point3d>& waypoints) {
        segments.clear();
        totalLength = 0.0f;

        if (waypoints.size() < 2) return;

        for (size_t i = 0; i < waypoints.size(); ++i) {
            SplineSegment seg;

            point3d p0 = waypoints[(i - 1 + waypoints.size()) % waypoints.size()];
            point3d p1 = waypoints[i];
            point3d p2 = waypoints[(i + 1) % waypoints.size()];
            point3d p3 = waypoints[(i + 2) % waypoints.size()];

            seg.p0 = p1;
            seg.p1 = p1 + (p2 - p0) * 0.2f;
            seg.p2 = p2 - (p3 - p1) * 0.2f;
            seg.p3 = p2;

            seg.length = (p2 - p1).magnitude();
            totalLength += seg.length;

            segments.push_back(seg);
        }
    }

    point3d Evaluate(float t) const {
        if (segments.empty()) return point3d();

        float normalizedT = t * totalLength;
        float accumulated = 0.0f;

        for (const auto& seg : segments) {
            if (normalizedT <= accumulated + seg.length) {
                float localT = (normalizedT - accumulated) / seg.length;
                return CubicBezier(seg.p0, seg.p1, seg.p2, seg.p3, localT);
            }
            accumulated += seg.length;
        }

        return segments.back().p3;
    }

private:
    point3d CubicBezier(point3d p0, point3d p1, point3d p2, point3d p3, float t) const {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        point3d p = p0 * uuu;
        p += p1 * 3 * uu * t;
        p += p2 * 3 * u * tt;
        p += p3 * ttt;

        return p;
    }
};