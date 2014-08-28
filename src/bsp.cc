#include <algorithm>
#include <iostream>

#include "bsp.h"
#include "exception.h"
#include "tex.h"
#include "binio.h"

namespace
{
    void swizzle(float v[3])
    {
        float t = v[1];
        v[0] = -v[0];
        v[1] = v[2];
        v[2] = t;
    }

    void swizzle(int v[3])
    {
        int t = v[1];
        v[0] = -v[0];
        v[1] = v[2];
        v[2] = t;
    }

    void make_aabb(const int mins[3], const int maxs[3], CVec* min, CVec* max)
    {
        min->x = mins[0];
        min->y = mins[1];
        min->z = mins[2];
        max->x = maxs[0];
        max->y = maxs[1];
        max->z = maxs[2];
    }

    void draw_aabb(const CVec& min, const CVec& max)
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
}

SimpleBezierSurface::SimpleBezierSurface(const DVertex_t* const controls,
        const int steps)
    : m_num_vertices(steps + 1), m_vertices(m_num_vertices * m_num_vertices)
{
    auto v_it = m_vertices.begin();
    DVertex_t temp[3];

    for (int i = 0; i < m_num_vertices; ++i) {
        float a = i / float(steps);
        float b = 1.0f - a;

        float t0 = b * b;
        float t1 = 2.0f * b * a;
        float t2 = a * a;

        DVertex_t* tv = temp;
        for (int j = 0; j < 3; ++j, ++tv) {
            const int row = 3 * j;
            const DVertex_t& v1 = controls[row + 0];
            const DVertex_t& v2 = controls[row + 1];
            const DVertex_t& v3 = controls[row + 2];
            for (int k = 0; k < 2; ++k) {
                tv->tex_coord[k] =
                    t0 * v1.tex_coord[k] +
                    t1 * v2.tex_coord[k] +
                    t2 * v3.tex_coord[k];
                tv->lm_coord[k] =
                    t0 * v1.lm_coord[k] +
                    t1 * v2.lm_coord[k] +
                    t2 * v3.lm_coord[k];
            }
            for (int k = 0; k < 3; ++k) {
                tv->position[k] =
                    t0 * v1.position[k] +
                    t1 * v2.position[k] +
                    t2 * v3.position[k];
            }
            for (int k = 0; k < 4; ++k) {
                tv->color[k] = std::uint8_t(
                    t0 * v1.color[k] +
                    t1 * v2.color[k] +
                    t2 * v3.color[k]);
            }
        }
        for (int j = 0; j < m_num_vertices; ++j, ++v_it) {
            a = j / float(steps);
            b = 1.0f - a;

            t0 = b * b;
            t1 = 2.0f * b * a;
            t2 = a * a;

            for (int k = 0; k < 2; ++k) {
                v_it->tex_coord[k] =
                    t0 * temp[0].tex_coord[k] +
                    t1 * temp[1].tex_coord[k] +
                    t2 * temp[2].tex_coord[k];
                v_it->lm_coord[k] =
                    t0 * temp[0].lm_coord[k] +
                    t1 * temp[1].lm_coord[k] +
                    t2 * temp[2].lm_coord[k];
            }
            for (int k = 0; k < 3; ++k) {
                v_it->position[k] =
                    t0 * temp[0].position[k] +
                    t1 * temp[1].position[k] +
                    t2 * temp[2].position[k];
            }
            for (int k = 0; k < 4; ++k) {
                v_it->color[k] = std::uint8_t(
                    t0 * temp[0].color[k] +
                    t1 * temp[1].color[k] +
                    t2 * temp[2].color[k]);
            }
        }
    }
}

void SimpleBezierSurface::draw() const
{
    auto v1_it = m_vertices.cbegin();
    auto v2_it = m_vertices.cbegin() + m_num_vertices;

    for (int i = 0; i < m_num_vertices - 1; ++i) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j < m_num_vertices; ++j, ++v1_it, ++v2_it) {
            glColor4ubv(v2_it->color);
            glMultiTexCoord2fv(GL_TEXTURE0_ARB, v2_it->tex_coord);
            glMultiTexCoord2fv(GL_TEXTURE1_ARB, v2_it->lm_coord);
            glVertex3fv(v2_it->position);

            glColor4ubv(v1_it->color);
            glMultiTexCoord2fv(GL_TEXTURE0_ARB, v1_it->tex_coord);
            glMultiTexCoord2fv(GL_TEXTURE1_ARB, v1_it->lm_coord);
            glVertex3fv(v1_it->position);
        }
        glEnd();
    }

#if 0
    glActiveTexture(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glColor3f(0.0f, 1.0f, 0.0f);

    auto v_it = m_vertices.cbegin();
    for (int i = 0; i < m_num_vertices; ++i) {
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < m_num_vertices; ++j, ++v_it) {
            glVertex3fv(v_it->position);
        }
        glEnd();
    }

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
#endif
}

GLBezierSurface::GLBezierSurface(const DVertex_t* const vertices,
        const int n_max, const int m_max, const int steps)
{
    m_gl_list_id = glGenLists(1);
    glNewList(m_gl_list_id, GL_COMPILE);
    DVertex_t controls[9];
    for (int i = 0; i < m_max - 2; i += 2) {
        for (int j = 0; j < n_max - 2; j += 2) {
            for (int m = 0; m < 3; ++m) {
                for (int n = 0; n < 3; ++n) {
                    controls[m * 3 + n] = vertices[(i + m) * n_max + (j + n)];
                }
            }
            SimpleBezierSurface(controls, steps).draw();
        }
    }
    glEndList();
}

GLBezierSurface::~GLBezierSurface() noexcept
{
    glDeleteLists(m_gl_list_id, 1);
}

void GLBezierSurface::draw() const
{
    glCallList(m_gl_list_id);
}

void MapBSP46::bsp_read_header(BinaryIO* bio)
{
    bio->read_chars(m_header.magic, sizeof(m_header.magic));
    m_header.version = bio->read_u32le();
}

void MapBSP46::bsp_read_directory(BinaryIO* bio)
{
    m_directory.entities.offset = bio->read_u32le();
    m_directory.entities.length = bio->read_u32le();

    m_directory.textures.offset = bio->read_u32le();
    m_directory.textures.length = bio->read_u32le();

    m_directory.planes.offset = bio->read_u32le();
    m_directory.planes.length = bio->read_u32le();

    m_directory.nodes.offset = bio->read_u32le();
    m_directory.nodes.length = bio->read_u32le();

    m_directory.leaves.offset = bio->read_u32le();
    m_directory.leaves.length = bio->read_u32le();

    m_directory.leaf_faces.offset = bio->read_u32le();
    m_directory.leaf_faces.length = bio->read_u32le();

    m_directory.leaf_brush.offset = bio->read_u32le();
    m_directory.leaf_brush.length = bio->read_u32le();

    m_directory.models.offset = bio->read_u32le();
    m_directory.models.length = bio->read_u32le();

    m_directory.brushes.offset = bio->read_u32le();
    m_directory.brushes.length = bio->read_u32le();

    m_directory.brush_sides.offset = bio->read_u32le();
    m_directory.brush_sides.length = bio->read_u32le();

    m_directory.vertices.offset = bio->read_u32le();
    m_directory.vertices.length = bio->read_u32le();

    m_directory.mesh_verts.offset = bio->read_u32le();
    m_directory.mesh_verts.length = bio->read_u32le();

    m_directory.effects.offset = bio->read_u32le();
    m_directory.effects.length = bio->read_u32le();

    m_directory.faces.offset = bio->read_u32le();
    m_directory.faces.length = bio->read_u32le();

    m_directory.lightmaps.offset = bio->read_u32le();
    m_directory.lightmaps.length = bio->read_u32le();

    m_directory.light_vols.offset = bio->read_u32le();
    m_directory.light_vols.length = bio->read_u32le();

    m_directory.vis_data.offset = bio->read_u32le();
    m_directory.vis_data.length = bio->read_u32le();
}

void MapBSP46::bsp_read_textures(BinaryIO* bio)
{
    bio->seek(m_directory.textures.offset);

    const std::size_t entry_size = 72;
    m_textures = std::vector<DTexture_t>(m_directory.textures.length / entry_size);
    for (auto&& texture : m_textures) {
        bio->read_chars(texture.name, sizeof(texture.name));
        texture.flags = bio->read_u32le();
        texture.contents = bio->read_u32le();
    }
}

void MapBSP46::bsp_read_faces(BinaryIO* bio)
{
    bio->seek(m_directory.faces.offset);

    const std::size_t entry_size = 104;
    m_faces = std::vector<DFace_t>(m_directory.faces.length / entry_size);
    for (auto&& face : m_faces) {
        face.texture = bio->read_u32le();
        face.effect = bio->read_s32le();
        face.type = bio->read_s32le();

        face.vertex = bio->read_u32le();
        face.num_vertices = bio->read_u32le();

        face.mesh_vert = bio->read_u32le();
        face.num_mesh_verts = bio->read_u32le();

        face.lm_index = bio->read_u32le();

        for (int i = 0; i < 2; ++i) {
            face.lm_start[i] = bio->read_s32le();
        }
        for (int i = 0; i < 2; ++i) {
            face.lm_size[i] = bio->read_s32le();
        }
        for (int i = 0; i < 3; ++i) {
            face.lm_origin[i] = bio->read_f32le();
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                face.lm_vecs[j][i] = bio->read_f32le();
            }
        }
        for (int i = 0; i < 3; ++i) {
            face.normal[i] = bio->read_f32le();
        }
        face.n_max = bio->read_s32le();
        face.m_max = bio->read_s32le();
    }
}

void MapBSP46::bsp_read_vertices(BinaryIO* bio)
{
    bio->seek(m_directory.vertices.offset);

    const std::size_t entry_size = 44;
    m_vertices = std::vector<DVertex_t>(m_directory.vertices.length / entry_size);
    for (auto&& vertex : m_vertices) {
        for (int i = 0; i < 3; ++i) {
            vertex.position[i] = bio->read_f32le();
        }
        for (int i = 0; i < 2; ++i) {
            vertex.tex_coord[i] = bio->read_f32le();
        }
        for (int i = 0; i < 2; ++i) {
            vertex.lm_coord[i] = bio->read_f32le();
        }
        for (int i = 0; i < 3; ++i) {
            vertex.normal[i] = bio->read_f32le();
        }
        for (int i = 0; i < 4; ++i) {
            vertex.color[i] = bio->read_u8();
        }

        swizzle(vertex.position);
        swizzle(vertex.normal);
    }
}

void MapBSP46::bsp_read_planes(BinaryIO* bio)
{
    bio->seek(m_directory.planes.offset);

    const std::size_t entry_size = 16;
    m_planes = std::vector<DPlane_t>(m_directory.planes.length / entry_size);
    for (auto&& plane : m_planes) {
        for (int i = 0; i < 3; ++i) {
            plane.normal[i] = bio->read_f32le();
        }
        plane.dist = bio->read_f32le();

        swizzle(plane.normal);
    }
}

void MapBSP46::bsp_read_leaves(BinaryIO* bio)
{
    bio->seek(m_directory.leaves.offset);

    const std::size_t entry_size = 48;
    m_leaves = std::vector<DLeaf_t>(m_directory.leaves.length / entry_size);
    for (auto&& leaf : m_leaves) {
        leaf.cluster = bio->read_s32le();
        leaf.area = bio->read_s32le();

        for (int i = 0; i < 3; ++i) {
            leaf.mins[i] = bio->read_s32le();
        }
        for (int i = 0; i < 3; ++i) {
            leaf.maxs[i] = bio->read_s32le();
        }

        leaf.leaf_face = bio->read_s32le();
        leaf.num_leaf_faces = bio->read_s32le();

        leaf.leaf_brush = bio->read_s32le();
        leaf.num_leaf_brushes = bio->read_s32le();

        swizzle(leaf.mins);
        swizzle(leaf.maxs);
    }
}

void MapBSP46::bsp_read_leaf_faces(BinaryIO* bio)
{
    bio->seek(m_directory.leaf_faces.offset);

    const std::size_t entry_size = 4;
    m_leaf_faces = std::vector<DLeafFace_t>(m_directory.leaf_faces.length / entry_size);
    for (auto&& leaf_face : m_leaf_faces) {
        leaf_face.face = bio->read_s32le();
    }
}

void MapBSP46::bsp_read_nodes(BinaryIO* bio)
{
    bio->seek(m_directory.nodes.offset);

    const std::size_t entry_size = 36;
    m_nodes = std::vector<DNode_t>(m_directory.nodes.length / entry_size);
    for (auto&& node : m_nodes) {
        node.plane = bio->read_s32le();

        node.front = bio->read_s32le();
        node.back = bio->read_s32le();

        for (int i = 0; i < 3; ++i) {
            node.mins[i] = bio->read_s32le();
        }
        for (int i = 0; i < 3; ++i) {
            node.maxs[i] = bio->read_s32le();
        }

        swizzle(node.mins);
        swizzle(node.maxs);
    }
}

void MapBSP46::bsp_read_mesh_verts(BinaryIO* bio)
{
    bio->seek(m_directory.mesh_verts.offset);

    const std::size_t entry_size = 4;
    m_mesh_verts = std::vector<DMeshVert_t>(m_directory.mesh_verts.length / entry_size);
    for (auto&& mesh_vert : m_mesh_verts) {
        mesh_vert.offset = bio->read_s32le();
    }
}

void MapBSP46::bsp_read_lightmaps(BinaryIO* bio)
{
    bio->seek(m_directory.lightmaps.offset);

    const std::size_t entry_size = 128 * 128 * 3;
    m_lightmaps = std::vector<DLightmap_t>(m_directory.lightmaps.length / entry_size);
    for (auto&& lightmap : m_lightmaps) {
        for (int i = 0; i < 128 * 128 * 3; ++i) {
            lightmap.map[i] = bio->read_u8();
        }
    }
}

void MapBSP46::bsp_read_vis_data(BinaryIO* bio)
{
    bio->seek(m_directory.vis_data.offset);

    m_vis_data.num_bitsets = bio->read_s32le();
    m_vis_data.bytes_per_cluster = bio->read_s32le();

    m_vis_bitset = std::vector<std::uint8_t>(m_directory.vis_data.length - 8);
    for (auto&& p : m_vis_bitset) {
        p = bio->read_u8();
    }
}

MapBSP46::MapBSP46(const char* filename, const PAK3Archive& pak)
{
    auto maybe_data = pak.read_file(filename);
    if (!maybe_data) {
        throwf("%s: Couldn't open file from ZIP archive", filename);
    }
    BinaryIO bio(std::move(maybe_data.value()));

    bsp_read_header(&bio);
    if (std::memcmp(m_header.magic, g_ibsp_magic, sizeof(m_header.magic)) != 0) {
        throwf("%s: Unsupported file format", filename);
    }
    if (m_header.version != g_ibsp46_version) {
        std::cout << filename << ": Warning: IBSP version " <<
            m_header.version <<
            " is not supported - graphical corruption may occur" << std::endl;
    }

    bsp_read_directory(&bio);

    bsp_read_faces(&bio);
    bsp_read_vertices(&bio);
    bsp_read_planes(&bio);
    bsp_read_leaves(&bio);
    bsp_read_leaf_faces(&bio);
    bsp_read_nodes(&bio);
    bsp_read_mesh_verts(&bio);

    bsp_read_textures(&bio);
    load_textures(pak);

    bsp_read_lightmaps(&bio);
    process_lightmaps();

    bsp_read_vis_data(&bio);

    std::cout << "Preprocessing bezier patches..." << std::endl;
    for (auto&& face : m_faces) {
        GLBezierSurface* surf = nullptr;
        if (face.type == 2) {
            surf = new GLBezierSurface(m_vertices.data() + face.vertex,
                    face.n_max, face.m_max, 7);
        }
        m_beziers.push_back(surf);
    }

    std::cout << filename << ":" << std::endl;
    std::cout << "  " << m_textures.size() << " textures, " << std::endl;
    std::cout << "  " << m_faces.size() << " faces, " << std::endl;
    std::cout << "  " << m_vertices.size() << " vertices, " << std::endl;
    std::cout << "  " << m_planes.size() << " planes, " << std::endl;
    std::cout << "  " << m_leaves.size() << " leaves, " << std::endl;
    std::cout << "  " << m_leaf_faces.size() << " leaf faces, " << std::endl;
    std::cout << "  " << m_nodes.size() << " nodes, " << std::endl;
    std::cout << "  " << m_mesh_verts.size() << " mesh vertices, " << std::endl;
    std::cout << "  " << m_lightmaps.size() << " lightmaps." << std::endl;
}

MapBSP46::~MapBSP46() noexcept
{
    CTexManager& tex_mgr = CTexManager::Instance();
    for (auto&& texture_id : m_texture_ids) {
        tex_mgr.Free(texture_id);
    }
    for (auto&& lightmap_id : m_lightmap_ids) {
        tex_mgr.Free(lightmap_id);
    }
    for (auto&& p : m_beziers) {
        delete p;
    }
}

void MapBSP46::load_textures(const PAK3Archive& pak)
{
    static const char* const file_extensions[2] = { ".jpg", ".tga" };
    CTexManager& manager = CTexManager::Instance();

    std::cout << "Precaching textures..." << std::endl;
    for (auto&& texture : m_textures) {
        uint32_t texture_id = 0;
        for (int j = 0; j < 2; ++j) {
            std::string filename = texture.name;
            filename += file_extensions[j];
            try {
                CImageTex bsp_texture(filename.c_str(), pak);
                texture_id = manager.Add(bsp_texture);
                break;
            }
            catch (const QException& e) {
            }
        }
        m_texture_ids.push_back(texture_id);
    }
}

void MapBSP46::process_lightmaps()
{
    CTexManager& tex_mgr = CTexManager::Instance();
    for (std::size_t i = 0; i < m_lightmaps.size(); ++i) {
        CLightmapTex lmap(m_lightmaps[i]);
        GLuint texture_id = tex_mgr.Add(lmap);
        m_lightmap_ids.push_back(texture_id);
    }
}

inline bool MapBSP46::is_cluster_visible(const int cur_cluster,
        const int cluster) const
{
    const int n = (cur_cluster * m_vis_data.bytes_per_cluster) + (cluster >> 3);
    return m_vis_bitset[n] & (1 << (cluster & 7));
}

int MapBSP46::find_leaf(const CVec& pos) const
{
    int index = 0;
    while (index >= 0) {
        const DNode_t& node = m_nodes[index];
        const DPlane_t& plane = m_planes[node.plane];
        CVec v(plane.normal);
        const float dist = v.Dot(pos) - plane.dist;
        if (dist >= 0.0f) {
            index = node.front;
        }
        else {
            index = node.back;
        }
    }
    return ~index;
}

void MapBSP46::collect_leaves(index_vector_t* leaves, const int index,
        const CFrustum& frustum) const
{
    if (index < 0) {
        leaves->push_back(~index);
        return;
    }
    const DNode_t& node = m_nodes[index];
    CVec box_min, box_max;
    make_aabb(node.mins, node.maxs, &box_min, &box_max);
    if (!frustum.AaBbVisible(box_min, box_max)) {
        return;
    }
    collect_leaves(leaves, node.front, frustum);
    collect_leaves(leaves, node.back, frustum);
}

void MapBSP46::draw_face(const std::size_t face_index) const
{
    const DFace_t& face = m_faces[face_index];

    glActiveTexture(GL_TEXTURE0_ARB);
    if (face.texture < m_texture_ids.size()) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texture_ids[face.texture]);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    glActiveTexture(GL_TEXTURE1_ARB);
    if (face.lm_index < m_lightmap_ids.size()) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_lightmap_ids[face.lm_index]);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    if (face.type == 1) {
        const DVertex_t* const vertex = m_vertices.data() + face.vertex;
        const int num_vertices = face.num_vertices;
        if (num_vertices == 3) {
            glBegin(GL_TRIANGLES);
        }
        else if (num_vertices == 4) {
            glBegin(GL_QUADS);
        }
        else {
            glBegin(GL_POLYGON);
        }
        for (int j = 0; j < num_vertices; ++j) {
            const DVertex_t& cv = vertex[j];
            glColor4ubv(cv.color);
            glMultiTexCoord2fv(GL_TEXTURE0_ARB, cv.tex_coord);
            glMultiTexCoord2fv(GL_TEXTURE1_ARB, cv.lm_coord);
            glVertex3fv(cv.position);
        }
        glEnd();
    }
    else if (face.type == 2) {
        m_beziers[face_index]->draw();
    }
    else if (face.type == 3) {
        const DVertex_t& vertex = m_vertices[face.vertex];
        glActiveTexture(GL_TEXTURE0_ARB);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(DVertex_t), vertex.color);
        glTexCoordPointer(2, GL_FLOAT, sizeof(DVertex_t), vertex.tex_coord);
        glVertexPointer(3, GL_FLOAT, sizeof(DVertex_t), vertex.position);
        glDrawElements(GL_TRIANGLES, face.num_mesh_verts, GL_UNSIGNED_INT,
                &m_mesh_verts[face.mesh_vert].offset);
    }
}

void MapBSP46::draw_leaves(const index_vector_t& leaves, const CFrustum& frustum) const
{
    CVec box_min, box_max;
    index_vector_t faces;

    for (auto&& leaf_id : leaves) {
        const DLeaf_t& leaf = m_leaves[leaf_id];
        make_aabb(leaf.mins, leaf.maxs, &box_min, &box_max);
        if (!frustum.AaBbVisible(box_min, box_max)) {
            continue;
        }
        const DLeafFace_t* leaf_face = m_leaf_faces.data() + leaf.leaf_face;
        for (int j = 0; j < leaf.num_leaf_faces; ++j) {
            faces.push_back(leaf_face[j].face);
        }
    }

    std::sort(faces.begin(), faces.end());
    auto end = std::unique(faces.begin(), faces.end());
    for (auto face_it = faces.cbegin(); face_it != end; ++face_it) {
        draw_face(*face_it);
    }

#if 0
    glActiveTexture(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    for (auto&& node : m_nodes) {
        make_aabb(node.mins, node.maxs, &box_min, &box_max);
        if (frustum.AaBbVisible(box_min, box_max)) {
            glColor3f(0.0f, 1.0f, 0.0f);
        }
        else {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        draw_aabb(box_min, box_max);
    }

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
#endif
}

void MapBSP46::draw(const CVec& camera_pos, const CFrustum& frustum) const
{
    int leaf_index = find_leaf(camera_pos);
    int cluster = m_leaves[leaf_index].cluster;
    if (cluster < 0) {
        draw(frustum);
        return;
    }

    index_vector_t leaves;
    for (size_t i = 0; i < m_leaves.size(); ++i) {
        if (is_cluster_visible(cluster, m_leaves[i].cluster)) {
            leaves.push_back(i);
        }
    }
    draw_leaves(leaves, frustum);
}

void MapBSP46::draw(const CFrustum& frustum) const
{
    index_vector_t leaves;
    collect_leaves(&leaves, 0, frustum);
    draw_leaves(leaves, frustum);
}
