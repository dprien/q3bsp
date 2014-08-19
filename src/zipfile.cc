#include <iostream>
#include <sstream>

#include "zipfile.h"
#include "exception.h"

// public:
// ==============================================
CZipFile::CZipFile(const char *filename)
{
    m_zipFd = unzOpen(filename);

    if (m_zipFd == NULL)
        Throwf<CException>("%s: unzOpen: Error opening ZIP file", filename);

    int r = unzGoToFirstFile(m_zipFd);

    while (r == UNZ_OK)
    {
        InfoRec_t fi;
        char zipFilename[257];

        r = unzGetCurrentFileInfo(m_zipFd, &fi.info, zipFilename, 256, 0, 0, 0, 0);

        if (r != UNZ_OK)
        {
            unzClose(m_zipFd);
            Throwf<CException>("%s: unzGetCurrentFileInfo: Failed", filename);
        }

        r = unzGetFilePos(m_zipFd, &fi.pos);

        if (r != UNZ_OK)
        {
            unzClose(m_zipFd);
            Throwf<CException>("%s: unzGetFilePos: Failed", filename);
        }

        m_list[zipFilename] = fi;
        r = unzGoToNextFile(m_zipFd);
    }
}

CZipFile::~CZipFile() throw ()
{
    unzClose(m_zipFd);
}

char *CZipFile::GetFile(const char *filename, uint32_t *len) const
{
    FileList_t::const_iterator p = m_list.find(filename);

    if (p == m_list.end())
        return 0;

    unz_file_pos pos = p->second.pos;

    if (unzGoToFilePos(m_zipFd, &pos) != UNZ_OK)
        Throwf<CException>("%s: unzGoToFilePos: Failed", filename);

    if (unzOpenCurrentFile(m_zipFd) != UNZ_OK)
        Throwf<CException>("%s: unzOpenCurrentFile: Failed", filename);

    const unz_file_info &info = p->second.info;

    char *buf = new char[info.uncompressed_size];
    int r = unzReadCurrentFile(m_zipFd, buf, info.uncompressed_size);

    if (r < 0)
    {
        delete [] buf;
        unzCloseCurrentFile(m_zipFd);

        Throwf<CException>("%s: unzReadCurrentFile: Failed", filename);
    }

    uint32_t rsize = uint32_t(r);

    if (rsize != info.uncompressed_size)
    {
        delete [] buf;
        unzCloseCurrentFile(m_zipFd);

        Throwf<CException>("%s: unzReadCurrentFile: Read %lu byte, expected %lu byte",
            filename, rsize, info.uncompressed_size);
    }

    if (unzCloseCurrentFile(m_zipFd) != UNZ_OK)
    {
        delete [] buf;
        Throwf<CException>("%s: unzCloseCurrentFile: Checksum error in ZIP", filename);
    }

    if (len)
        *len = rsize;

    return buf;
}

bool CZipFile::FileExists(const char *filename) const
{
    FileList_t::const_iterator p = m_list.find(filename);

    if (p == m_list.end())
        return false;

    return true;
}

// =======================================================================
// =======================================================================

// public:
// ==============================================
CPk3Archive::CPk3Archive(const char *cpath)
{
    std::string path = cpath;

    for (std::string::iterator p = path.begin(); p != path.end(); ++p)
        if (*p == '\\')
            *p = '/';

    if (path[path.length() - 1] != '/')
        path += '/';

    for (int i = 0; i < 10; ++i)
    {
        std::ostringstream filename;
        filename << path << "pak" << i << ".pk3";

        try
        {
            CZipFile *zip = new CZipFile(filename.str().c_str());
            m_zips.push_back(zip);
        }
        catch (const CException &)
        {
            if (i == 0)
                throw;

            break;
        }

        std::cout << "Using " << filename.str() << std::endl;
    }
}

CPk3Archive::~CPk3Archive() throw ()
{
    for (ZipList_t::const_iterator p = m_zips.begin(); p != m_zips.end(); ++p)
        delete *p;
}

char *CPk3Archive::GetFile(const char *filename, uint32_t *len) const
{
    ZipList_t::const_iterator p = m_zips.begin(), best = m_zips.end();

    for ( ; p != m_zips.end(); ++p)
        if ((*p)->FileExists(filename))
            best = p;

    if (best == m_zips.end())
        return 0;

    return (*best)->GetFile(filename, len);
}
