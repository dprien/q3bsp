#ifndef Q3BSP__IMAGE_H
#define Q3BSP__IMAGE_H

#include <vector>
#include <utility>
#include <cstdint>

using pixel_t = struct SPixel
{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t alpha;
};

using pixel_vector_t = std::vector<pixel_t>;

class Image
{
    public:
        Image()
            : m_width(0), m_height(0), m_pixels()
        {}

        Image(const unsigned width, const unsigned height, pixel_vector_t pixels)
            : m_width(width), m_height(height), m_pixels(std::move(pixels))
        {}

        Image(const unsigned, const unsigned, const void*);

        Image(const Image&) = delete;
        void operator=(const Image&) = delete;

        Image(Image&& other) = default;
        Image& operator=(Image&& other) = default;

        unsigned get_width() const
        {
            return m_width;
        }

        unsigned get_height() const
        {
            return m_height;
        }

        const pixel_vector_t& get_pixels() const
        {
            return m_pixels;
        }

    private:
        unsigned        m_width;
        unsigned        m_height;
        pixel_vector_t  m_pixels;

};

extern Image decode_tga(const std::vector<std::uint8_t>&);
extern Image decode_jpg(const std::vector<std::uint8_t>&);
extern Image decode_by_extension(const std::vector<std::uint8_t>&, std::string);

#endif
