///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#ifndef INCLUDES_H_INCLUDED
#define INCLUDES_H_INCLUDED

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <glm/gtc/quaternion.hpp>

namespace Helpers
{

inline
static std::string intTostring(int number)
{
    std::ostringstream oss;
    oss << number;
    return std::string(oss.str());
}

inline
static std::string floatTostring(float number)
{
    std::ostringstream oss;
    oss << number;
    return std::string(oss.str());
}

// Get's the size of the file in bytes.
static int GetFileLength( std::istream& file )
{
    int pos = file.tellg();
    file.seekg(0, std::ios::end );
    int length = file.tellg();
    // Restore the position of the get pointer
    file.seekg(pos);

    return length;
}

static void IgnoreLine( std::istream& file, int length )
{
    file.ignore( length, '\n' );
}

static void RemoveQuotes( std::string& str )
{
    size_t n;
    while ( ( n = str.find('\"') ) != std::string::npos ) str.erase(n,1);
}

inline
static float Invsqrt(float x)
{
    float xhalf = 0.5f*x;
    union
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5f3759df - (u.i >> 1);
    u.x = u.x * (1.5f - xhalf * u.x * u.x);
    return u.x;
}

inline
static float sqrt(float x)
{
    float xhalf = 0.5f*x;
    union
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5f3759df - (u.i >> 1);
    u.x = u.x * (1.5f - xhalf * u.x * u.x);
    return  1/u.x;
}

// Computes the W component of the quaternion based on the X, Y, and Z components.
// This method assumes the quaternion is of unit length.
static void ComputeQuatW( glm::quat& quat )
{
    float t = 1.0f - ( quat.x * quat.x ) - ( quat.y * quat.y ) - ( quat.z * quat.z );
    if ( t < 0.0f )
    {
        quat.w = 0.0f;
    }
    else
    {
        quat.w = -sqrtf(t);
    }
}

static std::string getExtension(std::string path)
{
    std::string ext;

    std::size_t found = path.find(".");

    if(found == std::string::npos)
        return "0";

    for(int i = path.length()-4 ; i < path.length() ; i++)
    {
        ext += path.at(i);
    }
    return ext;
}

static std::string removeExtension(std::string path)
{
    for(int i = path.length()-4 ; i < path.length() ; i++)
    {
        std::cout << path[i] << std::endl;
        path.erase(i);
    }
    return path;
}

static int findNearestPointFromSet(glm::vec3 TestingPoint, std::vector<glm::vec3> PointSet)
{
    std::cout << "Finding Nearest cubemap for mesh" << std::endl;
    std::cout << "Point set: " << PointSet.size() << std::endl;
    int pointIndex = 1;
    float shortestLength = glm::length(PointSet[0] - TestingPoint);
    std::cout << "llega" << std::endl;
    for(int i = 0; i < PointSet.size(); ++i)
    {
        if(glm::length(PointSet[i] - TestingPoint) < shortestLength)
        {
            shortestLength = glm::length(PointSet[i] - TestingPoint);
            pointIndex = i+1;
        }
    }
    std::cout << "Returning Nearest cubemap index" << std::endl;
    std::cout << "Shortest lenght: " << shortestLength << std::endl;
    return pointIndex;
}

static bool isBigEndian()
{
    int a=1;
    return !((char*)&a)[0];
}

static int ByteToInt(char* buffer,int len)
{
    int a=0;
    if(!isBigEndian())
        for(int i=0; i<len; i++)
            ((char*)&a)[i]=buffer[i];
    else
        for(int i=0; i<len; i++)
            ((char*)&a)[3-i]=buffer[i];
    return a;
}

// Helper class to count frame time
class ElapsedTime
{
public:
    ElapsedTime( float maxTimeStep = 0.03333f  )
        : m_fMaxTimeStep( maxTimeStep )
        , m_fPrevious ( std::clock() / (float)CLOCKS_PER_SEC )
    {}

    float GetElapsedTime() const
    {
        float fCurrentTime = std::clock() / (float)CLOCKS_PER_SEC;
        float fDeltaTime = fCurrentTime - m_fPrevious;
        m_fPrevious = fCurrentTime;

        // Clamp to the max time step
        fDeltaTime = std::min( fDeltaTime, m_fMaxTimeStep );

        return fDeltaTime;
    }

private:
    float m_fMaxTimeStep;
    mutable float m_fPrevious;
};
}



#endif /// INCLUDES_H_INCLUDED
