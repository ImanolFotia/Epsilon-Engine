#pragma once

#include <GL/glew.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "MD5_Anim.h"
#include <SOIL.h>

class MD5Model
{
public:
    MD5Model();
    virtual ~MD5Model();

    bool LoadModel( const std::string& filename );
    bool LoadAnim( const std::string& filename );
    void Update( float fDeltaTime, float blend );
    void Render(GLuint shader);
    void RenderSkeleton();
    void RenderJoints();
    glm::vec3 getJointPositionByName(std::string jointName);

protected:
    typedef std::vector<glm::vec3> PositionBuffer;
    typedef std::vector<glm::vec3> NormalBuffer;
    typedef std::vector<glm::vec3> TangentBuffer;
    typedef std::vector<glm::vec3> BinormalBuffer;
    typedef std::vector<glm::vec2> Tex2DBuffer;
    typedef std::vector<glm::vec4> WeightBuffer;
    typedef std::vector<glm::vec4> BoneIndexBuffer;
    typedef std::vector<GLuint> IndexBuffer;

    struct Vertex
    {
        glm::vec3   m_Pos;
        glm::vec3   m_Normal;
        glm::vec3   m_Tangent;
        glm::vec3   m_Binormal;
        glm::vec2   m_Tex0;
        glm::vec4   m_BoneWeights;
        glm::vec4   m_BoneIndices;

        //deprecated
        int         m_StartWeight;
        int         m_WeightCount;
    };
    typedef std::vector<Vertex> VertexList;

    struct Triangle
    {
        int             m_Indices[3];
    };
    typedef std::vector<Triangle> TriangleList;

    struct Weight
    {
        int             m_JointID;
        float           m_Bias;
        glm::vec3       m_Pos;
    };
    typedef std::vector<Weight> WeightList;

    struct Joint
    {
        std::string     m_Name;
        int             m_ParentID;
        glm::vec3       m_Pos;
        glm::quat       m_Orient;
    };
    typedef std::vector<Joint> JointList;

    struct Mesh
    {
        std::string         m_Shader;
        // This vertex list stores the vertices in the bind pose.
        VertexList          m_Verts;
        TriangleList        m_Tris;
        WeightList          m_Weights;
        glm::mat4           m_BindPoseMatrix;

        // A texture ID for the material
        GLuint              m_TexID;
        GLuint              m_nTexID;
        GLuint              m_sTexID;
        GLuint              m_mTexID;

        // These buffers are used for rendering the animated mesh
        PositionBuffer      m_PositionBuffer;   // Vertex position stream
        NormalBuffer        m_NormalBuffer;     // Vertex normals stream
        TangentBuffer       m_TangentBuffer;     // Vertex tangents stream
        BinormalBuffer      m_BinormalBuffer;     // Vertex binormals stream
        Tex2DBuffer         m_Tex2DBuffer;      // Texture coordinate set
        IndexBuffer         m_IndexBuffer;      // Vertex index buffer
        WeightBuffer        m_BoneWeights;      // Bone weights buffer
        BoneIndexBuffer     m_BoneIndex;       // Bone index buffer

        GLuint              m_VAO;              // Meshes' Vertex Array Object
        GLuint              m_VBO;              // Meshes' Vertex Buffer Array
        GLuint              m_NBO;
        GLuint              m_TBO;
        GLuint              m_BBO;
        GLuint              m_WBO;
        GLuint              m_IBBO;
        GLuint              m_IBO;
    };
    typedef std::vector<Mesh> MeshList;
    typedef std::vector<glm::mat4x4> MatrixList;

    // Prepare the mesh for rendering
    // Compute vertex positions and normals
    bool PrepareMesh( Mesh& mesh );
    bool PrepareMesh( Mesh& mesh, const std::vector<glm::mat4x4>& skel );
    bool PrepareNormals( Mesh& mesh );
    bool PrepareVAO(Mesh& mesh);
    void PrepareSkeletonVAO();
    bool CalcTangentSpace(Mesh& mesh);
    void BuildBindPose( const JointList& joints );

    // Render a single mesh of the model
    void RenderMesh( const Mesh& mesh, GLuint shader );
    void RenderNormals( const Mesh& mesh );

    // Draw the skeleton of the mesh for debugging purposes.
    void RenderSkeleton( const JointList& joints );

    bool CheckAnimation( const MD5Animation& animation ) const;

private:

    int                 m_iMD5Version;
    int                 m_iNumJoints;
    int                 m_iNumMeshes;

    bool                m_bHasAnimation;

    JointList           m_Joints;
    MeshList            m_Meshes;

    MatrixList          m_BindPose;
    MatrixList          m_InverseBindPose;

    // Animated bone matrix from the animation with the inverse bind pose applied.

    glm::mat4x4         m_LocalToWorldMatrix;

    std::vector<MD5Animation>        m_Animations;

    struct Skeleton
    {
        GLuint VAO;
        GLuint VBO;
    };

    std::vector<Skeleton> m_Skeletons;

    struct JointInfo
    {
     float x;
     float y;
     float z;

     float r;
     float g;
     float b;
    };

    struct JointDrawInfo
    {
        GLuint VAO;
        GLuint VBO;
        std::vector<JointInfo> Info;
    };

    JointDrawInfo m_JointDrawInfo;

    MD5Animation::FrameSkeleton finalskeleton;

public:

        std::vector<glm::mat4> SkeletonMatrix;

        std::vector<glm::mat4> GetAnimatedSkeletonMatrix();

        MatrixList          m_AnimatedBones;

};
