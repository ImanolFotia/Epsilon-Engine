#pragma once

#include <pch.hpp>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <Driver/API/VertexArrayObject.hpp>
#include <Driver/API/OpenGL/VertexArrayObject.h>
#include <Driver/API/VertexBufferObject.hpp>

#include "Curve.hpp"

namespace Epsilon::Renderer
{

    struct ControlPoint {
        ControlPoint(glm::vec3 p) : position(p) {}
        ControlPoint(glm::vec3 p, glm::vec3 t, glm::vec3 n) : position(p), tangent(t), normal(n) {}
        glm::vec3 position;
        glm::vec3 tangent;
        glm::vec3 normal;
    };

    template <int numPoints>
    struct CurveSegment
    {
        void set(ControlPoint* p, uint32_t i)
        {
            if (i < numPoints)
                mPoints[i] = p;
        }

        ControlPoint a()
        {
            return *mPoints[0];
        }

        ControlPoint b()
        {
            return *mPoints[1];
        }

        ControlPoint c()
        {
            return *mPoints[2];
        }

        ControlPoint d()
        {
            if(numPoints < 4) throw std::runtime_error("trying to get inexistent point in bezier segment");
            return *mPoints[3];
        }

        std::array<ControlPoint*, numPoints> mPoints;
        uint32_t index;
    };

    template <int numPoints>
    class Bezier : public Curve
    {
    public:
        virtual void Update() = 0;

        virtual void Render() = 0;

        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) = 0;

        std::vector<CurveSegment<numPoints>> getSegments() { return mSegments; }

        uint32_t numSegments() { return mNumSegments; }

    protected:
        std::vector<CurveSegment<numPoints>> mSegments;
        std::vector<ControlPoint> mPoints;

        std::shared_ptr<API::VertexArrayObject> mVertexArray;
        unsigned int VertexBufferId = 0;
        uint32_t mNumSegments = 0;
    };
}