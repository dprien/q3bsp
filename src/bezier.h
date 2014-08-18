// $Id$

#ifndef BEZIER__H
#define BEZIER__H

#include <list>

#include "ibsp46.h"

class CSimpleBezierSurf
{
    int         m_nVertices;
    DVertex_t   *m_vertices;

    CSimpleBezierSurf(const CSimpleBezierSurf &);
    void operator = (const CSimpleBezierSurf &);

public:
    CSimpleBezierSurf(const DVertex_t *, int);
    ~CSimpleBezierSurf() throw ();

    void Render() const;
};

// =======================================================================
// =======================================================================

class CBezierSurfQ3
{
    GLuint          m_glListId;

    CBezierSurfQ3(const CBezierSurfQ3 &);
    void operator = (const CBezierSurfQ3 &);

public:
    CBezierSurfQ3(const DVertex_t *, int, int, int);
    ~CBezierSurfQ3() throw ();

    void Render() const;
};

#endif
