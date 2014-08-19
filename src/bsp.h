#ifndef BSP__H
#define BSP__H

#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "ibsp46.h"
#include "vec.h"
#include "zipfile.h"
#include "bezier.h"
#include "frustum.h"

class CBspQ3
{
    typedef std::vector<CBezierSurfQ3 *>    BezierList_t;
    typedef std::vector<GLuint>             GlTexId_t;

	typedef std::vector<int>				IndexList_t;

    char            *m_bspData;

    size_t          m_nTextures;
    DTexture_t      *m_textures;

    size_t          m_nFaces;
    DFace_t         *m_faces;

    size_t          m_nVertices;
    DVertex_t       *m_vertices;

    size_t          m_nPlanes;
    DPlane_t        *m_planes;

    size_t          m_nLeaves;
    DLeaf_t         *m_leaves;

    size_t          m_nLeafFaces;
    DLeafFace_t     *m_leafFaces;

    size_t          m_nNodes;
    DNode_t         *m_nodes;

    size_t          m_nMeshVerts;
    DMeshVert_t     *m_meshVerts;

    DVisData_t      *m_visData;

    BezierList_t    m_beziers;

    GlTexId_t       m_glTexIds;
    GlTexId_t       m_glLmapIds;

    void LoadTextures(const CPk3Archive &);
    void ProcessLightmaps(const DLightmap_t *, size_t);
    
	void DrawFace(size_t) const;

    int FindLeaf(const CVec &) const;
    bool IsClusterVisible(int, int) const;
	
	void CollectLeaves(IndexList_t &, int, const CFrustum &) const;
	void DrawLeaves(const IndexList_t &, const CFrustum &) const;
    
	void Render(const CFrustum &) const;

    CBspQ3(const CBspQ3 &);
    void operator = (const CBspQ3 &);

public:
    CBspQ3(const char *, const CPk3Archive &);
    ~CBspQ3() throw ();

    void Render(const CVec &, const CFrustum &) const;
};

#endif
