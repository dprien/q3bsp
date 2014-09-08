#ifndef Q3BSP__MATRIX4_H
#define Q3BSP__MATRIX4_H

#include <utility>

#include <vector>

template <class T>
class Matrix4
{
    public:
        Matrix4()
            : m_v(4 * 4)
        {
            Matrix4& m = *this;
            m(0, 0) = T(1); m(0, 1) = T(0); m(0, 2) = T(0); m(0, 3) = T(0);
            m(1, 0) = T(0); m(1, 1) = T(1); m(1, 2) = T(0); m(1, 3) = T(0);
            m(2, 0) = T(0); m(2, 1) = T(0); m(2, 2) = T(1); m(2, 3) = T(0);
            m(3, 0) = T(0); m(3, 1) = T(0); m(3, 2) = T(0); m(3, 3) = T(1);
        }

        Matrix4(const Matrix4&) = default;
        Matrix4& operator=(const Matrix4&) = default;

        Matrix4(Matrix4&&) = default;
        Matrix4& operator=(Matrix4&&) = default;

        T& operator()(const int i, const int j)
        {
            return m_v.at(i * 4 + j);
        }

        T operator()(const int i, const int j) const
        {
            return m_v.at(i * 4 + j);
        }

        Matrix4& operator*=(const Matrix4& rhs)
        {
            const Matrix4<T>& lhs = *this;
            Matrix4<T> t;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    t(i, j) =
                        lhs(i, 0) * rhs(0, j) +
                        lhs(i, 1) * rhs(1, j) +
                        lhs(i, 2) * rhs(2, j) +
                        lhs(i, 3) * rhs(3, j);
                }
            }
            *this = std::move(t);
            return *this;
        }

        Matrix4& operator/=(const Matrix4& rhs)
        {
            const Matrix4<T>& lhs = *this;
            Matrix4<T> t;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    t(i, j) =
                        lhs(0, i) * rhs(0, j) +
                        lhs(1, i) * rhs(1, j) +
                        lhs(2, i) * rhs(2, j) +
                        lhs(3, i) * rhs(3, j);
                }
            }
            *this = std::move(t);
            return *this;
        }

        const T* get_floats() const
        {
            return m_v.data();
        }

    private:
        std::vector<T> m_v;
};

template <class T>
inline Matrix4<T> operator*(Matrix4<T> lhs, const Matrix4<T>& rhs)
{
    return lhs *= rhs;
}

template <class T>
inline Matrix4<T> operator/(Matrix4<T> lhs, const Matrix4<T>& rhs)
{
    return lhs /= rhs;
}

using mat4 = Matrix4<float>;
using dmat4 = Matrix4<double>;

#endif
