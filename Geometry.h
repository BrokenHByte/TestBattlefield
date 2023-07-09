#pragma once
#include <cmath>

struct Vector3
{
    Vector3() = default;
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator +(const Vector3& p) const
    {
        return { x + p.x, y + p.y, z + p.z };
    }

    Vector3& operator+=(const Vector3& p) {
        x += p.x;
        y += p.y;       
        z += p.z;
        return *this;
    }

    Vector3 operator -(const Vector3& p) const
    {
        return { x - p.x, y - p.y, z - p.z };
    }

    Vector3& operator-=(const Vector3& p) {
        x -= p.x;
        y -= p.y;
        z -= p.z;
        return *this;
    }

    Vector3 operator *(float s) const
    {
        return { x * s, y * s , z * s };
    }

    float length2() const
    {
        return x * x + y * y + z * z;
    }

    Vector3 normalize() const
    {
        float invLength = 1.0f / length();
        return *this * invLength;
    }

    float length() const
    {
        return std::sqrt(length2());
    }

    float x, y, z;
};

