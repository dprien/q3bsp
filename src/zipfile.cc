#include <iostream>
#include <sstream>

#include "zipfile.h"
#include "exception.h"

#include <zip.h>

ZIPFile::ZIPFile(const char* filename)
    : archive_filename(filename)
{
    int r;
    m_archive = zip_open(filename, 0, &r);
    if (m_archive == nullptr) {
        char buf[1024];
        zip_error_to_str(buf, sizeof(buf), r, errno);
        throwf("ZIPFile: zip_open: %s: %s", filename, buf);
    }
}

ZIPFile::~ZIPFile() noexcept
{
    zip_close(m_archive);
}

bool ZIPFile::file_exists(const char* filename) const
{
    return zip_name_locate(m_archive, filename, ZIP_FL_NOCASE) != -1;
}

ZIPFile::optional_data_t ZIPFile::read_file(const char* filename) const
{
    struct zip_stat stat;
    if (zip_stat(m_archive, filename, ZIP_FL_NOCASE, &stat) == -1) {
        throwf("ZIPFile: zip_stat: %s: %s", filename, zip_strerror(m_archive));
    }
    if ((stat.valid & ZIP_STAT_NAME) == 0) {
        throwf("ZIPFile: zip_stat: %s: ZIP_STAT_NAME not set", filename);
    }

    struct zip_file* zf = zip_fopen(m_archive, filename, 0);
    if (zf == nullptr) {
        return optional_data_t();
    }

    data_t v(stat.size);
    zip_int64_t nbytes_read = zip_fread(zf, v.data(), stat.size);
    if (nbytes_read == -1) {
        throwf("ZIPFile: zip_fread: %s: %s", filename, zip_file_strerror(zf));
    }
    if (static_cast<zip_uint64_t>(nbytes_read) != stat.size) {
        throwf("ZIPFile: zip_fread: %s: Read %d bytes, expected %lu bytes",
                filename, nbytes_read, stat.size);
    }

    zip_fclose(zf);
    return v;
}

PAK3Archive::PAK3Archive(const char* path, const int max_pak_files)
    : m_max_pak_files(max_pak_files)
{
    std::string cpath(path);
    if (cpath.back() == '/')
        cpath.pop_back();

    for (int i = 0; i < m_max_pak_files; ++i) {
        std::ostringstream filename;
        filename << cpath << "/pak" << i << ".pk3";
        try {
            ZIPFile* zip = new ZIPFile(filename.str().c_str());
            m_zip_files.push_back(zip);
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

PAK3Archive::~PAK3Archive() noexcept
{
    for (auto&& p : m_zip_files) {
        delete p;
    }
}

PAK3Archive::optional_data_t PAK3Archive::read_file(const char* filename) const
{
    const ZIPFile* best = nullptr;
    for (auto&& p : m_zip_files) {
        if (p->file_exists(filename)) {
            best = p;
        }
    }
    if (best == nullptr) {
        return optional_data_t();
    }

    std::cerr << "PAK3Archive: reading: " << best->archive_filename << ": " <<
        filename << std::endl;
    return best->read_file(filename);
}
