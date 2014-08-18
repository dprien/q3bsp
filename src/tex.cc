// $Id$

#include <GL/gl.h>
#include <GL/glu.h>

#include "tex.h"
#include "exception.h"

// public:
// =================================================
CImageTex::CImageTex(const char *filename, const CPk3Archive &pak)
    : m_pixels(0)
{
    uint32_t len;
    char *data = pak.GetFile(filename, &len);

    if (!data)
        Throwf<CException>("%s: Couldn't open file from ZIP archive", filename);

    try
    {
        Magick::Blob blob(data, len);
        Magick::Image img(blob);

        img.type(Magick::TrueColorType);

        m_width  = img.columns();
        m_height = img.rows();

        const Magick::PixelPacket *pixels = img.getConstPixels(0, 0, m_width, m_height);
        m_pixels = new uint8_t[m_width * m_height * 4];

        const Magick::PixelPacket *p = pixels;
        uint8_t *q = m_pixels;

        for (int j = 0; j < m_height; ++j)
        {
            for (int i = 0; i < m_width; ++i, ++p, q += 4)
            {
                q[0] = p->red;
                q[1] = p->green;
                q[2] = p->blue;
                q[3] = p->opacity;
            }
        }
    }
    catch (const Magick::Exception &e)
    {
        if (m_pixels)
            delete [] m_pixels;

        Throwf<CException>("%s: ImageMagick: %s", filename, e.what());
    }
}

CImageTex::~CImageTex() throw ()
{
    delete [] m_pixels;
}

int CImageTex::GetWidth() const
{
    return m_width;
}

int CImageTex::GetHeight() const
{
    return m_height;
}

const uint8_t *CImageTex::GetPixels() const
{
    return m_pixels;
}

// =======================================================================
// =======================================================================

// private:
// =================================================
int CLightmapTex::sm_overbrightBits = 1;

// public:
// =================================================
CLightmapTex::CLightmapTex(const DLightmap_t &lightmap)
{
    m_width  = 128;
    m_height = 128;

    m_pixels = new uint8_t[m_width * m_height * 4];

    const uint8_t *p = lightmap.map;
    uint8_t *q = m_pixels;

    int r, g, b, cmax;

    for (int i = 0; i < m_height; ++i)
    {
        for (int j = 0; j < m_width; ++j, p += 3, q += 4)
        {
            r = p[0] << sm_overbrightBits;
            g = p[1] << sm_overbrightBits;
            b = p[2] << sm_overbrightBits;

            cmax = std::max(std::max(r, g), b);

            if (cmax > 255)
            {
                cmax = (255 << 8) / cmax;

                r = (r * cmax) >> 8;
                g = (g * cmax) >> 8;
                b = (b * cmax) >> 8;
            }

            q[0] = r;
            q[1] = g;
            q[2] = b;
            q[3] = 255;
        }
    }
}

CLightmapTex::~CLightmapTex() throw ()
{
    delete [] m_pixels;
}

int CLightmapTex::GetWidth() const
{
    return m_width;
}

int CLightmapTex::GetHeight() const
{
    return m_height;
}

const uint8_t *CLightmapTex::GetPixels() const
{
    return m_pixels;
}

void CLightmapTex::SetOverbrightBits(int overbrightBits)
{
	sm_overbrightBits = overbrightBits;
}

// =======================================================================
// =======================================================================

// private:
// =================================================
CTexManager *CTexManager::sm_instance = 0;

CTexManager::CTexManager()
{
}

CTexManager::~CTexManager() throw ()
{
    for (TexList_t::const_iterator p = m_texList.begin(); p != m_texList.end(); ++p)
        glDeleteTextures(1, &p->id);
}

// public:
// =================================================
uint32_t CTexManager::Add(const ITexture &tex)
{
    TexInfo_t texture;

    glGenTextures(1, &texture.id);
    m_texList.push_back(texture);

    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gluBuild2DMipmaps(
        GL_TEXTURE_2D,
        GL_RGBA,
        tex.GetWidth(),
        tex.GetHeight(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex.GetPixels());

    return texture.id;
}

void CTexManager::Free(uint32_t texId)
{
    TexInfo_t texinfo;
    texinfo.id = texId;

    m_texList.remove(texinfo);
    glDeleteTextures(1, &texinfo.id);
}

CTexManager &CTexManager::Instance()
{
    if (sm_instance == 0)
        sm_instance = new CTexManager();

    return *sm_instance;
}

void CTexManager::Destroy()
{
    if (sm_instance)
        delete sm_instance;

    sm_instance = 0;
}
