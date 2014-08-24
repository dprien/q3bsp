#ifndef TEX__H
#define TEX__H

#include <list>
#include <cstdint>

#include "ibsp46.h"
#include "archive.h"
#include "image.h"

class ITexture
{
public:
    virtual ~ITexture() throw ()
    {
    }

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual const uint8_t *GetPixels() const = 0;
};

class CImageTex : public ITexture
{
    Image m_image;

    CImageTex(CImageTex const &);
    void operator = (CImageTex const &);

public:
    explicit CImageTex(const char *, const PAK3Archive &);

    int GetWidth() const;
    int GetHeight() const;

    const uint8_t *GetPixels() const;
};

class CLightmapTex : public ITexture
{
    int     	m_width;
    int     	m_height;

    uint8_t 	*m_pixels;

	static int	sm_overbrightBits;

    CLightmapTex(CLightmapTex const &);
    void operator = (CLightmapTex const &);

public:
    explicit CLightmapTex(const DLightmap_t &);
    ~CLightmapTex() throw ();

    int GetWidth() const;
    int GetHeight() const;

    const uint8_t *GetPixels() const;

	static void SetOverbrightBits(int);
};

class CTexManager
{
    typedef struct STexInfo
    {
        uint32_t id;

        bool operator == (const STexInfo &t)
        {
            if (t.id == id)
                return true;

            return false;
        }
    } TexInfo_t;

    typedef std::list<TexInfo_t> TexList_t;

    TexList_t           m_texList;
    static CTexManager  *sm_instance;

    CTexManager();
    ~CTexManager() throw ();

    CTexManager(CTexManager const &);
    CTexManager &operator = (CTexManager const &);

public:
    uint32_t Add(const ITexture &);
    void Free(uint32_t);

    static CTexManager &Instance();
    static void Destroy();
};

#endif
