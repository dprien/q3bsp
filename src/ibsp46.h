#ifndef IBSP46__H
#define IBSP46__H

#include "types.h"
#include "ibsp.h"

const uint32_t g_ibsp46Version = 46;

#pragma pack (push, 1)

// Directory
// =================================================
typedef struct
{
    DDirEntry_t     entities;   // Game-related object descriptions.
    DDirEntry_t     textures;   // Surface descriptions.
    DDirEntry_t     planes;     // Planes used by map geometry.
    DDirEntry_t     nodes;      // BSP tree nodes.
    DDirEntry_t     leaves;     // BSP tree leaves.
    DDirEntry_t     leafFaces;  // Lists of face indices, one list per leaf.
    DDirEntry_t     leafBrushes;// Lists of brush indices, one list per leaf.
    DDirEntry_t     models;     // Descriptions of rigid world geometry in map.
    DDirEntry_t     brushes;    // Convex polyhedra used to describe solid space.
    DDirEntry_t     brushSides; // Brush surfaces.
    DDirEntry_t     vertices;   // Vertices used to describe faces.
    DDirEntry_t     meshVerts;  // Lists of offsets, one list per mesh.
    DDirEntry_t     effects;    // List of special map effects.
    DDirEntry_t     faces;      // Surface geometry.
    DDirEntry_t     lightmaps;  // Packed lightmap data.
    DDirEntry_t     lightVols;  // Local illumination data.
    DDirEntry_t     visData;    // Cluster-cluster visibility data.
} DDir_t;

// Lump "entities"
// =================================================
typedef struct
{
    char        ents[1];        // Entity descriptions, stored as a string.
                                // The length of the entity string is given by
                                //   the size of the lump itself, as specified
                                //   in the lump directory.
} DEntities_t;

// Lump "textures"
// =================================================
typedef struct
{
    char        name[64];       // Texture name.
    uint32_t    flags;          // Surface flags.
    uint32_t    contents;       // Content flags.
} DTexture_t;

enum eDTextureContents
{
    SOLID = 1,
    LAVA = 8,
    SLIME = 16,
    WATER = 32,
    FOG = 64
};

// Lump "planes"
// =================================================
typedef struct
{
    float       normal[3];      // Plane normal.
    float       dist;           // Distance from origin to plane along normal.
} DPlane_t;

// Lump "nodes"
// =================================================
typedef struct
{
    int32_t     plane;          // Plane index.

    int32_t     front;          // Children indices. Negative numbers are leaf indices: -(leaf + 1).
    int32_t     back;           // Children indices. Negative numbers are leaf indices: -(leaf + 1).

    int32_t     mins[3];        // Integer bounding box min coord.
    int32_t     maxs[3];        // Integer bounding box max coord.
} DNode_t;

// Lump "leaves"
// =================================================
typedef struct
{
    int32_t     cluster;        // Visdata cluster index.
    int32_t     area;           // Areaportal area.

    int32_t     mins[3];        // Integer bounding box min coord.
    int32_t     maxs[3];        // Integer bounding box max coord.

    int32_t     leafFace;       // First leafface for leaf.
    int32_t     nLeafFaces;     // Number of leaffaces for leaf.

    int32_t     leafBrush;      // First leafbrush for leaf.
    int32_t     nLeafBrushes;   // Number of leafbrushes for leaf.
} DLeaf_t;

// Lump "leafFaces"
// =================================================
typedef struct
{
    int32_t     face;           // Face index.
} DLeafFace_t;

// Lump "leafBrushes"
// =================================================
typedef struct
{
    int32_t     brush;          // Brush index.
} DLeafBrush_t;

// Lump "models"
// =================================================
typedef struct
{
    float       mins[3];        // Bounding box min coord.
    float       maxs[3];        // Bounding box max coord.

    int32_t     face;           // First face for model.
    int32_t     nFaces;         // Number of faces for model.

    int32_t     brush;          // First brush for model.
    int32_t     nBrushes;       // Number of brushes for model.
} DModel_t;

// Lump "brushes"
// =================================================
typedef struct
{
    int32_t     brushSide;      // First brushside for brush.
    int32_t     nBrushSides;    // Number of brushsides for brush.

    int32_t     texture;        // Texture index.
} DBrush_t;

// Lump "brushSides"
// =================================================
typedef struct
{
    int32_t     plane;          // Plane index.
    int32_t     texture;        // Texture index.
} DBrushSide_t;

// Lump "vertices"
// =================================================
typedef struct
{
    float       position[3];    // Vertex position.
    float       texCoord[2];    // Vertex texture coordinates.
    float       lmCoord[2];     // Lightmap texture coordinates.
    float       normal[3];      // Vertex normal.
    uint8_t     color[4];       // Vertex color. RGBA.
} DVertex_t;

// Lump "meshVerts"
// =================================================
typedef struct
{
    int32_t     offset;         // Vertex index offset, relative to first vertex of corresponding face.
} DMeshVert_t;

// Lump "effects"
// =================================================
typedef struct
{
    char        name[64];       // Effect shader.
    int32_t     brush;          // Brush that generated this effect.
    int32_t     unknown;        // Always 5, except in q3dm8, which has one effect with -1.
} DEffect_t;

// Lump "faces"
// =================================================
typedef struct
{
    uint32_t    texture;        // Texture index.
    int32_t     effect;         // Index into lump 12 (Effects), or -1.
    int32_t     type;           // Face type. 1 = polygon, 2 = patch, 3 = mesh, 4 = billboard

    uint32_t    vertex;         // Index of first vertex.
    uint32_t    nVertices;      // Number of vertices.

    uint32_t    meshVert;       // Index of first meshvert.
    uint32_t    nMeshVerts;     // Number of meshverts.

    uint32_t    lmIndex;        // Lightmap index.
    int32_t     lmStart[2];     // Corner of this face's lightmap image in lightmap.
    int32_t     lmSize[2];      // Size of this face's lightmap image in lightmap.

    float       lmOrigin[3];    // World space origin of lightmap.
    float       lmVecs[2][3];   // World space lightmap s and t unit vectors.

    float       normal[3];      // Surface normal.

    int32_t     nMax;           // Patch dimensions n.
    int32_t     mMax;           // Patch dimensions m.
} DFace_t;

// Lump "lightmaps"
// =================================================
typedef struct
{
    uint8_t     map[128 * 128 * 3]; // Lightmap color data. RGB.
} DLightmap_t;

// Lump "lightVols"
// =================================================
typedef struct
{
    uint8_t     ambient[3];     // Ambient color component. RGB.
    uint8_t     directional[3]; // Directional color component. RGB.
    uint8_t     dir[2];         // Direction to light. 0 = phi, 1 = theta.
} DLightVol_t;

// Lump "visData"
// =================================================
typedef struct
{
    int32_t     nBitsets;       // Number of vectors.
    int32_t     bytesPerCluster;// Size of each vector, in bytes.

    uint8_t     bitset[1];      // Visibility data. One bit per cluster per vector.
} DVisData_t;

#pragma pack (pop)

#endif
