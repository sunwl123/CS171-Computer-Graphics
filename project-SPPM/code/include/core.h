#ifndef CORE_H_
#define CORE_H_

#include <Eigen/Core>
#include <Eigen/Dense>

#define UNIMPLEMENTED

template <typename T, int size>
using Vec = Eigen::Matrix<T, size, 1>;

using Vec3f = Eigen::Vector3f;
using Vec3i = Eigen::Vector3i;
using Vec2f = Eigen::Vector2f;
using Vec2i = Eigen::Vector2i;
using Vec4f = Eigen::Vector4f;
using Mat3f = Eigen::Matrix3f;


constexpr float RAY_DEFAULT_MIN = 1e-5;
constexpr float RAY_DEFAULT_MAX = 1e7;
constexpr float PI = 3.141592653579f;
constexpr float INV_PI = 0.31830988618379067154;
constexpr float EPS = 1e-5;

inline Vec3f minVec3f(const Vec3f &a, const Vec3f &b)
{
    return Vec3f(std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z()));
}

inline Vec3f maxVec3f(const Vec3f &a, const Vec3f &b)
{
    return Vec3f(std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z()));
}

inline bool operator>= (const Vec3f &a, const Vec3f &b)
{
    return a.x()+1e-6 >= b.x() && a.y()+1e-6 >= b.y() && a.z()+1e-6 >= b.z();
}

inline bool operator<= (const Vec3f &a, const Vec3f &b)
{
    return a.x()-1e-6 <= b.x() && a.y()-1e-6 <= b.y() && a.z()-1e-6 <= b.z();
}

#endif //CORE_H_
