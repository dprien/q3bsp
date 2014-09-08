#ifndef Q3BSP__VECTOR4_H
#define Q3BSP__VECTOR4_H

#include <cmath>

template <class T>
class Vector3;

template <class T>
class Matrix4;

template <class T>
class Vector4
{
    public:
        T x, y, z, w;

        explicit Vector4(const T px = T(0), const T py = T(0), const T pz = T(0),
                const T pw = T(0))
            : x(px), y(py), z(pz), w(pw)
        {}

        explicit Vector4(const Vector3<T>& other, const T pw = T(0))
            : x(other.x), y(other.y), z(other.z), w(pw)
        {}

        Vector4(const Vector4&) = default;
        Vector4& operator=(const Vector4&) = default;

        T length() const
        {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        T dot(const Vector4& other) const
        {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        T normalize()
        {
            const T m = length();
            if (m != T(0)) {
                x /= m;
                y /= m;
                z /= m;
                w /= m;
            }
            return m;
        }

        Vector4& operator+=(const Vector4& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        Vector4& operator-=(const Vector4& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        Vector4& operator*=(const T s)
        {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        Vector4& operator*=(const Matrix4<T>& a)
        {
            const T px = x * a(0, 0) + y * a(0, 1) + z * a(0, 2) + w * a(0, 3);
            const T py = x * a(1, 0) + y * a(1, 1) + z * a(1, 2) + w * a(1, 3);
            const T pz = x * a(2, 0) + y * a(2, 1) + z * a(2, 2) + w * a(2, 3);
            x = px;
            y = py;
            z = pz;
            return *this;
        }

        Vector4& operator/=(const Matrix4<T>& a)
        {
            const T px = x * a(0, 0) + y * a(1, 0) + z * a(2, 0) + w * a(3, 0);
            const T py = x * a(0, 1) + y * a(1, 1) + z * a(2, 1) + w * a(3, 1);
            const T pz = x * a(0, 2) + y * a(1, 2) + z * a(2, 2) + w * a(3, 2);
            x = px;
            y = py;
            z = pz;
            return *this;
        }
};

template <class T>
inline Vector4<T> operator+(Vector4<T> lhs, const Vector4<T>& rhs)
{
    return lhs += rhs;
}

template <class T>
inline Vector4<T> operator-(Vector4<T> lhs, const Vector4<T>& rhs)
{
    return lhs -= rhs;
}

template <class T>
inline Vector4<T> operator*(Vector4<T> lhs, const T rhs)
{
    return lhs *= rhs;
}

template <class T>
inline Vector4<T> operator*(Vector4<T> lhs, const Matrix4<T>& rhs)
{
    return lhs *= rhs;
}

template <class T>
inline Vector4<T> operator/(Vector4<T> lhs, const Matrix4<T>& rhs)
{
    return lhs /= rhs;
}

using vec4 = Vector4<float>;
using dvec4 = Vector4<double>;

#endif
