#pragma once
#include <vector>
#include <cstdint>

using byte = uint8_t;

// little-endian "VBSP"   0x50534256
const unsigned IDBSPHEADER = 0x50534256;

const unsigned HEADER_LUMPS = 64;
const unsigned MAX_MAP_FACES = 65536;
const unsigned MAX_MAP_TEXDATA_STRING_DATA = 256000;
const unsigned TEXTURE_NAME_LENGTH = 128;
const unsigned MAX_LUMP_STRINGDATA_SIZE = MAX_MAP_TEXDATA_STRING_DATA * TEXTURE_NAME_LENGTH;

const unsigned LUMP_TEXDATA = 2;
const unsigned LUMP_VERTEXES = 3;
const unsigned LUMP_NODES = 5;
const unsigned LUMP_TEXINFO = 6;
const unsigned LUMP_FACES = 7;
const unsigned LUMP_LEAFS = 10;
const unsigned LUMP_EDGES = 12;
const unsigned LUMP_SURFEDGES = 13;
const unsigned LUMP_DISPINFO = 26;
const unsigned LUMP_DISPVERTS = 33;
const unsigned LUMP_STRINGDATA = 43;
const unsigned LUMP_STRINGTABLE = 44;

#define MAX_DISP_CORNER_NEIGHBORS	4

#define MAX_DISPVERTS 289
#define MAX_DISPTRIS 512

struct lump_t
{
	int fileofs;	// offset into file (bytes)
	int filelen;	// length of lump (bytes)
	int version;	// lump format version
	char fourCC[4]; // lump ident code
};

struct dheader_t
{
	int ident;					// BSP file identifier
	int version;				// BSP file version
	lump_t lumps[HEADER_LUMPS]; // lump directory array
	int mapRevision;			// the map's revision (iteration, version) number
};

struct Vector
{
	float x;
	float y;
	float z;
};

struct dplane_t
{
	Vector normal; // normal vector
	float dist;	   // distance from origin
	int type;	   // plane axis identifier
};

struct dedge_t
{
	unsigned short v[2]; // vertex indices
};

struct dface_t
{
	unsigned short planenum;			// the plane number
	byte side;							// faces opposite to the node's plane direction
	byte onNode;						// 1 of on node, 0 if in leaf
	int firstedge;						// index into surfedges
	short numedges;						// number of surfedges
	short texinfo;						// texture info
	short dispinfo;						// displacement info
	short surfaceFogVolumeID;			// ?
	byte styles[4];						// switchable lighting info
	int lightofs;						// offset into lightmap lump
	float area;							// face area in units^2
	int LightmapTextureMinsInLuxels[2]; // texture lighting info
	int LightmapTextureSizeInLuxels[2]; // texture lighting info
	int origFace;						// original face this was split from
	unsigned short numPrims;			// primitives
	unsigned short firstPrimID;
	unsigned int smoothingGroups; // lightmap smoothing group
};

struct dnode_t
{
	int planenum;	 // index into plane array
	int children[2]; // negative numbers are -(leafs + 1), not nodes
	short mins[3];	 // for frustum culling
	short maxs[3];
	unsigned short firstface; // index into face array
	unsigned short numfaces;  // counting both sides
	short area;				  // If all leaves below this node are in the same area, then
							  // this is the area index. If not, this is -1.
	short paddding;			  // pad to 32 bytes length
};

struct CompressedLightCube
{
	char padding[24];
};
struct dleaf_t
{
	int contents;	 // OR of all brushes (not needed?)
	short cluster;	 // cluster this leaf is in
	short area : 9;	 // area this leaf is in
	short flags : 7; // flags
	short mins[3];	 // for frustum culling
	short maxs[3];
	unsigned short firstleafface; // index into leaffaces
	unsigned short numleaffaces;
	unsigned short firstleafbrush; // index into leafbrushes
	unsigned short numleafbrushes;
	short leafWaterDataID; // -1 for not in water

	//!!! NOTE: for maps of version 19 or lower uncomment this block
	//!!! Comment it for hl2 maps onwards
	// CompressedLightCube   ambientLighting;      // Precaculated light info for entities.
	// short                 padding;              // padding to 4-byte boundary
};

struct texinfo_t
{
	float textureVecs[2][4];  // [s/t][xyz offset]
	float lightmapVecs[2][4]; // [s/t][xyz offset] - length is in units of texels/area
	int flags;				  // miptex flags overrides
	int texdata;			  // Pointer to texture name, size, etc.
};

struct dtexdata_t
{
	Vector reflectivity;   // RGB reflectivity
	int nameStringTableID; // index into TexdataStringTable
	int width, height;	   // source image
	int view_width, view_height;
};

struct dmodel_t
{
	Vector mins, maxs;		 // bounding box
	Vector origin;			 // for sounds or lights
	int headnode;			 // index into node array
	int firstface, numfaces; // index into face array
};

struct dvis_t
{
	int numclusters;
	std::vector<int> byteofs0;
	std::vector<int> byteofs1;
};

struct TexdataStringTable
{
	int index;
};

struct TexdataStringData
{
	char data[TEXTURE_NAME_LENGTH];
};




// Corner indices. Used to index m_CornerNeighbors.
enum
{
	CORNER_LOWER_LEFT = 0,
	CORNER_UPPER_LEFT = 1,
	CORNER_UPPER_RIGHT = 2,
	CORNER_LOWER_RIGHT = 3
};

// These edge indices must match the edge indices of the CCoreDispSurface.
enum
{
	NEIGHBOREDGE_LEFT = 0,
	NEIGHBOREDGE_TOP = 1,
	NEIGHBOREDGE_RIGHT = 2,
	NEIGHBOREDGE_BOTTOM = 3
};


// These denote where one dispinfo fits on another.
// Note: tables are generated based on these indices so make sure to update
//       them if these indices are changed.
typedef enum
{
	CORNER_TO_CORNER = 0,
	CORNER_TO_MIDPOINT = 1,
	MIDPOINT_TO_CORNER = 2
} NeighborSpan;


// These define relative orientations of displacement neighbors.
typedef enum
{
	ORIENTATION_CCW_0 = 0,
	ORIENTATION_CCW_90 = 1,
	ORIENTATION_CCW_180 = 2,
	ORIENTATION_CCW_270 = 3
} NeighborOrientation;

struct CDispSubNeighbor
{
	unsigned short		m_iNeighbor;		// This indexes into ddispinfos.
	// 0xFFFF if there is no neighbor here.

	unsigned char		m_NeighborOrientation;		// (CCW) rotation of the neighbor wrt this displacement.

	// These use the NeighborSpan type.
	unsigned char		m_Span;						// Where the neighbor fits onto this side of our displacement.
	unsigned char		m_NeighborSpan;				// Where we fit onto our neighbor.
};
struct CDispNeighbor
{
	CDispSubNeighbor	m_SubNeighbors[2];
};

struct CDispCornerNeighbors
{

	unsigned short	m_Neighbors[MAX_DISP_CORNER_NEIGHBORS];	// indices of neighbors.
	unsigned char	m_nNeighbors;
};
struct ddispinfo_t
{
	glm::vec3 startPosition;					 // start position used for orientation
	int DispVertStart;						 // Index into LUMP_DISP_VERTS.
	int DispTriStart;						 // Index into LUMP_DISP_TRIS.
	int power;								 // power - indicates size of surface (2^power 1)
	int minTess;							 // minimum tesselation allowed
	float smoothingAngle;					 // lighting smoothing angle
	int contents;							 // surface contents
	unsigned short MapFace;					 // Which map face this displacement comes from.
	int LightmapAlphaStart;					 // Index into ddisplightmapalpha.
	int LightmapSamplePositionStart;		 // Index into LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS.
	CDispNeighbor EdgeNeighbors[4];			 // Indexed by NEIGHBOREDGE_ defines.
	CDispCornerNeighbors CornerNeighbors[4]; // Indexed by CORNER_ defines.
	unsigned int AllowedVerts[10];			 // active verticies
};

struct dDispVert
{
	glm::vec3 vec;	 // Vector field defining displacement volume.
	float dist;	 // Displacement distances.
	float alpha; // "per vertex" alpha values.
};

enum dprimitive_type
{
	PRIM_TRILIST = 0,
	PRIM_TRISTRIP = 1,
};

struct dprimitive_t
{
	unsigned char type;
	unsigned short firstIndex;
	unsigned short indexCount;
	unsigned short firstVert;
	unsigned short vertCount;
};