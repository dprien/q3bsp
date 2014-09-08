#ifndef Q3BSP__BSP_H
#define Q3BSP__BSP_H

#include <type_traits>
#include <vector>
#include <list>
#include <cstdint>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "src/ibsp46.h"
#include "src/archive.h"
#include "src/texture.h"
#include "src/math/vector3.h"

class BinaryIO;

template <class T>
class GLFrustum;

class SimpleBezierSurface
{
    public:
        SimpleBezierSurface(const DVertex_t* const, const unsigned);

        SimpleBezierSurface(const SimpleBezierSurface&) = delete;
        void operator=(const SimpleBezierSurface&) = delete;

        void draw() const;

    private:
        unsigned                m_num_vertices;
        std::vector<DVertex_t>  m_vertices;
};

class GLBezierSurface
{
    public:
        GLBezierSurface(const DVertex_t* const, const int, const int,
                const unsigned);
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

        void draw(const vec3&, const GLFrustum<float>&) const;

    private:
        TextureManager              m_tex_mgr;

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

        std::vector<GLBezierSurface*> m_beziers;

        std::vector<GLuint>         m_texture_ids;
        std::vector<GLuint>         m_lightmap_ids;

        using leaf_ptr_vec_t = std::vector<const DLeaf_t*>;

        using face_index_size_t = std::common_type<
            decltype(m_faces)::size_type,
            decltype(m_beziers)::size_type>::type;
        using face_index_vec_t = std::vector<face_index_size_t>;

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

        void draw_face(const face_index_size_t) const;
        void draw_leaves(const leaf_ptr_vec_t&, const GLFrustum<float>&) const;

        const DLeaf_t& find_leaf(const vec3&) const;
        bool is_cluster_visible(const std::int32_t, const std::int32_t) const;

        void collect_leaves(leaf_ptr_vec_t*, const int, const GLFrustum<float>&)
            const;
        void draw(const GLFrustum<float>&) const;
};

#endif
