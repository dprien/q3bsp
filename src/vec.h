#ifndef FLOAT__H
#define FLOAT__H

#include <cstring>
#include <cmath>

#ifndef M_PI
const double M_PI = 3.14159265358979323846;
#endif

typedef float   Mat_t[4][4];

// =================================================
// =================================================

class CMat;

class CVec
{
public:
	float	x;
	float	y;
	float	z;
	
	CVec(float = 0.0f, float = 0.0f, float = 0.0f);
    explicit CVec(const float *);

    float Magnitude() const;
    float Normalize();

	float Dot(float, float, float) const;
    float Dot(const CVec &) const;

    CVec &operator += (const CVec &);
    CVec &operator -= (const CVec &);
    CVec &operator *= (const CVec &);

    CVec &operator *= (float);

    CVec &operator *= (const CMat &);
    CVec &operator /= (const CMat &);

    friend void MatScale(CMat &, const CVec &);
    friend void MatTrans(CMat &, const CVec &);
};

// public:
// =================================================
inline CVec::CVec(float px, float py, float pz)
    : x(px), y(py), z(pz)
{
}

inline CVec::CVec(const float *v)
    : x(v[0]), y(v[1]), z(v[2])
{
}

inline float CVec::Magnitude() const
{
    return std::sqrt(x * x + y * y + z * z);
}

inline float CVec::Dot(float px, float py, float pz) const
{
	return x * px + y * py + z * pz;
}

inline float CVec::Dot(const CVec &v) const
{
    return Dot(v.x, v.y, v.z);
}

inline CVec &CVec::operator += (const CVec &v)
{
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

inline CVec &CVec::operator -= (const CVec &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
}

inline CVec &CVec::operator *= (float s)
{
    x *= s;
    y *= s;
    z *= s;

    return *this;
}

// =================================================
// =================================================

class CMat
{
    Mat_t   m_mat;

    void MulMat(const Mat_t);
    void InvMulMat(const Mat_t);

public:
    CMat();

    const float *GetMatrix() const;

    CMat &operator *= (const CMat &);
    CMat &operator *= (const Mat_t);

    CMat &operator /= (const CMat &);
    CMat &operator /= (const Mat_t);

    float operator () (int, int) const;

    friend CVec &CVec::operator *= (const CMat &);
    friend CVec &CVec::operator /= (const CMat &);
};

// public:
// =================================================
inline CMat::CMat()
{
    m_mat[0][0] = 1.0f; m_mat[0][1] = 0.0f; m_mat[0][2] = 0.0f; m_mat[0][3] = 0.0f;
    m_mat[1][0] = 0.0f; m_mat[1][1] = 1.0f; m_mat[1][2] = 0.0f; m_mat[1][3] = 0.0f;
    m_mat[2][0] = 0.0f; m_mat[2][1] = 0.0f; m_mat[2][2] = 1.0f; m_mat[2][3] = 0.0f;
    m_mat[3][0] = 0.0f; m_mat[3][1] = 0.0f; m_mat[3][2] = 0.0f; m_mat[3][3] = 1.0f;
}

inline const float *CMat::GetMatrix() const
{
    // FIXME
    return reinterpret_cast<const float *>(m_mat);
}

inline CMat &CMat::operator *= (const CMat &a)
{
    MulMat(a.m_mat);
    return *this;
}

inline CMat &CMat::operator *= (const Mat_t a)
{
    MulMat(a);
    return *this;
}

inline CMat &CMat::operator /= (const CMat &a)
{
    InvMulMat(a.m_mat);
    return *this;
}

inline CMat &CMat::operator /= (const Mat_t a)
{
    InvMulMat(a);
    return *this;
}

inline float CMat::operator () (int i, int j) const
{
    if (i < 0 || j < 0 || i > 3 || j > 3)
        return 0.0;

    return m_mat[j][i];
}

// =================================================
// =================================================

inline CVec operator + (CVec v, const CVec &v1)
{
    return v += v1;
}

inline CVec operator - (CVec v, const CVec &v1)
{
    return v -= v1;
}

inline CVec operator * (CVec v, const CVec &v1)
{
    return v *= v1;
}

inline CVec operator * (CVec v, float s)
{
    return v *= s;
}

inline CVec operator * (CVec v, const CMat &a)
{
    return v *= a;
}

inline CVec operator / (CVec v, const CMat &a)
{
    return v /= a;
}

inline CMat operator * (CMat a, const CMat &a1)
{
    return a *= a1;
}

inline CMat operator / (CMat a, const CMat &a1)
{
    return a /= a1;
}

extern void MatScale(CMat &, float, float, float);
extern void MatTrans(CMat &, float, float, float);

extern void MatXRot(CMat &, float);
extern void MatYRot(CMat &, float);
extern void MatZRot(CMat &, float);

#endif
