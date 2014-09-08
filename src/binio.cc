#include <memory>
#include <cstring>
#include <limits>

#include <SDL2/SDL.h>

#include "src/binio.h"
#include "src/exception.h"

BinaryIO::BinaryIO(BinaryIO::octet_vec_t octets)
    : m_octets(std::move(octets))
{
    if (m_octets.size() > std::numeric_limits<int>::max()) {
        throwf("SDL_RWFromMem: `size` parameter out of range");
    }
    m_ops = SDL_RWFromMem(m_octets.data(), static_cast<int>(m_octets.size()));
}

BinaryIO::~BinaryIO() noexcept
{
    SDL_RWclose(m_ops);
}

BinaryIO::offset_t BinaryIO::seek(const BinaryIO::offset_t ofs)
{
    Sint64 new_ofs = SDL_RWseek(m_ops, ofs, RW_SEEK_SET);
    if (new_ofs == -1) {
        throwf("SDL_RWseek: %s", SDL_GetError());
    }
    return static_cast<offset_t>(new_ofs);
}

BinaryIO::offset_t BinaryIO::tell() const
{
    Sint64 ofs = SDL_RWtell(m_ops);
    if (ofs == -1) {
        throwf("SDL_RWtell: %s", SDL_GetError());
    }
    return static_cast<offset_t>(ofs);
}

std::int8_t BinaryIO::read_s8()
{
    std::int8_t data;
    SDL_RWread(m_ops, &data, sizeof(data), 1);
    return data;
}

std::uint8_t BinaryIO::read_u8()
{
    std::uint8_t data;
    SDL_RWread(m_ops, &data, sizeof(data), 1);
    return data;
}

std::int16_t BinaryIO::read_s16le()
{
    return static_cast<std::int16_t>(SDL_ReadLE16(m_ops));
}

std::int16_t BinaryIO::read_s16be()
{
    return static_cast<std::int16_t>(SDL_ReadBE16(m_ops));
}

std::uint16_t BinaryIO::read_u16le()
{
    return SDL_ReadLE16(m_ops);
}

std::uint16_t BinaryIO::read_u16be()
{
    return SDL_ReadBE16(m_ops);
}

std::int32_t BinaryIO::read_s32le()
{
    return static_cast<std::int32_t>(SDL_ReadLE32(m_ops));
}

std::int32_t BinaryIO::read_s32be()
{
    return static_cast<std::int32_t>(SDL_ReadBE32(m_ops));
}

std::uint32_t BinaryIO::read_u32le()
{
    return SDL_ReadLE32(m_ops);
}

std::uint32_t BinaryIO::read_u32be()
{
    return SDL_ReadBE32(m_ops);
}

std::int64_t BinaryIO::read_s64le()
{
    return static_cast<std::int64_t>(SDL_ReadLE64(m_ops));
}

std::int64_t BinaryIO::read_s64be()
{
    return static_cast<std::int64_t>(SDL_ReadBE64(m_ops));
}

std::uint64_t BinaryIO::read_u64le()
{
    return SDL_ReadLE64(m_ops);
}

std::uint64_t BinaryIO::read_u64be()
{
    return SDL_ReadBE64(m_ops);
}

float BinaryIO::read_f32le()
{
    static_assert(sizeof(float) == 4, "sizeof(float) != 4 not supported");
    std::uint8_t buf[4];
    SDL_RWread(m_ops, buf, 1, 4);
    float n;
    std::memcpy(&n, buf, 4);
    return n;
}

float BinaryIO::read_f32be()
{
    static_assert(sizeof(float) == 4, "sizeof(float) != 4 not supported");
    std::uint8_t buf[4];
    SDL_RWread(m_ops, buf, 1, 4);
    float n;
    std::memcpy(&n, buf, 4);
    return n;
}

double BinaryIO::read_f64le()
{
    static_assert(sizeof(double) == 8, "sizeof(double) != 8 not supported");
    std::uint8_t buf[8];
    SDL_RWread(m_ops, buf, 1, 8);
    double n;
    std::memcpy(&n, buf, 8);
    return n;
}

double BinaryIO::read_f64be()
{
    static_assert(sizeof(double) == 8, "sizeof(double) != 8 not supported");
    std::uint8_t buf[8];
    SDL_RWread(m_ops, buf, 1, 8);
    double n;
    std::memcpy(&n, buf, 8);
    return n;
}

void BinaryIO::read_chars(char* const ptr, const std::size_t size)
{
    SDL_RWread(m_ops, ptr, sizeof(char), size);
}

std::string BinaryIO::read_string(const std::size_t size)
{
    auto c_str = std::make_unique<char[]>(size);
    read_chars(c_str.get(), size);
    return std::string(c_str.get(), size);
}
