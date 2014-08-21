#ifndef ZIPFILE__H
#define ZIPFILE__H

#include <string>
#include <vector>
#include <list>
#include <cstdint>
#include <experimental/optional>

#include <zip.h>

class ZIPFile
{
    public:
        using data_t = std::vector<uint8_t>;
        using optional_data_t = std::experimental::optional<data_t>;

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
        optional_data_t read_file(const char*) const;
};

// =======================================================================
// =======================================================================

class PAK3Archive
{
    public:
        using data_t = std::vector<uint8_t>;
        using optional_data_t = std::experimental::optional<data_t>;

    private:
        using zip_list_t = std::list<ZIPFile*>;

        const int   m_max_pak_files;
        zip_list_t  m_zip_files;

    public:
        explicit PAK3Archive(const char*, const int = 10);
        ~PAK3Archive() noexcept;

        PAK3Archive(const PAK3Archive&) = delete;
        void operator=(const PAK3Archive&) = delete;

        optional_data_t read_file(const char*) const;
};

#endif
