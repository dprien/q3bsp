#ifndef Q3BSP__MATH_H
#define Q3BSP__MATH_H

#include <cstring>
#include <cmath>

#include <GL/gl.h>

#include "src/math/vector4.h"
#include "src/math/matrix4.h"

template <class T>
class Plane
{
    public:
        Vector3<T>  n;
        T           d;

        Plane(T x = T(0), T y = T(0), T z = T(0), T pd = T(0))
            : n(x, y, z), d(pd)
        {
        }

        Plane(const Vector3<T>& pn, T pd = T(0))
            : n(pn), d(pd)
        {
        }

        void normalize()
        {
            d /= n.normalize();
        }

        T distance(T x, T y, T z) const
        {
            return n.dot(Vector3<T>(x, y, z)) + d;
        }

        T distance(const Vector3<T>& v) const
        {
            return distance(v.x, v.y, v.z);
        }
};

template <class T>
class GLFrustum
{
    public:
        GLFrustum()
        {
            float p[16];
            glGetFloatv(GL_PROJECTION_MATRIX, p);

            float m[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, m);

            float clip[16];
            clip[0]  = m[0]  * p[0] + m[1]  * p[4] + m[2]  * p[8]  + m[3]  * p[12];
            clip[1]  = m[0]  * p[1] + m[1]  * p[5] + m[2]  * p[9]  + m[3]  * p[13];
            clip[2]  = m[0]  * p[2] + m[1]  * p[6] + m[2]  * p[10] + m[3]  * p[14];
            clip[3]  = m[0]  * p[3] + m[1]  * p[7] + m[2]  * p[11] + m[3]  * p[15];

            clip[4]  = m[4]  * p[0] + m[5]  * p[4] + m[6]  * p[8]  + m[7]  * p[12];
            clip[5]  = m[4]  * p[1] + m[5]  * p[5] + m[6]  * p[9]  + m[7]  * p[13];
            clip[6]  = m[4]  * p[2] + m[5]  * p[6] + m[6]  * p[10] + m[7]  * p[14];
            clip[7]  = m[4]  * p[3] + m[5]  * p[7] + m[6]  * p[11] + m[7]  * p[15];

            clip[8]  = m[8]  * p[0] + m[9]  * p[4] + m[10] * p[8]  + m[11] * p[12];
            clip[9]  = m[8]  * p[1] + m[9]  * p[5] + m[10] * p[9]  + m[11] * p[13];
            clip[10] = m[8]  * p[2] + m[9]  * p[6] + m[10] * p[10] + m[11] * p[14];
            clip[11] = m[8]  * p[3] + m[9]  * p[7] + m[10] * p[11] + m[11] * p[15];

            clip[12] = m[12] * p[0] + m[13] * p[4] + m[14] * p[8]  + m[15] * p[12];
            clip[13] = m[12] * p[1] + m[13] * p[5] + m[14] * p[9]  + m[15] * p[13];
            clip[14] = m[12] * p[2] + m[13] * p[6] + m[14] * p[10] + m[15] * p[14];
            clip[15] = m[12] * p[3] + m[13] * p[7] + m[14] * p[11] + m[15] * p[15];

            // RIGHT
            m_plane[0].n.x = clip[3]  - clip[0];
            m_plane[0].n.y = clip[7]  - clip[4];
            m_plane[0].n.z = clip[11] - clip[8];
            m_plane[0].d   = clip[15] - clip[12];

            // LEFT
            m_plane[1].n.x = clip[3]  + clip[0];
            m_plane[1].n.y = clip[7]  + clip[4];
            m_plane[1].n.z = clip[11] + clip[8];
            m_plane[1].d   = clip[15] + clip[12];

            // BOTTOM
            m_plane[2].n.x = clip[3]  + clip[1];
            m_plane[2].n.y = clip[7]  + clip[5];
            m_plane[2].n.z = clip[11] + clip[9];
            m_plane[2].d   = clip[15] + clip[13];

            // TOP
            m_plane[3].n.x = clip[3]  - clip[1];
            m_plane[3].n.y = clip[7]  - clip[5];
            m_plane[3].n.z = clip[11] - clip[9];
            m_plane[3].d   = clip[15] - clip[13];

            // FAR
            m_plane[4].n.x = clip[3]  - clip[2];
            m_plane[4].n.y = clip[7]  - clip[6];
            m_plane[4].n.z = clip[11] - clip[10];
            m_plane[4].d   = clip[15] - clip[14];

            // NEAR
            m_plane[5].n.x = clip[3]  + clip[2];
            m_plane[5].n.y = clip[7]  + clip[6];
            m_plane[5].n.z = clip[11] + clip[10];
            m_plane[5].d   = clip[15] + clip[14];

            for (int i = 0; i < 6; ++i) {
                m_plane[i].normalize();
            }
        }

        bool is_aabb_visible(const Vector3<T>& vmin, const Vector3<T>& vmax)
            const
        {
            for (int i = 0; i < 6; ++i) {
                const Plane<T>& plane = m_plane[i];
                if (plane.distance(vmin.x, vmin.y, vmin.z) < T(0) &&
                        plane.distance(vmax.x, vmin.y, vmin.z) < T(0) &&
                        plane.distance(vmin.x, vmax.y, vmin.z) < T(0) &&
                        plane.distance(vmax.x, vmax.y, vmin.z) < T(0) &&
                        plane.distance(vmin.x, vmin.y, vmax.z) < T(0) &&
                        plane.distance(vmax.x, vmin.y, vmax.z) < T(0) &&
                        plane.distance(vmin.x, vmax.y, vmax.z) < T(0) &&
                        plane.distance(vmax.x, vmax.y, vmax.z) < T(0)) {
                    return false;
                }
            }
            return true;
        }

    private:
        Plane<T> m_plane[6];
};

template <class T>
void mat4_scale(Matrix4<T>& a, const float sx, const float sy, const float sz)
{
    Matrix4<T> t;
    t(0, 0) = sx;   t(0, 1) = T(0); t(0, 2) = T(0); t(0, 3) = T(0);
    t(1, 0) = T(0); t(1, 1) = sy;   t(1, 2) = T(0); t(1, 3) = T(0);
    t(2, 0) = T(0); t(2, 1) = T(0); t(2, 2) = sz;   t(2, 3) = T(0);
    t(3, 0) = T(0); t(3, 1) = T(0); t(3, 2) = T(0); t(3, 3) = T(1);

    a *= t;
}

template <class T>
void mat4_translate(Matrix4<T>& a, const float tx, const float ty,
        const float tz)
{
    Matrix4<T> t;
    t(0, 0) = T(1); t(0, 1) = T(0); t(0, 2) = T(0); t(0, 3) = T(0);
    t(1, 0) = T(0); t(1, 1) = T(1); t(1, 2) = T(0); t(1, 3) = T(0);
    t(2, 0) = T(0); t(2, 1) = T(0); t(2, 2) = T(1); t(2, 3) = T(0);
    t(3, 0) = tx;   t(3, 1) = ty;   t(3, 2) = tz;   t(3, 3) = T(1);

    a *= t;
}

template <class T>
void mat4_rotate_x(Matrix4<T>& a, const float deg)
{
    const float rad = static_cast<float>(deg * M_PI / 180.0);
    float s = std::sin(rad), c = std::cos(rad);

    Matrix4<T> t;
    t(0, 0) = T(1); t(0, 1) = T(0); t(0, 2) = T(0); t(0, 3) = T(0);
    t(1, 0) = T(0); t(1, 1) = c;    t(1, 2) = s;    t(1, 3) = T(0);
    t(2, 0) = T(0); t(2, 1) = -s;   t(2, 2) = c;    t(2, 3) = T(0);
    t(3, 0) = T(0); t(3, 1) = T(0); t(3, 2) = T(0); t(3, 3) = T(1);

    a *= t;
}

template <class T>
void mat4_rotate_y(Matrix4<T>& a, const float deg)
{
    const float rad = static_cast<float>(deg * M_PI / 180.0);
    float s = std::sin(rad), c = std::cos(rad);

    Matrix4<T> t;
    t(0, 0) = c;    t(0, 1) = T(0); t(0, 2) = -s;   t(0, 3) = T(0);
    t(1, 0) = T(0); t(1, 1) = T(1); t(1, 2) = T(0); t(1, 3) = T(0);
    t(2, 0) = s;    t(2, 1) = T(0); t(2, 2) = c;    t(2, 3) = T(0);
    t(3, 0) = T(0); t(3, 1) = T(0); t(3, 2) = T(0); t(3, 3) = T(1);

    a *= t;
}

template <class T>
void mat4_rotate_z(Matrix4<T>& a, const float deg)
{
    const float rad = static_cast<float>(deg * M_PI / 180.0);
    float s = std::sin(rad), c = std::cos(rad);

    Matrix4<T> t;
    t(0, 0) = c;    t(0, 1) = s;    t(0, 2) = T(0); t(0, 3) = T(0);
    t(1, 0) = -s;   t(1, 1) = c;    t(1, 2) = T(0); t(1, 3) = T(0);
    t(2, 0) = T(0); t(2, 1) = T(0); t(2, 2) = T(1); t(2, 3) = T(0);
    t(3, 0) = T(0); t(3, 1) = T(0); t(3, 2) = T(0); t(3, 3) = T(1);

    a *= t;
}

#endif
