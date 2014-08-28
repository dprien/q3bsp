#ifndef IBSP__H
#define IBSP__H

#include <cstdint>

const char g_ibsp_magic[] = { 'I', 'B', 'S', 'P' };

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

#endif
