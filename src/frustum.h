#ifndef FRUSTUM__H
#define FRUSTUM__H

#include "vec.h"

class CPlane
{
public:
    CVec    n;
    float   d;

    CPlane(float x = 0.0f, float y = 0.0f, float z = 0.0f, float pd = 0.0f)
        : n(x, y, z), d(pd)
    {
    }

    CPlane(const CVec &pn, float pd = 0.0f)
        : n(pn), d(pd)
    {
    }

    void Normalize()
    {
        d /= n.Normalize();
    }

    float Dist(float x, float y, float z) const
    {
        return n.Dot(x, y, z) + d;
    }

    float Dist(const CVec &v) const
    {
        return Dist(v.x, v.y, v.z);
    }
};

class CFrustum
{
    CPlane  m_plane[6];

public:
    CFrustum();

    bool AaBbVisible(const CVec &, const CVec &) const;
};

#endif
