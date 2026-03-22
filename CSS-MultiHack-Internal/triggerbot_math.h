#pragma once

#include <cmath>

struct Vector3
{
    float x, y, z;
};

inline float Get3DDistance(Vector3 src, Vector3 dst)
{
    return std::sqrt(std::pow(dst.x - src.x, 2.0f) +
        std::pow(dst.y - src.y, 2.0f) +
        std::pow(dst.z - src.z, 2.0f));
}

inline Vector3 CalculateAngle(Vector3 src, Vector3 dst)
{
    Vector3 delta = { dst.x - src.x, dst.y - src.y, dst.z - src.z };
    float hyp = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    Vector3 angles;
    angles.x = std::atan2(-delta.z, hyp) * (180.0f / 3.14159265358979323846f);
    angles.y = std::atan2(delta.y, delta.x) * (180.0f / 3.14159265358979323846f);
    angles.z = 0.0f;
    return angles;
}

inline void ClampAngles(Vector3& angles)
{
    if (angles.x > 89.0f) angles.x = 89.0f;
    if (angles.x < -89.0f) angles.x = -89.0f;
    while (angles.y > 180.0f) angles.y -= 360.0f;
    while (angles.y < -180.0f) angles.y += 360.0f;
    angles.z = 0.0f;
}

inline float GetAngleDistance(Vector3 angle1, Vector3 angle2)
{
    Vector3 delta = { angle1.x - angle2.x, angle1.y - angle2.y, 0.0f };
    ClampAngles(delta);
    return std::sqrt((delta.x * delta.x) + (delta.y * delta.y));
}