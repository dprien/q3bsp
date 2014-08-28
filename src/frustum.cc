#include <cmath>

#include <GL/gl.h>

#include "frustum.h"

CFrustum::CFrustum()
{
    float proj[16];
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    float modl[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    float clip[16];

    clip[0]  = modl[0]  * proj[0] + modl[1]  * proj[4] + modl[2]  * proj[8]  + modl[3]  * proj[12];
    clip[1]  = modl[0]  * proj[1] + modl[1]  * proj[5] + modl[2]  * proj[9]  + modl[3]  * proj[13];
    clip[2]  = modl[0]  * proj[2] + modl[1]  * proj[6] + modl[2]  * proj[10] + modl[3]  * proj[14];
    clip[3]  = modl[0]  * proj[3] + modl[1]  * proj[7] + modl[2]  * proj[11] + modl[3]  * proj[15];

    clip[4]  = modl[4]  * proj[0] + modl[5]  * proj[4] + modl[6]  * proj[8]  + modl[7]  * proj[12];
    clip[5]  = modl[4]  * proj[1] + modl[5]  * proj[5] + modl[6]  * proj[9]  + modl[7]  * proj[13];
    clip[6]  = modl[4]  * proj[2] + modl[5]  * proj[6] + modl[6]  * proj[10] + modl[7]  * proj[14];
    clip[7]  = modl[4]  * proj[3] + modl[5]  * proj[7] + modl[6]  * proj[11] + modl[7]  * proj[15];

    clip[8]  = modl[8]  * proj[0] + modl[9]  * proj[4] + modl[10] * proj[8]  + modl[11] * proj[12];
    clip[9]  = modl[8]  * proj[1] + modl[9]  * proj[5] + modl[10] * proj[9]  + modl[11] * proj[13];
    clip[10] = modl[8]  * proj[2] + modl[9]  * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[8]  * proj[3] + modl[9]  * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8]  + modl[15] * proj[12];
    clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9]  + modl[15] * proj[13];
    clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

    CVec t;

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

    for (int i = 0; i < 6; ++i)
        m_plane[i].Normalize();
}

bool CFrustum::AaBbVisible(const CVec &mn, const CVec &mx) const
{
    for (int i = 0; i < 6; ++i)
    {
        const CPlane &plane = m_plane[i];

        if (plane.Dist(mn.x, mn.y, mn.z) > 0.0f) continue;
        if (plane.Dist(mx.x, mn.y, mn.z) > 0.0f) continue;
        if (plane.Dist(mn.x, mx.y, mn.z) > 0.0f) continue;
        if (plane.Dist(mx.x, mx.y, mn.z) > 0.0f) continue;
        if (plane.Dist(mn.x, mn.y, mx.z) > 0.0f) continue;
        if (plane.Dist(mx.x, mn.y, mx.z) > 0.0f) continue;
        if (plane.Dist(mn.x, mx.y, mx.z) > 0.0f) continue;
        if (plane.Dist(mx.x, mx.y, mx.z) > 0.0f) continue;

        return false;
    }

    return true;
}
