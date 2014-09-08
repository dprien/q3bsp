#include <boost/filesystem.hpp>

#include <GL/gl.h>
#include <GL/glu.h>

#include "src/texture.h"
#include "src/archive.h"
#include "src/exception.h"

ImageTexture::ImageTexture(const char* filename, const PAK3Archive& pak)
{
    auto maybe_data = pak.read_file(filename);
    if (!maybe_data) {
        throwf("%s: Couldn't open file from ZIP archive", filename);
    }

    boost::filesystem::path path(filename);
    m_image = decode_by_extension(maybe_data.value(), path.extension().string());
}

unsigned ImageTexture::get_width() const
{
    return m_image.get_width();
}

unsigned ImageTexture::get_height() const
{
    return m_image.get_height();
}

const uint8_t* ImageTexture::get_pixels() const
{
    return reinterpret_cast<const uint8_t*>(m_image.get_pixels().data());
}

unsigned LightmapTexture::m_overbright_bits = 1;

LightmapTexture::LightmapTexture(const DLightmap_t& lightmap)
    : m_width(128), m_height(128), m_pixels(m_width * m_height * 4)
{
    unsigned r, g, b, cmax;
    const uint8_t* p = lightmap.map;
    auto q = m_pixels.begin();

    for (unsigned i = 0; i < m_height; ++i) {
        for (unsigned j = 0; j < m_width; ++j, p += 3, q += 4) {
            r = static_cast<unsigned>(p[0]) << m_overbright_bits;
            g = static_cast<unsigned>(p[1]) << m_overbright_bits;
            b = static_cast<unsigned>(p[2]) << m_overbright_bits;
            cmax = std::max(std::max(r, g), b);
            if (cmax > 255) {
                cmax = (255 << 8) / cmax;
                r = (r * cmax) >> 8;
                g = (g * cmax) >> 8;
                b = (b * cmax) >> 8;
            }
            q[0] = static_cast<std::uint8_t>(r);
            q[1] = static_cast<std::uint8_t>(g);
            q[2] = static_cast<std::uint8_t>(b);
            q[3] = 255;
        }
    }
}

unsigned LightmapTexture::get_width() const
{
    return m_width;
}

unsigned LightmapTexture::get_height() const
{
    return m_height;
}

const uint8_t* LightmapTexture::get_pixels() const
{
    return m_pixels.data();
}

void LightmapTexture::set_overbright_bits(const unsigned overbright_bits)
{
    m_overbright_bits = overbright_bits;
}

GLuint TextureManager::add(const Texture& tex)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    m_texture_ids.push_back(texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gluBuild2DMipmaps(
        GL_TEXTURE_2D,
        GL_RGBA,
        static_cast<GLsizei>(tex.get_width()),
        static_cast<GLsizei>(tex.get_height()),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex.get_pixels());

    return texture_id;
}

void TextureManager::free(const GLuint texture_id)
{
    m_texture_ids.remove(texture_id);
    glDeleteTextures(1, &texture_id);
}
