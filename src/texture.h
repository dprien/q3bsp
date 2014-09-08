#ifndef TEX__H
#define TEX__H

#include <list>
#include <cstdint>

#include <GL/gl.h>

#include "src/ibsp46.h"
#include "src/image.h"

class PAK3Archive;

class Texture
{
    public:
        virtual ~Texture() noexcept
        {}

        virtual unsigned get_width() const = 0;
        virtual unsigned get_height() const = 0;

        virtual const std::uint8_t* get_pixels() const = 0;
};

class ImageTexture : public Texture
{
    public:
        ImageTexture(const char*, const PAK3Archive&);

        ImageTexture(const ImageTexture&) = delete;
        void operator=(const ImageTexture&) = delete;

        unsigned get_width() const;
        unsigned get_height() const;

        const std::uint8_t* get_pixels() const;

    private:
        Image m_image;
};

class LightmapTexture : public Texture
{
    public:
        explicit LightmapTexture(const DLightmap_t&);

        LightmapTexture(const LightmapTexture&) = delete;
        void operator=(const LightmapTexture&) = delete;

        unsigned get_width() const;
        unsigned get_height() const;

        const std::uint8_t* get_pixels() const;

        static void set_overbright_bits(const unsigned);

    private:
        static unsigned             m_overbright_bits;

        unsigned                    m_width;
        unsigned                    m_height;
        std::vector<std::uint8_t>   m_pixels;
};

class TextureManager
{
    public:
        TextureManager()
        {}

        ~TextureManager() noexcept
        {
            for (auto&& id : m_texture_ids) {
                glDeleteTextures(1, &id);
            }
        }

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        GLuint add(const Texture&);
        void free(const GLuint);

    private:
        std::list<GLuint> m_texture_ids;
};

#endif
