#pragma once
#include <glm/glm.hpp>
#include <iostream>
/** @brief Box structure with some helper functions
 * This box structure is the building block of the octree container
 * The box consists of a size and a position, the later representinf the bottom left corner
 * */
namespace engine
{
    struct Box
    {
        /**
         * @brief Size of the box
         */
        glm::vec3 size;
        /**
         * @brief The position of the bottom left corner of the box
         */
        glm::vec3 position;

        /**
         * @brief Construct a new Box object (default)
         *
         */
        Box() = default;
        /**
         * @brief Construct a new Box object, given position and size
         *
         * @param pos position of the box
         * @param s size of the box
         */
        Box(glm::vec3 pos, glm::vec3 s) : size(s), position(pos) {}

        /**
         * @brief Function for checking if this box can contain another box of size zero
         *
         * @param p position of the box
         * @return true
         * @return false
         */
        bool canContain(glm::vec3 p) const
        {
            return !(p.x < position.x || p.x > position.x + size.x ||
                     p.y < position.y || p.y > position.y + size.y ||
                     p.z < position.z || p.z > position.z + size.z);
        }

        /**
         * @brief Function for checking of this box can contain another box inside it
         *
         * @param other other box object
         * @return true
         * @return false
         */
        bool canContain(Box other) const
        {
            return (other.position.x >= position.x &&
                    other.position.x + other.size.x < position.x + size.x &&
                    other.position.y >= position.y &&
                    other.position.y + other.size.y < position.y + size.y &&
                    other.position.z >= position.z &&
                    other.position.z + other.size.z < position.z + size.z);
        }

        /**
         * @brief Function to check if this box overlaps with another box
         *
         * @param other other box object
         * @return true
         * @return false
         */

        bool overlaps(Box other) const
        {
            return (position.x < other.position.x + other.size.x &&
                    position.x + size.x >= other.position.x &&
                    position.y < other.position.y + other.size.y &&
                    position.y + size.y >= other.position.y &&
                    position.z < other.position.z + other.size.z &&
                    position.z + size.z >= other.position.z);
        }

        /**
         * @brief Returns the center of the mass of the box
         *
         * @return glm::vec3
         */
        inline glm::vec3 center() { return ((position + size) - position) / 2.0f; }

        /**
         * @brief Returns the size from the center of the box, to thier sides
         *
         * @return glm::vec3
         */
        inline glm::vec3 halfSize() { return size / 2.0f; }

        /**
         * @brief Function to debug print the box's data
         *
         * @param stream
         * @param other
         * @return std::ostream&
         */
        friend std::ostream &operator<<(std::ostream &stream, Box &other)
        {
            return stream << "Box with position : " << other.position.x << "\n"
                          << "And size: " << other.size.x << std::endl;
        }
    };
}