#ifndef IBSP__H
#define IBSP__H

#include <cstdint>

const char g_ibspMagic[4] = { 'I', 'B', 'S', 'P' };

#pragma pack (push, 1)

typedef struct
{
    char        magic[4];       // Magic number. Always "IBSP".
    uint32_t    version;        // Version number. 46 for the BSP files distributed with Quake 3.
} DHeader_t;

typedef struct
{
    uint32_t    offset;         // Offset to start of lump, relative to beginning of file.
    uint32_t    length;         // Length of lump. Always a multiple of 4.
} DDirEntry_t;

#pragma pack (pop)

#endif
