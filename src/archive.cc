#include <iostream>
#include <sstream>

#include <zip.h>

#include "exception.h"
#include "archive.h"

namespace
{
    class ZIPFile
    {
        public:
            ZIPFile(struct zip* z, const char* filename, zip_flags_t flags)
                : handle(zip_fopen(z, filename, flags))
            {}

            ~ZIPFile() noexcept
            {
                if (handle != nullptr) {
                    zip_fclose(handle);
                }
            }

            ZIPFile(const ZIPFile&) = delete;
            void operator=(const ZIPFile&) = delete;

        public:
            struct zip_file* const handle;
    };
}

ZIPArchive::ZIPArchive(const char* filename)
    : archive_filename(filename)
{
    int r;
    m_archive = zip_open(filename, 0, &r);
    if (m_archive == nullptr) {
        char buf[1024];
        zip_error_to_str(buf, sizeof(buf), r, errno);
        throwf("ZIPArchive: zip_open: %s: %s", filename, buf);
    }
}

ZIPArchive::~ZIPArchive() noexcept
{
    zip_close(m_archive);
}

bool ZIPArchive::file_exists(const char* filename) const
{
    return zip_name_locate(m_archive, filename, ZIP_FL_NOCASE) != -1;
}

ZIPArchive::optional_data_t ZIPArchive::read_file(const char* filename) const
{
    struct zip_stat stat;
    if (zip_stat(m_archive, filename, ZIP_FL_NOCASE, &stat) == -1) {
        throwf("ZIPArchive: zip_stat: %s: %s", filename, zip_strerror(m_archive));
    }
    if ((stat.valid & ZIP_STAT_NAME) == 0) {
        throwf("ZIPArchive: zip_stat: %s: ZIP_STAT_NAME not set", filename);
    }

    ZIPFile zf(m_archive, filename, 0);
    if (zf.handle == nullptr) {
        return optional_data_t();
    }

    data_t v(stat.size);
    zip_int64_t nbytes_read = zip_fread(zf.handle, v.data(), stat.size);
    if (nbytes_read == -1) {
        throwf("ZIPArchive: zip_fread: %s: %s", filename,
                zip_file_strerror(zf.handle));
    }
    if (static_cast<zip_uint64_t>(nbytes_read) != stat.size) {
        throwf("ZIPArchive: zip_fread: %s: Read %d bytes, expected %lu bytes",
                filename, nbytes_read, stat.size);
    }

    return v;
}

PAK3Archive::PAK3Archive(const char* path, const int max_pak_files)
{
    std::string cpath(path);
    if (cpath.back() == '/')
        cpath.pop_back();

    for (int i = 0; i < max_pak_files; ++i) {
        std::ostringstream filename;
        filename << cpath << "/pak" << i << ".pk3";
        try {
            m_zip_files.emplace_back(filename.str().c_str());
            std::cerr << "PAK3Archive: Using " << filename.str() << std::endl;
        }
        catch (const QException&) {
            break;
        }
    }
    if (m_zip_files.empty()) {
        throwf("PAK3Archive: No PK3 files could be read");
    }
}

PAK3Archive::optional_data_t PAK3Archive::read_file(const char* filename) const
{
    const ZIPArchive* best = nullptr;
    for (auto&& p : m_zip_files) {
        if (p.file_exists(filename)) {
            best = &p;
        }
    }
    if (best == nullptr) {
        return optional_data_t();
    }

    std::cerr << "PAK3Archive: reading: " << best->archive_filename << ": " <<
        filename << std::endl;
    return best->read_file(filename);
}
