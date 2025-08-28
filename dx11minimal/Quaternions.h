#pragma once

#include "dx11.h"

namespace DXMathExtensions {

    class Quaternion {
    private:
        DirectX::XMVECTOR quat;

    public:
        // Конструкторы
        Quaternion() : quat(DirectX::XMQuaternionIdentity()) {}
        Quaternion(float w, float x, float y, float z) : quat(DirectX::XMVectorSet(x, y, z, w)) {}
        Quaternion(const DirectX::XMVECTOR& xmQuat) : quat(xmQuat) {}

        // Конвертация из/в XMVECTOR
        operator DirectX::XMVECTOR() const { return quat; }
        DirectX::XMVECTOR toXMVECTOR() const { return quat; }

        // Статические методы создания
        static Quaternion Identity() { return Quaternion(DirectX::XMQuaternionIdentity()); }
        static Quaternion FromAxisAngle(const DirectX::XMVECTOR& axis, float angle);
        static Quaternion FromEulerAngles(float roll, float pitch, float yaw);
        static Quaternion FromRotationMatrix(const DirectX::XMMATRIX& matrix);
        static Quaternion FromDirection(const DirectX::XMVECTOR& direction, const DirectX::XMVECTOR& up = DirectX::XMVectorSet(0, 1, 0, 0));

        // Основные операции
        Quaternion conjugate() const;
        Quaternion inverse() const;
        Quaternion normalize() const;
        float length() const;
        float lengthSquared() const;

        // Операторы
        Quaternion operator*(const Quaternion& other) const;
        DirectX::XMVECTOR operator*(const DirectX::XMVECTOR& vec) const;
        Quaternion& operator*=(const Quaternion& other);

        // Интерполяции
        static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);
        static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
        static Quaternion Nlerp(const Quaternion& q1, const Quaternion& q2, float t);

        // Преобразования
        DirectX::XMMATRIX toMatrix() const;
        void toAxisAngle(DirectX::XMVECTOR& axis, float& angle) const;
        DirectX::XMFLOAT3 toEulerAngles() const;

        // Утилиты
        float dot(const Quaternion& other) const;
        bool isIdentity(float tolerance = 1e-6f) const;
        bool isNormalized(float tolerance = 1e-6f) const;

        // Получение компонентов
        float w() const { return DirectX::XMVectorGetW(quat); }
        float x() const { return DirectX::XMVectorGetX(quat); }
        float y() const { return DirectX::XMVectorGetY(quat); }
        float z() const { return DirectX::XMVectorGetZ(quat); }

        // Для вращения векторов
        DirectX::XMVECTOR rotateVector(const DirectX::XMVECTOR& vector) const;
        DirectX::XMVECTOR getForward() const;
        DirectX::XMVECTOR getUp() const;
        DirectX::XMVECTOR getRight() const;

        // Плавное вращение к цели
        Quaternion rotateTowards(const Quaternion& target, float maxRadians) const;

        // LookAt функциональность
        static Quaternion LookAt(const DirectX::XMVECTOR& from, const DirectX::XMVECTOR& to,
            const DirectX::XMVECTOR& up = DirectX::XMVectorSet(0, 1, 0, 0));
    };

    // Реализация методов
    inline Quaternion Quaternion::FromAxisAngle(const DirectX::XMVECTOR& axis, float angle) {
        return Quaternion(DirectX::XMQuaternionRotationAxis(axis, angle));
    }

    inline Quaternion Quaternion::FromEulerAngles(float roll, float pitch, float yaw) {
        DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
        return Quaternion(quat);
    }

    inline Quaternion Quaternion::FromRotationMatrix(const DirectX::XMMATRIX& matrix) {
        return Quaternion(DirectX::XMQuaternionRotationMatrix(matrix));
    }

    inline Quaternion Quaternion::FromDirection(const DirectX::XMVECTOR& direction, const DirectX::XMVECTOR& up) {
        DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(direction);
        DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, dir);
        right = DirectX::XMVector3Normalize(right);
        DirectX::XMVECTOR newUp = DirectX::XMVector3Cross(dir, right);

        DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixSet(
            DirectX::XMVectorGetX(right), DirectX::XMVectorGetY(right), DirectX::XMVectorGetZ(right), 0,
            DirectX::XMVectorGetX(newUp), DirectX::XMVectorGetY(newUp), DirectX::XMVectorGetZ(newUp), 0,
            DirectX::XMVectorGetX(dir), DirectX::XMVectorGetY(dir), DirectX::XMVectorGetZ(dir), 0,
            0, 0, 0, 1
        );

        return FromRotationMatrix(rotMatrix);
    }

    inline Quaternion Quaternion::conjugate() const {
        return Quaternion(DirectX::XMQuaternionConjugate(quat));
    }

    inline Quaternion Quaternion::inverse() const {
        return Quaternion(DirectX::XMQuaternionInverse(quat));
    }

    inline Quaternion Quaternion::normalize() const {
        return Quaternion(DirectX::XMQuaternionNormalize(quat));
    }

    inline float Quaternion::length() const {
        return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(quat));
    }

    inline float Quaternion::lengthSquared() const {
        return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(quat));
    }

    inline Quaternion Quaternion::operator*(const Quaternion& other) const {
        return Quaternion(DirectX::XMQuaternionMultiply(quat, other.quat));
    }

    inline DirectX::XMVECTOR Quaternion::operator*(const DirectX::XMVECTOR& vec) const {
        return rotateVector(vec);
    }

    inline Quaternion& Quaternion::operator*=(const Quaternion& other) {
        quat = DirectX::XMQuaternionMultiply(quat, other.quat);
        return *this;
    }

    inline Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t) {
        return Quaternion(DirectX::XMQuaternionSlerp(q1.quat, q2.quat, t));
    }

    inline Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t) {
        return Quaternion(DirectX::XMQuaternionSlerp(q1.quat, q2.quat, t));
    }

    inline Quaternion Quaternion::Nlerp(const Quaternion& q1, const Quaternion& q2, float t) {
        DirectX::XMVECTOR result = DirectX::XMVectorLerp(q1.quat, q2.quat, t);
        return Quaternion(DirectX::XMQuaternionNormalize(result));
    }

    inline DirectX::XMMATRIX Quaternion::toMatrix() const {
        return DirectX::XMMatrixRotationQuaternion(quat);
    }

    inline void Quaternion::toAxisAngle(DirectX::XMVECTOR& axis, float& angle) const {
        DirectX::XMQuaternionToAxisAngle(&axis, &angle, quat);
    }

    inline DirectX::XMFLOAT3 Quaternion::toEulerAngles() const {
        float roll, pitch, yaw;
        DirectX::XMQuaternionToEulerAngles(quat, &pitch, &yaw, &roll);
        return DirectX::XMFLOAT3(roll, pitch, yaw);
    }

    inline float Quaternion::dot(const Quaternion& other) const {
        return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(quat, other.quat));
    }

    inline bool Quaternion::isIdentity(float tolerance) const {
        DirectX::XMVECTOR identity = DirectX::XMQuaternionIdentity();
        float dotValue = DirectX::XMVectorGetX(DirectX::XMQuaternionDot(quat, identity));
        return abs(dotValue - 1.0f) < tolerance;
    }

    inline bool Quaternion::isNormalized(float tolerance) const {
        float lenSq = lengthSquared();
        return abs(lenSq - 1.0f) < tolerance;
    }

    inline DirectX::XMVECTOR Quaternion::rotateVector(const DirectX::XMVECTOR& vector) const {
        return DirectX::XMVector3Rotate(vector, quat);
    }

    inline DirectX::XMVECTOR Quaternion::getForward() const {
        return rotateVector(DirectX::XMVectorSet(0, 0, 1, 0));
    }

    inline DirectX::XMVECTOR Quaternion::getUp() const {
        return rotateVector(DirectX::XMVectorSet(0, 1, 0, 0));
    }

    inline DirectX::XMVECTOR Quaternion::getRight() const {
        return rotateVector(DirectX::XMVectorSet(1, 0, 0, 0));
    }

    inline Quaternion Quaternion::rotateTowards(const Quaternion& target, float maxRadians) const {
        float angle = acosf(fminf(fmaxf(dot(target), -1.0f), 1.0f));

        if (angle < maxRadians) {
            return target;
        }

        float t = maxRadians / angle;
        return Slerp(*this, target, t);
    }

    inline Quaternion Quaternion::LookAt(const DirectX::XMVECTOR& from, const DirectX::XMVECTOR& to,
        const DirectX::XMVECTOR& up) {
        DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(to, from));
        return FromDirection(direction, up);
    }

}