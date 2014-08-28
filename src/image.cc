#include <memory>
#include <map>
#include <cstring>

#include <boost/algorithm/string.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "image.h"
#include "exception.h"

namespace
{
    using surf_uptr_t = std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)>;

    using load_func_t = SDL_Surface* (*)(SDL_RWops*);
    using decode_func_t = Image (*)(const std::vector<std::uint8_t>&);

    std::map<std::string, decode_func_t> extension_decode_map = {
        {".tga", decode_tga},
        {".jpg", decode_jpg}
    };

    Image make_image_from_surface(surf_uptr_t surf)
    {
        SDL_PixelFormat fmt;
        fmt.format = SDL_PIXELFORMAT_RGBA8888;
        fmt.palette = nullptr;
        fmt.BitsPerPixel = 32;
        fmt.BytesPerPixel = 4;
        fmt.Rmask = 0x000000ff;
        fmt.Gmask = 0x0000ff00;
        fmt.Bmask = 0x00ff0000;
        fmt.Amask = 0xff000000;

        surf_uptr_t c(SDL_ConvertSurface(surf.get(), &fmt, 0), SDL_FreeSurface);
        if (!c) {
            throwf("SDL_ConvertSurface: %s", SDL_GetError());
        }
        return Image(c->w, c->h, c->pixels);
    }

    Image decode_with(load_func_t load_func,
            const std::vector<std::uint8_t>& buf, const char* load_func_name)
    {
        SDL_RWops* ops = SDL_RWFromConstMem(buf.data(), buf.size());
        surf_uptr_t surf(load_func(ops), SDL_FreeSurface);
        if (!surf) {
            throwf("%s: %s", load_func_name, IMG_GetError());
        }
        return make_image_from_surface(std::move(surf));
    }
}

#define DECODE_WITH(func_name, buf) decode_with((func_name), (buf), #func_name)

Image decode_tga(const std::vector<std::uint8_t>& buf)
{
    return DECODE_WITH(IMG_LoadTGA_RW, buf);
}

Image decode_jpg(const std::vector<std::uint8_t>& buf)
{
    return DECODE_WITH(IMG_LoadJPG_RW, buf);
}

Image decode_by_extension(const std::vector<std::uint8_t>& buf,
        std::string extension)
{
    boost::algorithm::to_lower(extension);
    decode_func_t func = extension_decode_map.at(extension);
    return func(buf);
}

Image::Image(const int width, const int height, const void* raw_pixels)
    : m_width(width), m_height(height)
{
    typename pixel_vector_t::size_type size = width * height * sizeof(pixel_t);
    m_pixels = pixel_vector_t(size);
    std::memcpy(m_pixels.data(), raw_pixels, size);
}
