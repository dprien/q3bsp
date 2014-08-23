#ifndef ZIPFILE__H
#define ZIPFILE__H

#include <string>
#include <vector>
#include <list>
#include <cstdint>
#include <experimental/optional>

#include <zip.h>

class ZIPArchive
{
    public:
        using data_t = std::vector<uint8_t>;
        using optional_data_t = std::experimental::optional<data_t>;

        const std::string archive_filename;

        explicit ZIPArchive(const char*);
        ~ZIPArchive() noexcept;

        ZIPArchive(const ZIPArchive&) = delete;
        void operator=(const ZIPArchive&) = delete;

        bool file_exists(const char*) const;
        optional_data_t read_file(const char*) const;

    private:
        struct zip* m_archive;
};

// =======================================================================
// =======================================================================

class PAK3Archive
{
    public:
        using data_t = ZIPArchive::data_t;
        using optional_data_t = ZIPArchive::optional_data_t;

        explicit PAK3Archive(const char*, const int = 10);

        PAK3Archive(const PAK3Archive&) = delete;
        void operator=(const PAK3Archive&) = delete;

        optional_data_t read_file(const char*) const;

    private:
        std::list<ZIPArchive> m_zip_files;
};

#endif
