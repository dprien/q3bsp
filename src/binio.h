#ifndef Q3BSP__BINIO_H
#define Q3BSP__BINIO_H

#include <vector>
#include <string>
#include <utility>
#include <cstdint>

struct SDL_RWops;

class BinaryIO
{
    public:
        using octet_vec_t = std::vector<std::uint8_t>;
        using offset_t = typename octet_vec_t::size_type;

        BinaryIO(octet_vec_t octets);
        ~BinaryIO() noexcept;

        BinaryIO(const BinaryIO&) = delete;
        void operator=(const BinaryIO&) = delete;

        offset_t seek(const offset_t ofs);
        offset_t tell() const;

        std::int8_t read_s8();
        std::uint8_t read_u8();

        std::int16_t read_s16le();
        std::int16_t read_s16be();

        std::uint16_t read_u16le();
        std::uint16_t read_u16be();

        std::int32_t read_s32le();
        std::int32_t read_s32be();

        std::uint32_t read_u32le();
        std::uint32_t read_u32be();

        std::int64_t read_s64le();
        std::int64_t read_s64be();

        std::uint64_t read_u64le();
        std::uint64_t read_u64be();

        float read_f32le();
        float read_f32be();

        double read_f64le();
        double read_f64be();

        void read_chars(char* const, const std::size_t);
        std::string read_string(const std::size_t);

    private:
        octet_vec_t m_octets;
        SDL_RWops*  m_ops;
};

#endif
