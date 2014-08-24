#ifndef Q3BSP__IMAGE_H
#define Q3BSP__IMAGE_H

#include <vector>
#include <utility>
#include <cstdint>

typedef struct
{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t alpha;
} pixel_t;

using pixel_vector_t = std::vector<pixel_t>;

class Image
{
    public:
        Image()
            : m_width(0), m_height(0), m_pixels()
        {}

        Image(const int width, const int height, pixel_vector_t pixels)
            : m_width(width), m_height(height), m_pixels(std::move(pixels))
        {}

        Image(const int, const int, const void*);

        Image(const Image&) = delete;
        void operator=(const Image&) = delete;

        Image(Image&& other) = default;
        Image& operator=(Image&& other) = default;

        int get_width() const
        {
            return m_width;
        }

        int get_height() const
        {
            return m_height;
        }

        const pixel_vector_t& get_pixels() const
        {
            return m_pixels;
        }

    private:
        int             m_width;
        int             m_height;
        pixel_vector_t  m_pixels;

};

extern Image decode_tga(const std::vector<std::uint8_t>&);
extern Image decode_jpg(const std::vector<std::uint8_t>&);
extern Image decode_by_extension(const std::vector<std::uint8_t>&, std::string);

#endif
