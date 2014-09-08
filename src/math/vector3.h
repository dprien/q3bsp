#ifndef Q3BSP__VECTOR3_H
#define Q3BSP__VECTOR3_H

#include <cmath>

template <class T>
class Vector4;

template <class T>
class Vector3
{
    public:
        T x, y, z;

        explicit Vector3(const T px = T(0), const T py = T(0), const T pz = T(0))
            : x(px), y(py), z(pz)
        {}

        explicit Vector3(const Vector4<T>& other)
            : x(other.x), y(other.y), z(other.z)
        {}

        Vector3(const Vector3&) = default;
        Vector3& operator=(const Vector3&) = default;

        T length() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        T dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

        T normalize()
        {
            const T m = length();
            if (m != T(0)) {
                x /= m;
                y /= m;
                z /= m;
            }
            return m;
        }

        Vector3& cross(const Vector3<T>& other)
        {
            const T px = y * other.z - z * other.y;
            const T py = z * other.x - x * other.z;
            const T pz = x * other.y - y * other.x;
            x = px;
            y = py;
            z = pz;
            return *this;
        }

        Vector3& operator+=(const Vector3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        Vector3& operator*=(const T rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            return *this;
        }
};

template <class T>
inline Vector3<T> operator+(Vector3<T> lhs, const Vector3<T>& rhs)
{
    return lhs += rhs;
}

template <class T>
inline Vector3<T> operator-(Vector3<T> lhs, const Vector3<T>& rhs)
{
    return lhs -= rhs;
}

template <class T>
inline Vector3<T> operator*(Vector3<T> lhs, const T rhs)
{
    return lhs *= rhs;
}

using vec3 = Vector3<float>;
using dvec3 = Vector3<double>;

#endif
