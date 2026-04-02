#ifndef _INTERP_H_
#define _INTERP_H_

#include <functional>
#include <cmath>
#include <vector>
#include <memory>
#include "../timer.h"
#include "../constants.h"

namespace interp {

    enum class Curve {
        Linear,
        EaseInQuad,
        EaseOutQuad,
        EaseInOutQuad,
        EaseInCubic,
        EaseOutCubic,
        EaseInOutCubic,
        EaseOutElastic,
        EaseOutBounce
    };

    inline float EaseCurve(Curve curve, float t);


    class ITween {
    public:
        virtual ~ITween() = default;
        virtual void Update() = 0;
        virtual bool IsPlaying() const = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };

    template<typename T>
    class Tween : public ITween
    {
    private:
        T* target;
        T startValue;
        T endValue;
        double duration;  // в секундах
        double startTime;
        Curve curve;
        bool isPlaying = false;
        bool isPaused = false;
        double pausedTime = 0.0;

        T interpolate(float t) const {
            float easedT = EaseCurve(curve, t);
            return startValue + (endValue - startValue) * easedT;
        }

    public:
        Tween(T* target, const T& endValue, double duration, Curve curve = Curve::Linear)
            : target(target), endValue(endValue), duration(duration), curve(curve) {
            if (target) {
                startValue = *target;
            }
        }

        void Start() {
            if (!target) return;

            startValue = *target;
            startTime = timer::currentTime;
            isPlaying = true;
            isPaused = false;
        }

        void Stop() {
            if (!target) return;

            isPlaying = false;
            isPaused = false;
            *target = startValue;
        }

        void Pause() {
            if (!isPlaying || isPaused) return;

            isPaused = true;
            isPlaying = false;
            pausedTime = timer::currentTime;
        }

        void Resume() {
            if (!isPaused) return;

            double elapsed = pausedTime - startTime;
            startTime = timer::currentTime - elapsed;
            isPlaying = true;
            isPaused = false;
        }

        void Update() {
            if (!isPlaying || !target) return;

            double elapsed = timer::currentTime - startTime;
            float t = (float)std::min(1.0, elapsed / duration);

            *target = interpolate(t);

            if (t >= 1.0f) {
                isPlaying = false;
                *target = endValue;
            }
        }

        bool IsPlaying() const { return isPlaying; }
        bool IsPaused() const { return isPaused; }

        T GetCurrentValue() const { return target ? *target : T{}; }
    };


    static std::vector<std::unique_ptr<ITween>> activeTweens;


    template<typename T>
    Tween<T>& CreateTween(T& target, const T& endValue, double duration, Curve curve = Curve::Linear) {
        auto tween = std::make_unique<Tween<T>>(&target, endValue, duration, curve);
        Tween<T>& ref = *tween;
        activeTweens.push_back(std::move(tween));
        return ref;
    }

    template<typename T>
    Tween<T>& Animate(T& target, const T& endValue, double duration, Curve curve = Curve::Linear) {
        auto& tween = CreateTween(target, endValue, duration, curve);
        tween.Start();
        return tween;
    }

    inline void UpdateAll() {
        for (auto& tween : activeTweens) {
            tween->Update();
        }

        activeTweens.erase(
            std::remove_if(activeTweens.begin(), activeTweens.end(),
                [](const auto& tween) { return !tween->IsPlaying(); }),
            activeTweens.end()
        );
    }

    inline void ClearAllTweens() {
        activeTweens.clear();
    }
}

#endif