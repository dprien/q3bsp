// $Id$

#ifndef ZIPFILE__H
#define ZIPFILE__H

#include <string>
#include <list>
#include <map>

#include "types.h"
#include "zlib/contrib/minizip/unzip.h"

class CZipFile
{
    class CFilenameCompare
    {
    public:
        bool operator () (const std::string &s1, const std::string &s2) const
        {
            return unzStringFileNameCompare(s1.c_str(), s2.c_str(), 1) < 0;
        }
    };

    typedef struct
    {
        unz_file_info   info;
        unz_file_pos    pos;
    } InfoRec_t;

    typedef std::map<std::string, InfoRec_t, CFilenameCompare> FileList_t;

    unzFile     m_zipFd;
    FileList_t  m_list;

    CZipFile(const CZipFile &);
    void operator = (const CZipFile &);

public:
    explicit CZipFile(const char *);
    ~CZipFile() throw ();

    char *GetFile(const char *, uint32_t *) const;
    bool FileExists(const char *) const;
};

// =======================================================================
// =======================================================================

class CPk3Archive
{
    typedef std::list<CZipFile *> ZipList_t;
    ZipList_t   m_zips;

    CPk3Archive(const CPk3Archive &);
    void operator = (const CPk3Archive &);

public:
    explicit CPk3Archive(const char *);
    ~CPk3Archive() throw ();

    char *GetFile(const char *, uint32_t *) const;
};

#endif
