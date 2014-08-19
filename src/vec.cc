#include <cstring>

#include "vec.h"

void MatScale(CMat &a, float sx, float sy, float sz)
{
    Mat_t t;

    t[0][0] = sx;       t[0][1] = 0.0f;     t[0][2] = 0.0f;     t[0][3] = 0.0f;
    t[1][0] = 0.0f;     t[1][1] = sy;       t[1][2] = 0.0f;     t[1][3] = 0.0f;
    t[2][0] = 0.0f;     t[2][1] = 0.0f;     t[2][2] = sz;       t[2][3] = 0.0f;
    t[3][0] = 0.0f;     t[3][1] = 0.0f;     t[3][2] = 0.0f;     t[3][3] = 1.0f;

    a *= t;
}

void MatTrans(CMat &a, float tx, float ty, float tz)
{
    Mat_t t;

    t[0][0] = 1.0f;     t[0][1] = 0.0f;     t[0][2] = 0.0f;     t[0][3] = 0.0f;
    t[1][0] = 0.0f;     t[1][1] = 1.0f;     t[1][2] = 0.0f;     t[1][3] = 0.0f;
    t[2][0] = 0.0f;     t[2][1] = 0.0f;     t[2][2] = 1.0f;     t[2][3] = 0.0f;
    t[3][0] = tx;       t[3][1] = ty;       t[3][2] = tz;       t[3][3] = 1.0f;

    a *= t;
}

void MatXRot(CMat &a, float angle)
{
    angle *= M_PI / 180.0f;
    float s = std::sin(angle), c = std::cos(angle);

    Mat_t t;

    t[0][0] = 1.0f;     t[0][1] = 0.0f;     t[0][2] = 0.0f;     t[0][3] = 0.0f;
    t[1][0] = 0.0f;     t[1][1] = c;        t[1][2] = s;        t[1][3] = 0.0f;
    t[2][0] = 0.0f;     t[2][1] = -s;       t[2][2] = c;        t[2][3] = 0.0f;
    t[3][0] = 0.0f;     t[3][1] = 0.0f;     t[3][2] = 0.0f;     t[3][3] = 1.0f;

    a *= t;
}

void MatYRot(CMat &a, float angle)
{
    angle *= M_PI / 180.0f;
    float s = std::sin(angle), c = std::cos(angle);

    Mat_t t;

    t[0][0] = c;        t[0][1] = 0.0f;     t[0][2] = -s;       t[0][3] = 0.0f;
    t[1][0] = 0.0f;     t[1][1] = 1.0f;     t[1][2] = 0.0f;     t[1][3] = 0.0f;
    t[2][0] = s;        t[2][1] = 0.0f;     t[2][2] = c;        t[2][3] = 0.0f;
    t[3][0] = 0.0f;     t[3][1] = 0.0f;     t[3][2] = 0.0f;     t[3][3] = 1.0f;

    a *= t;
}

void MatZRot(CMat &a, float angle)
{
    angle *= M_PI / 180.0f;
    float s = std::sin(angle), c = std::cos(angle);

    Mat_t t;

    t[0][0] = c;        t[0][1] = s;        t[0][2] = 0.0f;     t[0][3] = 0.0f;
    t[1][0] = -s;       t[1][1] = c;        t[1][2] = 0.0f;     t[1][3] = 0.0f;
    t[2][0] = 0.0f;     t[2][1] = 0.0f;     t[2][2] = 1.0f;     t[2][3] = 0.0f;
    t[3][0] = 0.0f;     t[3][1] = 0.0f;     t[3][2] = 0.0f;     t[3][3] = 1.0f;

    a *= t;
}

// ==========================================================================
// CVec
// ==========================================================================

// public:
// =================================================
float CVec::Normalize()
{
    float m = Magnitude();

    if (m)
    {
        x /= m;
        y /= m;
        z /= m;
    }

    return m;
}

CVec &CVec::operator *= (const CVec &v)
{
    float px = y * v.z - z * v.y;
    float py = z * v.x - x * v.z;
    float pz = x * v.y - y * v.x;

    x = px;
    y = py;
    z = pz;

    return *this;
}

CVec &CVec::operator *= (const CMat &a)
{
    float px =
        x * a.m_mat[0][0] +
        y * a.m_mat[0][1] +
        z * a.m_mat[0][2] +
        1.0f * a.m_mat[0][3];

    float py =
        x * a.m_mat[1][0] +
        y * a.m_mat[1][1] +
        z * a.m_mat[1][2] +
        1.0f * a.m_mat[1][3];

    float pz =
        x * a.m_mat[2][0] +
        y * a.m_mat[2][1] +
        z * a.m_mat[2][2] +
        1.0f * a.m_mat[2][3];

    x = px;
    y = py;
    z = pz;

    return *this;
}

CVec &CVec::operator /= (const CMat &a)
{
    float px =
        x * a.m_mat[0][0] +
        y * a.m_mat[1][0] +
        z * a.m_mat[2][0] +
        1.0f * a.m_mat[3][0];

    float py =
        x * a.m_mat[0][1] +
        y * a.m_mat[1][1] +
        z * a.m_mat[2][1] +
        1.0f * a.m_mat[3][1];

    float pz =
        x * a.m_mat[0][2] +
        y * a.m_mat[1][2] +
        z * a.m_mat[2][2] +
        1.0f * a.m_mat[3][2];

    x = px;
    y = py;
    z = pz;

    return *this;
}

// ==========================================================================
// CMat
// ==========================================================================

// private:
// =================================================
void CMat::MulMat(const Mat_t a)
{
    Mat_t t;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t[i][j] =
                m_mat[i][0] * a[0][j] +
                m_mat[i][1] * a[1][j] +
                m_mat[i][2] * a[2][j] +
                m_mat[i][3] * a[3][j];

    std::memcpy(m_mat, t, sizeof(Mat_t));
}

void CMat::InvMulMat(const Mat_t a)
{
    Mat_t t;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t[i][j] =
                m_mat[0][i] * a[0][j] +
                m_mat[1][i] * a[1][j] +
                m_mat[2][i] * a[2][j] +
                m_mat[3][i] * a[3][j];

    std::memcpy(m_mat, t, sizeof(Mat_t));
}
