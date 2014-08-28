#ifndef Q3BSP__BSP_H
#define Q3BSP__BSP_H

#include <vector>
#include <list>
#include <cstdint>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "ibsp46.h"
#include "vec.h"
#include "archive.h"
#include "frustum.h"

class BinaryIO;

class SimpleBezierSurface
{
    public:
        SimpleBezierSurface(const DVertex_t* const, const int);

        SimpleBezierSurface(const SimpleBezierSurface&) = delete;
        void operator=(const SimpleBezierSurface&) = delete;

        void draw() const;

    private:
        int                     m_num_vertices;
        std::vector<DVertex_t>  m_vertices;
};

class GLBezierSurface
{
    public:
        GLBezierSurface(const DVertex_t* const, const int, const int, const int);
        ~GLBezierSurface() noexcept;

        GLBezierSurface(const GLBezierSurface&) = delete;
        void operator=(const GLBezierSurface&) = delete;

        void draw() const;

    private:
        GLuint m_gl_list_id;
};

class MapBSP46
{
    public:
        MapBSP46(const char* const, const PAK3Archive&);
        ~MapBSP46() noexcept;

        MapBSP46(const MapBSP46&) = delete;
        void operator=(const MapBSP46&) = delete;

        void draw(const CVec&, const CFrustum&) const;

    private:
        using bezier_vector_t = std::vector<GLBezierSurface*>;
        using tex_id_vector_t = std::vector<GLuint>;
        using index_vector_t = std::vector<int>;

        DHeader_t                   m_header;
        DDir_t                      m_directory;

        std::vector<DTexture_t>     m_textures;
        std::vector<DFace_t>        m_faces;
        std::vector<DVertex_t>      m_vertices;
        std::vector<DPlane_t>       m_planes;
        std::vector<DLeaf_t>        m_leaves;
        std::vector<DLeafFace_t>    m_leaf_faces;
        std::vector<DNode_t>        m_nodes;
        std::vector<DMeshVert_t>    m_mesh_verts;
        std::vector<DLightmap_t>    m_lightmaps;

        DVisData_t                  m_vis_data;
        std::vector<std::uint8_t>   m_vis_bitset;

        bezier_vector_t             m_beziers;

        tex_id_vector_t             m_texture_ids;
        tex_id_vector_t             m_lightmap_ids;

        void bsp_read_header(BinaryIO*);
        void bsp_read_directory(BinaryIO*);
        void bsp_read_textures(BinaryIO*);
        void bsp_read_faces(BinaryIO*);
        void bsp_read_vertices(BinaryIO*);
        void bsp_read_planes(BinaryIO*);
        void bsp_read_leaves(BinaryIO*);
        void bsp_read_leaf_faces(BinaryIO*);
        void bsp_read_nodes(BinaryIO*);
        void bsp_read_mesh_verts(BinaryIO*);
        void bsp_read_lightmaps(BinaryIO*);
        void bsp_read_vis_data(BinaryIO*);

        void load_textures(const PAK3Archive&);
        void process_lightmaps();

        bool is_cluster_visible(const int, const int) const;

        int find_leaf(const CVec&) const;
        void collect_leaves(index_vector_t*, const int, const CFrustum&) const;

        void draw_face(const std::size_t) const;
        void draw_leaves(const index_vector_t&, const CFrustum&) const;

        void draw(const CFrustum&) const;
};

#endif
