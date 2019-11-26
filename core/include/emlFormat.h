#pragma once
#include <string>
// Format identifier must always be 0x53454D4C || SEML
#define EMLHEADER (('L'<<24)+('M'<<16)+('E'<<8)+'S')

const int maxLumps = 3;
const float emlVersion = 1.0f;
enum emlLumps
{
	lVertices = 0,
	lIndices,
	lMeshes

};

struct t_eml_header
{
	int format;
	float version;
	int numLumps;
	int numOfMeshes;
};

struct t_Lumps
{
	int offset;
	int size;
};

struct t_Position
{
	float x;
	float y;
	float z;
};

struct t_Normal
{
	float x;
	float y;
	float z;
};

struct t_Tangent
{
	float x;
	float y;
	float z;
};

struct t_Bitangent
{
	float x;
	float y;
	float z;
};

struct t_Index
{
	unsigned int index;
};

struct t_TexCoord
{
	float s;
	float t;
};

struct t_Vertex
{
	t_Position position;
	t_TexCoord texcoord;
	t_Normal normal;
	t_Tangent tangent;
	t_Bitangent bitangent;
};

struct t_Mesh
{
	char mMaterial[4][100] = {{0}};
	int mNumVertices = 0;
	int mFirstVertex = 0;
	int mNumIndices = 0;
	int mFirstIndex = 0;
};
