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
#include <iomanip>
#include <glm/gtc/quaternion.hpp>
#include <Types.h>
#include <algorithm>
#include <limits>

//TODO: This file should be renamed as Helpers.hpp
namespace Helpers {

//Test point inside Box
inline static bool PointInBox(MIN_MAX_POINTS box, glm::vec3 point) {
    return (point.x > box.MIN_X && point.x < box.MAX_X &&
            point.y > box.MIN_Y && point.y < box.MAX_Y &&
            point.z > box.MIN_Z && point.z < box.MAX_Z);
}

//Test point inside a transformed Box
inline static bool PointInBox(MIN_MAX_POINTS box, glm::vec3 point, glm::mat4 model) {

    glm::vec4 min = model*glm::vec4(box.MIN_X, box.MIN_Y, box.MIN_Z, 1.0);
    glm::vec4 max = model*glm::vec4(box.MAX_X, box.MAX_Y, box.MAX_Z, 1.0);

    return (point.x > min.x && point.x < max.x &&
            point.y > min.y && point.y < max.y &&
            point.z > min.z && point.z < max.z);
}

inline
static std::string intTostring(int number) {
    std::ostringstream oss;
    oss << number;
    return std::string(oss.str());
}

inline
static std::string floatTostring(float number) {
    std::ostringstream oss;
    oss << number;
    return std::string(oss.str());
}

template < typename T > static std::string to_upper(T& n) {
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    return n;
}

template < typename T > static std::string to_string( const T& n ) {
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
}

template< typename T >
std::string to_hex( T i ) {
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(T)*2)
           << std::hex << i;
    return stream.str();
}

// Get's the size of the file in bytes.
static int GetFileLength( std::istream& file ) {
    int pos = file.tellg();
    file.seekg(0, std::ios::end );
    int length = file.tellg();
    // Restore the position of the get pointer
    file.seekg(pos);

    return length;
}

static void IgnoreLine( std::istream& file, int length ) {
    file.ignore( length, '\n' );
}

static void RemoveQuotes( std::string& str ) {
    size_t n;
    while ( ( n = str.find('\"') ) != std::string::npos ) str.erase(n,1);
}

//Minimum value for any given type
template <typename T>
inline static T getMinimum() {
    return std::numeric_limits<T>::min();
}

//Maximum value for any given type
template <typename T>
inline static T getMaximum() {
    return std::numeric_limits<T>::max();
}

//Clamp between two values
template <typename T>
auto static clamp(T a, T b, T c, double d = 0.0, double e = 1.0) -> T {
    a = (a < b) ? (b+d)*e : a;
    return (a > c) ? (c+d)*e : a;
};

//Carmack's Inverse
inline
static float Invsqrt(float x) {
    float xhalf = 0.5f*x;
    union {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5f3759df - (u.i >> 1);
    u.x = u.x * (1.5f - xhalf * u.x * u.x);
    return u.x;
}

inline
static float sqrt(float x) {
    float xhalf = 0.5f*x;
    union {
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
static void ComputeQuatW( glm::quat& quat ) {
    float t = 1.0f - ( quat.x * quat.x ) - ( quat.y * quat.y ) - ( quat.z * quat.z );
    if ( t < 0.0f ) {
        quat.w = 0.0f;
    } else {
        quat.w = -sqrtf(t);
    }
}

static std::string getExtension(std::string path) {
    std::string ext;

    std::size_t found = path.find(".");

    if(found == std::string::npos)
        return "NO EXTENSION";

    std::size_t EXTL = path.length() - found;

    for(unsigned int i = path.length()-EXTL ; i < path.length() ; i++) {
        ext += path.at(i);
    }
    return ext;
}

static std::string removeExtension(std::string path) {
    for(unsigned int i = path.length()-4 ; i < path.length() ; i++) {
        std::cout << path[i] << std::endl;
        path.erase(i);
    }
    return path;
}

static int findNearestPointFromSet(glm::vec3 TestingPoint, std::vector<glm::vec3> PointSet) {
    //std::cout << "Finding Nearest cubemap for mesh" << std::endl;
    //std::cout << "Point set: " << PointSet.size() << std::endl;
    int pointIndex = 0;
    float shortestLength = glm::length(PointSet[0] - TestingPoint);
    //std::cout << "llega" << std::endl;
    for(unsigned int i = 0; i < PointSet.size(); ++i) {
        if(glm::length(PointSet[i] - TestingPoint) < shortestLength) {
            shortestLength = glm::length(PointSet[i] - TestingPoint);
            pointIndex = i;
        }
    }
    //std::cout << "Returning Nearest cubemap index" << std::endl;
    //std::cout << "Shortest lenght: " << shortestLength << std::endl;
    return pointIndex;
}

//Test device's endianess
static bool isBigEndian() {
    int a=1;
    return !((char*)&a)[0];
}

template <typename T>
T SwapEndian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

static int ByteToInt(char* buffer,int len) {
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
class ElapsedTime {
public:
    ElapsedTime( float maxTimeStep = 0.03333f  )
        : m_fMaxTimeStep( maxTimeStep )
        , m_fPrevious ( std::clock() / (float)CLOCKS_PER_SEC ) {
    }

    float GetElapsedTime() const {
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

//Basic linear interpolation
static float lerp(float v0, float v1, float t) {
    return (1.0f-t)*v0 + t*v1;
}

}



#endif /// INCLUDES_H_INCLUDED
