// $Id$

#include <cmath>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "bezier.h"

// public:
// =================================================
CSimpleBezierSurf::CSimpleBezierSurf(const DVertex_t *controls, int steps)
    : m_nVertices(steps + 1)
{
    m_vertices = new DVertex_t[m_nVertices * m_nVertices];
    DVertex_t *cv = m_vertices;

    for (int i = 0; i < m_nVertices; ++i)
    {
        float a = i / float(steps);
        float b = 1.0f - a;

        float t0 = b * b;
        float t1 = 2.0f * b * a;
        float t2 = a * a;

        DVertex_t temp[3], *tv = temp;

        for (int j = 0; j < 3; ++j, ++tv)
        {
            const int row = 3 * j;

            const DVertex_t &v1 = controls[row + 0];
            const DVertex_t &v2 = controls[row + 1];
            const DVertex_t &v3 = controls[row + 2];

            for (int k = 0; k < 2; ++k)
            {
                tv->texCoord[k] =
                    t0 * v1.texCoord[k] +
                    t1 * v2.texCoord[k] +
                    t2 * v3.texCoord[k];

                tv->lmCoord[k] =
                    t0 * v1.lmCoord[k] +
                    t1 * v2.lmCoord[k] +
                    t2 * v3.lmCoord[k];
            }

            for (int k = 0; k < 3; ++k)
            {
                tv->position[k] =
                    t0 * v1.position[k] +
                    t1 * v2.position[k] +
                    t2 * v3.position[k];
            }

            for (int k = 0; k < 4; ++k)
            {
                tv->color[k] = uint8_t(
                    t0 * v1.color[k] +
                    t1 * v2.color[k] +
                    t2 * v3.color[k]);
            }
        }

        for (int j = 0; j < m_nVertices; ++j, ++cv)
        {
            a = j / float(steps);
            b = 1.0f - a;

            t0 = b * b;
            t1 = 2.0f * b * a;
            t2 = a * a;

            for (int k = 0; k < 2; ++k)
            {
                cv->texCoord[k] =
                    t0 * temp[0].texCoord[k] +
                    t1 * temp[1].texCoord[k] +
                    t2 * temp[2].texCoord[k];

                cv->lmCoord[k] =
                    t0 * temp[0].lmCoord[k] +
                    t1 * temp[1].lmCoord[k] +
                    t2 * temp[2].lmCoord[k];
            }

            for (int k = 0; k < 3; ++k)
            {
                cv->position[k] =
                    t0 * temp[0].position[k] +
                    t1 * temp[1].position[k] +
                    t2 * temp[2].position[k];
            }

            for (int k = 0; k < 4; ++k)
            {
                cv->color[k] = uint8_t(
                    t0 * temp[0].color[k] +
                    t1 * temp[1].color[k] +
                    t2 * temp[2].color[k]);
            }
        }
    }
}

CSimpleBezierSurf::~CSimpleBezierSurf() throw ()
{
    delete [] m_vertices;
}

void CSimpleBezierSurf::Render() const
{
    const DVertex_t *cv1 = m_vertices;
    const DVertex_t *cv2 = m_vertices + m_nVertices;

    for (int i = 0; i < m_nVertices - 1; ++i)
    {
        glBegin(GL_QUAD_STRIP);

        for (int j = 0; j < m_nVertices; ++j, ++cv1, ++cv2)
        {
            glColor4ubv(cv2->color);
            glMultiTexCoord2fv(GL_TEXTURE0_ARB, cv2->texCoord);
            glMultiTexCoord2fv(GL_TEXTURE1_ARB, cv2->lmCoord);
            glVertex3fv(cv2->position);

            glColor4ubv(cv1->color);
            glMultiTexCoord2fv(GL_TEXTURE0_ARB, cv1->texCoord);
            glMultiTexCoord2fv(GL_TEXTURE1_ARB, cv1->lmCoord);
            glVertex3fv(cv1->position);
        }

        glEnd();
    }

    /*glActiveTexture(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    glColor3f(0.0f, 1.0f, 0.0f);

	glDisable(GL_DEPTH_TEST);

    const DVertex_t *cv = m_vertices;

    for (int i = 0; i < m_nVertices; ++i)
    {
        glBegin(GL_LINE_STRIP);

        for (int j = 0; j < m_nVertices; ++j, ++cv)
            glVertex3fv(cv->position);

        glEnd();
	}

    glActiveTexture(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);*/
}

// =======================================================================
// =======================================================================

// public:
// =================================================
CBezierSurfQ3::CBezierSurfQ3(const DVertex_t *vertices, int nMax, int mMax, int steps)
{
    DVertex_t controls[9];

    m_glListId = glGenLists(1);
    glNewList(m_glListId, GL_COMPILE);

    for (int i = 0; i < mMax - 2; i += 2)
    {
        for (int j = 0; j < nMax - 2; j += 2)
        {
            for (int m = 0; m < 3; ++m)
                for (int n = 0; n < 3; ++n)
                    controls[m * 3 + n] = vertices[(i + m) * nMax + (j + n)];

            CSimpleBezierSurf bezier(controls, steps);
            bezier.Render();
        }
    }

    glEndList();
}

CBezierSurfQ3::~CBezierSurfQ3() throw ()
{
    glDeleteLists(m_glListId, 1);
}

void CBezierSurfQ3::Render() const
{
    glCallList(m_glListId);
}
