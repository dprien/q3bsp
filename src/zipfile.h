#ifndef ZIPFILE__H
#define ZIPFILE__H

#include <string>
#include <list>
#include <memory>
#include <cstdint>

#include <zip.h>

class ZIPFile
{
    private:
        struct zip* m_archive;

    public:
        const std::string archive_filename;

    public:
        explicit ZIPFile(const char*);
        ~ZIPFile() noexcept;

        ZIPFile(const ZIPFile&) = delete;
        void operator=(const ZIPFile&) = delete;

        bool file_exists(const char*) const;
        std::unique_ptr<std::uint8_t[]> read_file(const char*,
                std::uint64_t* = nullptr) const;
};

// =======================================================================
// =======================================================================

class PAK3Archive
{
    private:
        typedef std::list<ZIPFile*> zip_list_t;

        const int   m_max_pak_files;
        zip_list_t  m_zip_files;

    public:
        explicit PAK3Archive(const char*, const int = 10);
        ~PAK3Archive() noexcept;

        PAK3Archive(const PAK3Archive&) = delete;
        void operator=(const PAK3Archive&) = delete;

        std::unique_ptr<std::uint8_t[]> read_file(const char*,
                std::uint64_t* = nullptr) const;
};

#endif
