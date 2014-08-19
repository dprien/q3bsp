#include <iostream>
#include <iomanip>
#include <list>
#include <algorithm>

using std::cout;
using std::endl;

#include "bsp.h"
#include "exception.h"
#include "ibsp46.h"
#include "tex.h"
#include "bezier.h"

namespace
{
    void Swizzle(float v[3])
    {
		float t = v[1];

        v[0] = -v[0];
        v[1] = v[2];
        v[2] = t;
    }

	void Swizzle(int v[3])
	{
		int t = v[1];

		v[0] = -v[0];
		v[1] = v[2];
		v[2] = t;
	}

	// AaBb = Axis-aligned bounding box
	void MakeAaBb(const int mins[3], const int maxs[3], CVec &min, CVec &max)
	{
		min.x = mins[0];
		min.y = mins[1];
		min.z = mins[2];

		max.x = maxs[0];
		max.y = maxs[1];
		max.z = maxs[2];
	}
}

// private:
// =================================================
void CBspQ3::LoadTextures(const PAK3Archive &pak)
{
    const char *fileExts[2] = { ".jpg", ".tga" };
    CTexManager &texMgr = CTexManager::Instance();

	cout << "Precaching textures..." << endl;

    for (size_t i = 0; i < m_nTextures; ++i)
    {
        uint32_t texId = 0;

        for (int j = 0; j < 2; ++j)
        {
            std::string texFile = m_textures[i].name;
            texFile += fileExts[j];

            try
            {
                CImageTex bspTex(texFile.c_str(), pak);
                texId = texMgr.Add(bspTex);

                break;
            }
            catch (const QException &e)
            {
            }
        }

        m_glTexIds.push_back(texId);
    }
}

void CBspQ3::ProcessLightmaps(const DLightmap_t *lightmaps, size_t nLightmaps)
{
    CTexManager &texMgr = CTexManager::Instance();

    for (size_t i = 0; i < nLightmaps; ++i)
    {
        CLightmapTex lmap(lightmaps[i]);

        GLuint texId = texMgr.Add(lmap);
        m_glLmapIds.push_back(texId);
    }
}

void CBspQ3::DrawFace(size_t faceIndex) const
{
    const DFace_t &face = m_faces[faceIndex];

    glActiveTexture(GL_TEXTURE0_ARB);

    if (face.texture < m_glTexIds.size())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_glTexIds[face.texture]);
    }
    else
        glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1_ARB);

    if (face.lmIndex < m_glLmapIds.size())
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_glLmapIds[face.lmIndex]);
    }
    else
        glDisable(GL_TEXTURE_2D);

    if (face.type == 1)
    {
        const DVertex_t *vertex = m_vertices + face.vertex;
        const int nVertices = face.nVertices;

        if (nVertices == 3)
            glBegin(GL_TRIANGLES);
        else if (nVertices == 4)
            glBegin(GL_QUADS);
        else
            glBegin(GL_POLYGON);

        for (int j = 0; j < nVertices; ++j)
        {
            const DVertex_t &cv = vertex[j];

            glColor4ubv(cv.color);
            glMultiTexCoord2fv(GL_TEXTURE0_ARB, cv.texCoord);
            glMultiTexCoord2fv(GL_TEXTURE1_ARB, cv.lmCoord);
            glVertex3fv(cv.position);
        }

        glEnd();
    }
    else if (face.type == 2)
        m_beziers[faceIndex]->Render();
    else if (face.type == 3)
    {
        const DVertex_t &vertex = m_vertices[face.vertex];
        glActiveTexture(GL_TEXTURE0_ARB);

        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DVertex_t), vertex.color);
        glTexCoordPointer(2, GL_FLOAT, sizeof(DVertex_t), vertex.texCoord);
        glVertexPointer(3, GL_FLOAT, sizeof(DVertex_t), vertex.position);

        glDrawElements(GL_TRIANGLES, face.nMeshVerts, GL_UNSIGNED_INT, &m_meshVerts[face.meshVert].offset);
    }
}

int CBspQ3::FindLeaf(const CVec &pos) const
{
    int index = 0;

    while (index >= 0)
    {
        const DNode_t &node = m_nodes[index];
        const DPlane_t &plane = m_planes[node.plane];

        CVec v(plane.normal);
        const float dist = v.Dot(pos) - plane.dist;

        if (dist >= 0.0f)
            index = node.front;
        else
            index = node.back;
    }

    return ~index;
}

inline bool CBspQ3::IsClusterVisible(int curCluster, int cluster) const
{
    const int n = (curCluster * m_visData->bytesPerCluster) + (cluster >> 3);

	return m_visData->bitset[n] & (1 << (cluster & 7));
}

void CBspQ3::CollectLeaves(IndexList_t &leaves, int index, const CFrustum &frustum) const
{
	if (index < 0)
	{
		leaves.push_back(~index);
		return;
	}

	const DNode_t &node = m_nodes[index];
	
	CVec boxMin, boxMax;
	MakeAaBb(node.mins, node.maxs, boxMin, boxMax);

	if (!frustum.AaBbVisible(boxMin, boxMax))
		return;

	CollectLeaves(leaves, node.front, frustum);
	CollectLeaves(leaves, node.back, frustum);
}

void DEBUG_DrawAaBb(const CVec &min, const CVec &max)
{
	const float x1 = min.x, y1 = min.y, z1 = min.z;
	const float x2 = max.x, y2 = max.y, z2 = max.z;

	glBegin(GL_LINE_LOOP);
	
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y1, z1);
	glVertex3f(x2, y2, z1);
	glVertex3f(x1, y2, z1);

	glEnd();

	glBegin(GL_LINE_LOOP);

	glVertex3f(x1, y1, z2);
	glVertex3f(x2, y1, z2);
	glVertex3f(x2, y2, z2);
	glVertex3f(x1, y2, z2);

	glEnd();

	glBegin(GL_LINES);

	glVertex3f(x1, y1, z1);
	glVertex3f(x1, y1, z2);

	glVertex3f(x2, y1, z1);
	glVertex3f(x2, y1, z2);

	glVertex3f(x2, y2, z1);
	glVertex3f(x2, y2, z2);

	glVertex3f(x1, y2, z1);
	glVertex3f(x1, y2, z2);

	glEnd();
}

void CBspQ3::DrawLeaves(const IndexList_t &leaves, const CFrustum &frustum) const
{
	CVec boxMin, boxMax;
	IndexList_t faces;
	
	for (IndexList_t::const_iterator p = leaves.begin(); p != leaves.end(); ++p)
    {
        const DLeaf_t &leaf = m_leaves[*p];
		MakeAaBb(leaf.mins, leaf.maxs, boxMin, boxMax);

		if (!frustum.AaBbVisible(boxMin, boxMax))
			continue;		

		const DLeafFace_t *leafFace = m_leafFaces + leaf.leafFace;

		for (int j = 0; j < leaf.nLeafFaces; ++j)
			faces.push_back(leafFace[j].face);
    }

    std::sort(faces.begin(), faces.end());
    IndexList_t::const_iterator end = std::unique(faces.begin(), faces.end());

    for (IndexList_t::const_iterator p = faces.begin(); p != end; ++p)
        DrawFace(*p);

#if 0
	glActiveTexture(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	for (size_t i = 0; i < m_nNodes; ++i)
	{
		const DNode_t &node = m_nodes[i];
		
		MakeAaBb(node.mins, node.maxs, boxMin, boxMax);

		if (frustum.AaBbVisible(boxMin, boxMax))
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);
		
		DEBUG_DrawAaBb(boxMin, boxMax);

		if (i > 10)
			break;
	}
#endif
}

void CBspQ3::Render(const CFrustum &frustum) const
{
	IndexList_t leaves;
	CollectLeaves(leaves, 0, frustum);

	DrawLeaves(leaves, frustum);
}

// public:
// =================================================
CBspQ3::CBspQ3(const char *filename, const PAK3Archive &pak)
{
    auto uptr = pak.read_file(filename);
    m_bspData = uptr.release(); // FIXME: Leak!

    if (!m_bspData)
        throwf<QException>("%s: Couldn't open file from ZIP archive", filename);

    DHeader_t *header = reinterpret_cast<DHeader_t *>(m_bspData);

    for (size_t i = 0; i < sizeof(g_ibspMagic); ++i)
    {
        if (header->magic[i] != g_ibspMagic[i])
        {
            delete [] m_bspData;
            throwf<QException>("%s: Unsupported file format", filename);
        }
    }

    if (header->version != g_ibsp46Version)
        cout << filename << ": Warning: IBSP version " << header->version <<
            " is not supported - graphical corruption may occur" << endl;

    // Directory
    // =================================================
    DDir_t *dir = reinterpret_cast<DDir_t *>(m_bspData + sizeof(DHeader_t));

    // Textures
    // =================================================
    m_nTextures = dir->textures.length / sizeof(DTexture_t);
    m_textures = reinterpret_cast<DTexture_t *>(m_bspData + dir->textures.offset);

    LoadTextures(pak);

    // Faces
    // =================================================
    m_nFaces = dir->faces.length / sizeof(DFace_t);
    m_faces = reinterpret_cast<DFace_t *>(m_bspData + dir->faces.offset);

    // Vertices
    // =================================================
    m_nVertices = dir->vertices.length / sizeof(DVertex_t);
    m_vertices = reinterpret_cast<DVertex_t *>(m_bspData + dir->vertices.offset);

    // Planes
    // =================================================
    m_nPlanes = dir->planes.length / sizeof(DPlane_t);
    m_planes = reinterpret_cast<DPlane_t *>(m_bspData + dir->planes.offset);

    // Leaves
    // =================================================
    m_nLeaves = dir->leaves.length / sizeof(DLeaf_t);
    m_leaves = reinterpret_cast<DLeaf_t *>(m_bspData + dir->leaves.offset);

    // LeafFaces
    // =================================================
    m_nLeafFaces = dir->leafFaces.length / sizeof(DLeafFace_t);
    m_leafFaces = reinterpret_cast<DLeafFace_t *>(m_bspData + dir->leafFaces.offset);

    // Nodes
    // =================================================
    m_nNodes = dir->nodes.length / sizeof(DNode_t);
    m_nodes = reinterpret_cast<DNode_t *>(m_bspData + dir->nodes.offset);

    // MeshVerts
    // =================================================
    m_nMeshVerts = dir->meshVerts.length / sizeof(DMeshVert_t);
    m_meshVerts = reinterpret_cast<DMeshVert_t *>(m_bspData + dir->meshVerts.offset);

    // Lightmaps
    // =================================================
    size_t nLightmaps = dir->lightmaps.length / sizeof(DLightmap_t);
    DLightmap_t *lightmaps = reinterpret_cast<DLightmap_t *>(m_bspData + dir->lightmaps.offset);

    ProcessLightmaps(lightmaps, nLightmaps);

    // VisData
    // =================================================
    m_visData = reinterpret_cast<DVisData_t *>(m_bspData + dir->visData.offset);

    // =================================================

    for (size_t i = 0; i < m_nVertices; ++i)
    {
        Swizzle(m_vertices[i].position);
        Swizzle(m_vertices[i].normal);
    }

    for (size_t i = 0; i < m_nPlanes; ++i)
    {
        Swizzle(m_planes[i].normal);
    }

	for (size_t i = 0; i < m_nNodes; ++i)
	{
		Swizzle(m_nodes[i].mins);
		Swizzle(m_nodes[i].maxs);
	}

	for (size_t i = 0; i < m_nLeaves; ++i)
	{
		Swizzle(m_leaves[i].mins);
		Swizzle(m_leaves[i].maxs);
	}

	cout << "Preprocessing bezier patches..." << endl;

    for (size_t i = 0; i < m_nFaces; ++i)
    {
        const DFace_t &face = m_faces[i];
        CBezierSurfQ3 *bsurf = 0;

        if (face.type == 2)
            bsurf = new CBezierSurfQ3(m_vertices + face.vertex, face.nMax, face.mMax, 7);

        m_beziers.push_back(bsurf);
    }

    cout << filename << ":" << endl;
    cout << "  " << m_nVertices << " vertices, " << endl;
    cout << "  " << m_nFaces << " faces, " << endl;
    cout << "  " << m_nPlanes << " planes, " << endl;
    cout << "  " << m_nNodes << " nodes, " << endl;
    cout << "  " << m_nLeaves << " leaves, " << endl;
    cout << "  " << m_nMeshVerts << " mesh vertices, " << endl;
    cout << "  " << nLightmaps << " lightmaps." << endl;
}

CBspQ3::~CBspQ3() throw ()
{
    delete [] m_bspData;

    CTexManager &texMgr = CTexManager::Instance();

    for (GlTexId_t::const_iterator p = m_glTexIds.begin(); p != m_glTexIds.end(); ++p)
        texMgr.Free(*p);

    for (GlTexId_t::const_iterator p = m_glLmapIds.begin(); p != m_glLmapIds.end(); ++p)
        texMgr.Free(*p);
}

void CBspQ3::Render(const CVec &camPos, const CFrustum &frustum) const
{
    int leafNum = FindLeaf(camPos);
    int cluster = m_leaves[leafNum].cluster;

    if (cluster < 0)
    {
        Render(frustum);
        return;
    }

    IndexList_t leaves;

    for (size_t i = 0; i < m_nLeaves; ++i)
        if (IsClusterVisible(cluster, m_leaves[i].cluster))
			leaves.push_back(i);

	DrawLeaves(leaves, frustum);
}
