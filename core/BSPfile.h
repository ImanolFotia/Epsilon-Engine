///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef BSPFILE_H_INCLUDED
#define BSPFILE_H_INCLUDED

#include <pch.hpp>

#include <Core.hpp>

#include <BSPformat.h>
#include <CBitset.h>

class BSPfile
{
public:
    BSPfile();
    virtual ~BSPfile() {
        std::cout << "Deleted BSPfile" << std::endl;}

    /// This loads a .bsp file by it's file name (Returns true if successful)
    bool LoadBSP(const char *strFileName);

    /// This renders the level to the screen, currently the camera pos isn't being used
    void RenderLevel(const glm::vec3 &vPos);

    /// This destroys the level data
    void Destroy();

private:

    /// This attaches the correct extension to the file name, if found
    void FindTextureExtension(char *strFileName);

    /// This renders a single face to the screen
    void RenderFace(int faceIndex);

    int  m_numOfVerts;			/// The number of verts in the model
    int  m_numOfFaces;			/// The number of faces in the model
    int  m_numOfIndices;		/// The number of indices for the model
    int  m_numOfTextures;		/// The number of texture maps

    int			*m_pIndices;	/// The object's indices for rendering
    tBSPVertex  *m_pVerts;		/// The object's vertices
    tBSPFace	*m_pFaces;		/// The faces information of the object
    /// The texture and lightmap array for the level
    unsigned int m_textures[500];

    CBitset m_FacesDrawn;		/// The bitset for the faces that have/haven't been drawn

};

#endif /// BSPFILE_H_INCLUDED
