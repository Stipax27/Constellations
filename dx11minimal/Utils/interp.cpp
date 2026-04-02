#include "interp.h"

inline float interp::EaseCurve(Curve curve, float t) {
    switch (curve) {
    case Curve::Linear: return t;
    case Curve::EaseInQuad: return t * t;
    case Curve::EaseOutQuad: return t * (2 - t);
    case Curve::EaseInOutQuad:
        return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
    case Curve::EaseInCubic: return t * t * t;
    case Curve::EaseOutCubic:
        return (--t) * t * t + 1;
    case Curve::EaseInOutCubic:
        return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
    case Curve::EaseOutElastic: {
        const float c4 = (2 * 3.14159f) / 3;
        return t == 0 ? 0 : t == 1 ? 1 :
            powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
    }
    case Curve::EaseOutBounce: {
        if (t < 1 / 2.75f) {
            return 7.5625f * t * t;
        }
        else if (t < 2 / 2.75f) {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        }
        else if (t < 2.5 / 2.75f) {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        }
        else {
            t -= 2.625f / 2.75f;
            return 7.5625f * t * t + 0.984375f;
        }
    }
    default: return t;
    }
}