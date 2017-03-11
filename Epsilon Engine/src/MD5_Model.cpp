#include <Includes.h>
#include <iostream>
#include <MD5_Model.h>
#include <Texture.h>
MD5Model::MD5Model()
    : m_iMD5Version(-1)
    , m_iNumJoints(0)
    , m_iNumMeshes(0)
    , m_bHasAnimation(false)
    , m_LocalToWorldMatrix(1)
{
}

MD5Model::~MD5Model()
{
        std::cout << "Deleted MD5 Model" << endl;
}

bool MD5Model::LoadModel( const std::string &filename )
{


    std::string param;
    std::string junk;   // Read junk from the file

    std::ifstream file(filename);
    int fileLength = Helpers::GetFileLength( file );
    assert( fileLength > 0 );

    m_Joints.clear();
    m_Meshes.clear();

    file >> param;

    while ( !file.eof() )
    {
        if ( param == "MD5Version" )
        {
            file >> m_iMD5Version;
            assert( m_iMD5Version == 10 );
        }
        else if ( param == "commandline" )
        {
            Helpers::IgnoreLine(file, fileLength ); // Ignore the contents of the line
        }
        else if ( param == "numJoints" )
        {
            file >> m_iNumJoints;
            m_Joints.reserve(m_iNumJoints);
            m_AnimatedBones.assign( m_iNumJoints, glm::mat4x4(1.0f) );
        }
        else if ( param == "numMeshes" )
        {
            file >> m_iNumMeshes;
            m_Meshes.reserve(m_iNumMeshes);
        }
        else if ( param == "joints" )
        {
            Joint joint;
            file >> junk; // Read the '{' character
            for ( int i = 0; i < m_iNumJoints; ++i )
            {
                file >> joint.m_Name >> joint.m_ParentID >> junk
                     >> joint.m_Pos.x >> joint.m_Pos.y >> joint.m_Pos.z >> junk >> junk
                     >> joint.m_Orient.x >> joint.m_Orient.y >> joint.m_Orient.z >> junk;

                Helpers::RemoveQuotes( joint.m_Name );
                Helpers::ComputeQuatW( joint.m_Orient );

                m_Joints.push_back(joint);
                // Ignore everything else on the line up to the end-of-line character.
                Helpers::IgnoreLine( file, fileLength );
            }
            file >> junk; // Read the '}' character

            BuildBindPose( m_Joints );
        }
        else if ( param == "mesh" )
        {
            Mesh mesh;
            int numVerts, numTris, numWeights;

            file >> junk; // Read the '{' character
            file >> param;
            while ( param != "}" )  // Read until we get to the '}' character
            {
                if ( param == "shader" )
                {
                    file >> mesh.m_Shader;
                    Helpers::RemoveQuotes( mesh.m_Shader );
                    std::string texturePath = mesh.m_Shader;
                    eTexture texd(texturePath.c_str());
                    mesh.m_TexID = texd.getTextureID();
                    std::string ext = Helpers::getExtension(texturePath);
                    std::string n_path = Helpers::removeExtension(texturePath) + "_n" + ".tga";
                    std::string s_path = Helpers::removeExtension(texturePath) + "_s" + ".tga";
                    std::string m_path = Helpers::removeExtension(texturePath) + "_m" + ".tga";
                    //std::cout << n_path << std::endl;
                    eTexture texn((n_path).c_str());
                    mesh.m_nTexID = texn.getTextureID();
                    //std::cout << "Texture: " << texturePath << std::endl;
                    eTexture texs((s_path).c_str());
                    mesh.m_sTexID = texs.getTextureID();

                    eTexture texm((m_path).c_str());
                    mesh.m_mTexID = texm.getTextureID();

                    file.ignore(fileLength, '\n' ); // Ignore everything else on the line
                }
                else if ( param == "numverts")
                {
                    file >> numVerts;               // Read in the vertices
                    Helpers::IgnoreLine(file, fileLength);
                    for ( int i = 0; i < numVerts; ++i )
                    {
                        Vertex vert;

                        file >> junk >> junk >> junk                    // vert vertIndex (
                             >> vert.m_Tex0.x >> vert.m_Tex0.y >> junk  //  s t )
                             >> vert.m_StartWeight >> vert.m_WeightCount;

                        Helpers::IgnoreLine(file, fileLength);

                        mesh.m_Verts.push_back(vert);
                        mesh.m_Tex2DBuffer.push_back(vert.m_Tex0);
                    }
                }
                else if ( param == "numtris" )
                {
                    file >> numTris;
                    Helpers::IgnoreLine(file, fileLength);
                    for ( int i = 0; i < numTris; ++i )
                    {
                        Triangle tri;
                        file >> junk >> junk >> tri.m_Indices[0] >> tri.m_Indices[1] >> tri.m_Indices[2];

                        Helpers::IgnoreLine( file, fileLength );

                        mesh.m_Tris.push_back(tri);
                        mesh.m_IndexBuffer.push_back( (GLuint)tri.m_Indices[0] );
                        mesh.m_IndexBuffer.push_back( (GLuint)tri.m_Indices[1] );
                        mesh.m_IndexBuffer.push_back( (GLuint)tri.m_Indices[2] );
                    }
                }
                else if ( param == "numweights" )
                {
                    file >> numWeights;
                    Helpers::IgnoreLine( file, fileLength );
                    for ( int i = 0; i < numWeights; ++i )
                    {
                        Weight weight;
                        file >> junk >> junk >> weight.m_JointID >> weight.m_Bias >> junk
                             >> weight.m_Pos.x >> weight.m_Pos.y >> weight.m_Pos.z >> junk;

                        Helpers::IgnoreLine( file, fileLength );
                        mesh.m_Weights.push_back(weight);
                    }
                }
                else
                {
                    Helpers::IgnoreLine(file, fileLength);
                }

                file >> param;
            }

            PrepareMesh(mesh);
            PrepareNormals(mesh);
            CalcTangentSpace(mesh);
            this->PrepareVAO(mesh);
            this->PrepareSkeletonVAO();
            m_Meshes.push_back(mesh);

        }

        file >> param;
    }
    //BuildBindPose();

    assert( m_Joints.size() == m_iNumJoints );
    assert( m_Meshes.size() == m_iNumMeshes );

    return true;
}

bool MD5Model::LoadAnim( const std::string& filename )
{
    MD5Animation m_Animation;
    if ( m_Animation.LoadAnimation( filename ) )
    {
        // Check to make sure the animation is appropriate for this model
        m_bHasAnimation = CheckAnimation( m_Animation );
    }
    m_Animations.push_back(m_Animation);

    return m_bHasAnimation;
}

bool MD5Model::CheckAnimation( const MD5Animation& animation ) const
{
    if ( m_iNumJoints != animation.GetNumJoints() )
    {
        return false;
    }

    // Check to make sure the joints match up
    for ( unsigned int i = 0; i < m_Joints.size(); ++i )
    {
        const Joint& meshJoint = m_Joints[i];
        const MD5Animation::JointInfo& animJoint = animation.GetJointInfo( i );

        if ( meshJoint.m_Name != animJoint.m_Name ||
                meshJoint.m_ParentID != animJoint.m_ParentID )
        {
            return false;
        }
    }

    return true;
}


// Compute the position of the vertices in object local space
// in the skeleton's bind pose
bool MD5Model::PrepareMesh( Mesh& mesh )
{
    mesh.m_PositionBuffer.clear();
    mesh.m_Tex2DBuffer.clear();
    mesh.m_BoneIndex.clear();
    mesh.m_BoneWeights.clear();
    // Compute vertex positions
    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        glm::vec3 finalPos(0);
        Vertex& vert = mesh.m_Verts[i];

        vert.m_Pos = glm::vec3(0);
        vert.m_Normal = glm::vec3(0);
        vert.m_BoneWeights = glm::vec4(0);
        vert.m_BoneIndices = glm::vec4(0);
        // Sum the position of the weights
        for ( int j = 0; j < vert.m_WeightCount; ++j )
        {
            //if(j >= 4)
            //    continue;

            Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
            Joint& joint = m_Joints[weight.m_JointID];

            // Convert the weight position from Joint local space to object space
            glm::vec3 rotPos = glm::normalize(joint.m_Orient) * weight.m_Pos;

            vert.m_Pos += ( joint.m_Pos + rotPos ) * weight.m_Bias;
            if(j >= 4)
            {
                vert.m_BoneIndices[3] = (float)weight.m_JointID;
                vert.m_BoneWeights[3] = weight.m_Bias;
            }
            else
            {
                vert.m_BoneIndices[j] += (float)weight.m_JointID;
                vert.m_BoneWeights[j] += weight.m_Bias;
            }
        }
        mesh.m_PositionBuffer.push_back(vert.m_Pos);
        mesh.m_Tex2DBuffer.push_back(vert.m_Tex0);
        mesh.m_BoneIndex.push_back(vert.m_BoneIndices);
        mesh.m_BoneWeights.push_back(vert.m_BoneWeights);
    }

    return true;
}
/*
bool MD5Model::PrepareMesh( Mesh& mesh, const std::vector<glm::mat4x4>& skel )
{

    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        const Vertex& vert = mesh.m_Verts[i];
        glm::vec3& pos = mesh.m_PositionBuffer[i];
        glm::vec3& normal = mesh.m_NormalBuffer[i];
        glm::vec3& tangent = mesh.m_TangentBuffer[i];
        glm::vec3& binormal = mesh.m_BinormalBuffer[i];

        pos = glm::vec3(0);
        normal = glm::vec3(0);
        tangent = glm::vec3(0);
        binormal = glm::vec3(0);

        for ( int j = 0; j < vert.m_WeightCount; ++j )
        {
            const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
            const glm::mat4x4 boneMatrix = skel[weight.m_JointID];

            pos += glm::vec3( ( boneMatrix * glm::vec4( vert.m_Pos, 1.0f ) ) * weight.m_Bias );
            normal += glm::vec3( ( boneMatrix * glm::vec4( vert.m_Normal, 0.0f ) ) * weight.m_Bias );
            tangent += glm::vec3( ( boneMatrix * glm::vec4( vert.m_Tangent , 0.0f ) ) * weight.m_Bias);
            binormal += glm::vec3( ( boneMatrix * glm::vec4( vert.m_Binormal , 0.0f ) ) * weight.m_Bias);
        }
    }


    return true;
}
*/

glm::vec3 MD5Model::getJointPositionByName(std::string jointName)
{
    int index = 0;
    for(int i = 0; i < m_Joints.size(); ++i)
    {
        if(m_Joints[i].m_Name == jointName)
        {
            index = i;
            break;
        }
    }
    return m_Joints[index].m_Pos;
}

void MD5Model::BuildBindPose( const JointList& joints )
{
    m_BindPose.clear();
    m_InverseBindPose.clear();

    JointList::const_iterator iter = joints.begin();
    while ( iter != joints.end() )
    {
        const Joint& joint = (*iter);
        glm::mat4x4 boneTranslation = glm::translate( glm::mat4x4() ,joint.m_Pos );
        glm::mat4x4 boneRotation = glm::mat4_cast( joint.m_Orient );

        glm::mat4x4 boneMatrix = boneTranslation * boneRotation;

        glm::mat4x4 inverseBoneMatrix = glm::inverse( boneMatrix );

        m_BindPose.push_back( boneMatrix );
        m_InverseBindPose.push_back( inverseBoneMatrix );

        ++iter;
    }
}


// Compute the vertex normals in the Mesh's bind pose
bool MD5Model::PrepareNormals( Mesh& mesh )
{
    mesh.m_NormalBuffer.clear();

    // Loop through all triangles and calculate the normal of each triangle
    for ( unsigned int i = 0; i < mesh.m_Tris.size(); ++i )
    {
        glm::vec3 v0 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Pos;
        glm::vec3 v1 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Pos;
        glm::vec3 v2 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Pos;

        glm::vec3 normal = glm::cross( v2 - v0, v1 - v0 );

        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Normal = normal;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Normal = normal;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Normal = normal;
    }

    // Now normalize all the normals
    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        Vertex& vert = mesh.m_Verts[i];

        vert.m_Normal = glm::normalize( vert.m_Normal );
        mesh.m_NormalBuffer.push_back( vert.m_Normal );
    }

    return true;
}


bool MD5Model::CalcTangentSpace(Mesh& mesh)
{
    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    //std::cout << mesh.m_Tris.size() <<  std::endl;
    for(int i = 0 ; i < mesh.m_Tris.size(); i++)
    {
        glm::vec3 v0 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Pos;
        glm::vec3 v1 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Pos;
        glm::vec3 v2 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Pos;

        glm::vec3 edge1 = v2 - v0;
        glm::vec3 edge2 = v1 - v0;

        glm::vec2 t0 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Tex0;
        glm::vec2 t1 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Tex0;
        glm::vec2 t2 = mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Tex0;

        glm::vec2 deltaUV1 = t1 - t0;
        glm::vec2 deltaUV2 = t2 - t0;

        //GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y + deltaUV2.x * deltaUV1.y);

        float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;
        // when t1, t2, t3 in same position in UV space, just use default UV direction.
        if ( 0 == deltaUV2.x && 0 ==deltaUV2.y && 0 == deltaUV1.x && 0 == deltaUV1.y )
        {
            deltaUV1.x = 0.0;
            deltaUV1.y = 1.0;
            deltaUV2.y = 1.0;
            deltaUV2.y = 0.0;
        }

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        //tangent1 = glm::normalize(tangent1);

        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Tangent = tangent1;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Tangent = tangent1;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Tangent = tangent1;

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        //bitangent1 = glm::normalize(bitangent1);

        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[0] ].m_Binormal = bitangent1;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[1] ].m_Binormal = bitangent1;
        mesh.m_Verts[ mesh.m_Tris[i].m_Indices[2] ].m_Binormal = bitangent1;
    }

    for ( unsigned int i = 0; i < mesh.m_Verts.size(); ++i )
    {
        Vertex& vert = mesh.m_Verts[i];
        // Reset the normal to calculate the bind-pose normal in joint space

        glm::vec3 tangent = vert.m_Tangent ;
        mesh.m_TangentBuffer.push_back( tangent );

        glm::vec3 binormal =vert.m_Binormal ;
        mesh.m_BinormalBuffer.push_back( binormal );

        glm::vec3 normal = vert.m_Normal ;

        vert.m_Tangent = glm::vec3(0);
        vert.m_Binormal = glm::vec3(0);

        // re-orthogonalize T with respect to N
        tangent = glm::normalize(tangent - glm::dot(tangent, normal) * normal);
        // then retrieve perpendicular vector B with the cross product of T and N
        binormal = glm::cross(tangent, normal);

        if (glm::dot(glm::cross(normal, tangent), binormal) < 0.0f)
        {
            tangent = tangent * -1.0f;
        }
        /*
                // Put the bind-pose normal into joint-local space
                // so the animated normal can be computed faster later
                for ( int j = 0; j < vert.m_WeightCount; ++j )
                {
                    const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
                    const Joint& joint = m_Joints[weight.m_JointID];
                    vert.m_Tangent +=  (tangent * glm::normalize(joint.m_Orient)) * weight.m_Bias;
                    vert.m_Binormal +=  (binormal * glm::normalize(joint.m_Orient)) * weight.m_Bias;
                }*/
    }

    return true;
}

bool MD5Model::PrepareVAO(Mesh& mesh)
{
    GLuint UVB, EBO;
    glGenVertexArrays(1, &mesh.m_VAO);
    glGenBuffers(1, &mesh.m_VBO);
    glGenBuffers(1, &UVB);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &mesh.m_TBO);
    glGenBuffers(1, &mesh.m_BBO);
    glGenBuffers(1, &mesh.m_NBO);
    glGenBuffers(1, &mesh.m_WBO);
    glGenBuffers(1, &mesh.m_IBO);
    glGenBuffers(1, &mesh.m_IBBO);
    glBindVertexArray(mesh.m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_PositionBuffer.size(), &mesh.m_PositionBuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, UVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.m_Tex2DBuffer.size(), &mesh.m_Tex2DBuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_NormalBuffer.size(), &mesh.m_NormalBuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_TangentBuffer.size(), &mesh.m_TangentBuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_BBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.m_BinormalBuffer.size(), &mesh.m_BinormalBuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_WBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * mesh.m_BoneWeights.size(), &mesh.m_BoneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_IBBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * mesh.m_BoneIndex.size(), &mesh.m_BoneIndex[0], GL_STATIC_DRAW);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
    glEnableVertexAttribArray(6);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.m_IndexBuffer.size(), &mesh.m_IndexBuffer[0], GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    ///std::cout << "VBO Loaded" << std::endl;
    return true;
}

void MD5Model::PrepareSkeletonVAO()
{

    for ( unsigned int i = 0; i < m_Joints.size(); ++i )
    {
        Skeleton skel;
        const Joint& j0 = m_Joints[i];
        if ( j0.m_ParentID != -1 )
        {

            const Joint& j1 = m_Joints[j0.m_ParentID];
            GLfloat bone[] =
            {
                j0.m_Pos.x, j0.m_Pos.y, j0.m_Pos.z, 1.0, 0.0, 0.0,
                j1.m_Pos.x, j1.m_Pos.y, j1.m_Pos.z, 1.0, 0.0, 0.0
            };

            glGenVertexArrays(1, &skel.VAO);
            glGenBuffers(1, &skel.VBO);

            glBindVertexArray(skel.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, skel.VBO);

            glBufferData(GL_ARRAY_BUFFER, sizeof(bone), bone, GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6,(GLvoid*)(sizeof(GLfloat) * 3));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);


        }
        m_Skeletons.push_back(skel);

    }




    for(int i = 0 ; i < m_Joints.size() ; i++)
    {
        JointInfo info;
        info.x = m_Joints[i].m_Pos.x;
        info.y = m_Joints[i].m_Pos.y;
        info.z = m_Joints[i].m_Pos.z;

        info.r = 1.0;
        info.g = 0.0;
        info.b = 0.0;

        m_JointDrawInfo.Info.push_back(info);

        //std::cout << "Joints: x = " << info.x << " y = " << info.y << " z = " << info.z << std::endl;
    }

    //std::cout<< "llega " << std::endl;
    GLuint col;
    glGenVertexArrays(1, &m_JointDrawInfo.VAO);
    glGenBuffers(1, &m_JointDrawInfo.VBO);

    glBindVertexArray(m_JointDrawInfo.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_JointDrawInfo.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * m_JointDrawInfo.Info.size(), &m_JointDrawInfo.Info[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);




}

void MD5Model::Update( float fDeltaTime, float blend )
{

    if ( m_bHasAnimation )
    {
        m_Animations.at(0).Update(fDeltaTime);

        const MatrixList& animatedSkeleton = m_Animations.at(0).GetSkeletonMatrixList();

        m_Animations.at(1).Update(fDeltaTime);

        const MatrixList& animatedSkeleton2 = m_Animations.at(1).GetSkeletonMatrixList();

        for ( int i = 0; i < m_iNumJoints; ++i )
        {
            m_AnimatedBones[i] = glm::mix(animatedSkeleton[i], animatedSkeleton2[i], blend) * m_InverseBindPose[i] * glm::mat4x4(1);
        }

    }
    else
    {
        // No animation.. Just use identity matrix for each bone.
        m_AnimatedBones.assign(m_iNumJoints, glm::mat4x4(1.0) );
    }
}

void MD5Model::Render(GLuint shader)
{
    // Render the meshes
    for ( unsigned int i = 0; i < m_Meshes.size(); ++i )
    {
        //if(i != 1 && i != 0/*&& i != 5 && i != 6*/)
        //continue;
        RenderMesh( m_Meshes[i], shader);

    }
}

void MD5Model::RenderMesh( const Mesh& mesh, GLuint shader )
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, mesh.m_TexID );
    glUniform1i(glGetUniformLocation(shader, "sampler"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, mesh.m_nTexID );
    glUniform1i(glGetUniformLocation(shader, "n_sampler"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture( GL_TEXTURE_2D, mesh.m_sTexID );
    glUniform1i(glGetUniformLocation(shader, "s_sampler"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mesh.m_mTexID);
    glUniform1i(glGetUniformLocation(shader, "m_sampler"), 3);

    glBindVertexArray(mesh.m_VAO);

    glDrawElements( GL_TRIANGLES, mesh.m_IndexBuffer.size(), GL_UNSIGNED_INT, (GLvoid*)0);
    glBindTexture( GL_TEXTURE_2D, 0 );

}

void MD5Model::RenderSkeleton()
{

    for ( unsigned int i = 0; i < m_Joints.size(); ++i )
    {
        const MD5Animation::SkeletonJoint& j0 = this->finalskeleton.m_Joints[i];
        if ( j0.m_Parent != -1 )
        {

            const MD5Animation::SkeletonJoint& j1 = this->finalskeleton.m_Joints[j0.m_Parent];
            GLfloat bone[] =
            {
                j0.m_Pos.x, j0.m_Pos.y, j0.m_Pos.z, 1.0, 1.0, 0.0,
                j1.m_Pos.x, j1.m_Pos.y, j1.m_Pos.z, 1.0, 1.0, 0.0
            };

            glBindVertexArray(m_Skeletons[i].VAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_Skeletons[i].VBO);

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bone), bone);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

        }
    }
    for ( unsigned int i = 0; i < m_Joints.size(); ++i )
    {

        glBindVertexArray(m_Skeletons[i].VAO);
        glDrawArrays( GL_LINES, 0, 2);
        glBindVertexArray(0);
    }

}

void MD5Model::RenderJoints()
{


    glBindVertexArray(m_JointDrawInfo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_JointDrawInfo.VBO);

    for(int i = 0 ; i < m_JointDrawInfo.Info.size() ; i++)
    {
        m_JointDrawInfo.Info.at(i).x = this->finalskeleton.m_Joints[i].m_Pos.x;
        m_JointDrawInfo.Info.at(i).y = this->finalskeleton.m_Joints[i].m_Pos.y;
        m_JointDrawInfo.Info.at(i).z = this->finalskeleton.m_Joints[i].m_Pos.z;

    }

    glBufferSubData(GL_ARRAY_BUFFER, 0,  sizeof(float) * 6 * m_JointDrawInfo.Info.size(), &m_JointDrawInfo.Info[0]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(m_JointDrawInfo.VAO);
    glDrawArrays( GL_POINTS, 0, m_JointDrawInfo.Info.size()/4);
}
