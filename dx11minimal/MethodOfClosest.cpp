#ifndef SURFACECOLLISION_CPP
#define SURFACECOLLISION_CPP

#include "MethodOfClosest.h"
#include "SurfaceCollider.cpp"
#include <cmath>
#include <random>

// Вспомогательная функция для вычисления градиента поверхности
static void surfaceGradient(const point3d& position, const SurfaceCollider& surface,
    float& dy_dx, float& dy_dz, float h = 1e-3f) {
    float f_center = surface.getHeight(position.x, position.z);
    float f_xplus = surface.getHeight(position.x + h, position.z);
    float f_zplus = surface.getHeight(position.x, position.z + h);

    dy_dx = (f_xplus - f_center) / h;
    dy_dz = (f_zplus - f_center) / h;
}

// Основная функция поиска ближайшей точки на поверхности
bool findClosestPointOnSurface(const point3d& playerPos,
    const SurfaceCollider& surface,
    point3d& outClosestPoint,
    float& outDistance,
    int maxIterations,
    float collisionThreshold) {

    // Инициализация случайного генератора
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    // Быстрая проверка вертикального расстояния
    float y_projection = surface.getHeight(playerPos.x, playerPos.z);
    //float vertical_distance = std::abs(playerPos.y - y_projection);

    point3d current_pos = playerPos;
    float learning_rate = 0.1f;

    //// Настраиваем параметры в зависимости от расстояния
    //if (vertical_distance > collisionThreshold) {
    //    learning_rate = 0.5f;
    //    maxIterations = maxIterations / 2;
    //}

    point3d best_pos = playerPos;
    float best_distance = 1e20f;

    int num_restarts = 1;//(vertical_distance > collisionThreshold) ? 3 : 1;

    for (int restart = 0; restart < num_restarts; ++restart) {
        if (restart > 0) {
            current_pos = playerPos + point3d(dist(gen), 0, dist(gen));
        }
        else {
            current_pos = playerPos;
        }

        float current_learning_rate = learning_rate;

        for (int iter = 0; iter < maxIterations; ++iter) {
            float y = surface.getHeight(current_pos.x, current_pos.z);

            float dy_dx, dy_dz;
            surfaceGradient(current_pos, surface, dy_dx, dy_dz);

            // Градиент функции расстояния
            float grad_x = 1.0f * ((current_pos.x - playerPos.x) + (y - playerPos.y) * dy_dx);
            float grad_z = 1.0f * ((current_pos.z - playerPos.z) + (y - playerPos.y) * dy_dz);

            // Ограничиваем градиент для стабильности
            float grad_norm = std::sqrt(grad_x * grad_x + grad_z * grad_z);
            if (grad_norm > 10.0f) {
                grad_x = grad_x * 10.0f / grad_norm;
                grad_z = grad_z * 10.0f / grad_norm;
            }

            current_pos.x -= current_learning_rate * grad_x;
            current_pos.z -= current_learning_rate * grad_z;

            // Уменьшаем шаг на поздних итерациях
            if (iter == maxIterations / 2) {
                current_learning_rate *= 0.5f;
            }

            // Сохраняем лучшую точку
            float current_y = surface.getHeight(current_pos.x, current_pos.z);
            point3d current_point(current_pos.x, current_y, current_pos.z);
            float current_distance = (current_point - playerPos).magnitude();

            if (current_distance < best_distance) {
                best_distance = current_distance;
                best_pos = current_point;
            }
        }
    }

    outClosestPoint = best_pos;
    outDistance = best_distance;

    return (best_pos - playerPos).dot(surface.getNormal(best_pos.x, best_pos.z)) > 0;
}

#endif