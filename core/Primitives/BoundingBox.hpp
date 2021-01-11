#pragma once

#include <pch.hpp>
#include "Primitive.hpp"

namespace Epsilon
{
    class BoundingBox : public PrimitiveBase
    {
    public:
        BoundingBox(uint8_t NumInstances = 0) : mNumInstances(NumInstances)
        {
            mInstanced = mNumInstances > 0;

            mVertexArray.addBuffer(GL_ARRAY_BUFFER, sizeof(float) * sizeof(PrimitiveHelpers::BoundingBoxVertices), &PrimitiveHelpers::BoundingBoxVertices, GL_STATIC_DRAW);
            mVertexArray.setAttribute(3, sizeof(float) * 3, (GLvoid *)0);

            if (mInstanced)
            {
                mPositionBufferHandler = mVertexArray.addBuffer(GL_ARRAY_BUFFER, mNumInstances * sizeof(glm::mat4), nullptr, GL_STREAM_DRAW);
                mVertexArray.setAttribute(4, sizeof(glm::mat4), (GLvoid *)0);
                mVertexArray.setAttribute(4, sizeof(glm::mat4), (GLvoid *)(sizeof(glm::vec4)));
                mVertexArray.setAttribute(4, sizeof(glm::mat4), (GLvoid *)(2 * sizeof(glm::vec4)));
                mVertexArray.setAttribute(4, sizeof(glm::mat4), (GLvoid *)(3 * sizeof(glm::vec4)));
            }
        }

        void Update() override
        {
        }

        void UpdatePositions(GLintptr offset, GLsizeiptr size, const void *data) {
            if(!mInstanced) return;

            mVertexArray.UpdateBuffer(mPositionBufferHandler, offset, size, data);
        }

        void Render() override
        {
            if (mInstanced)
            {
                mVertexArray.Bind();
                glDrawArraysInstanced(GL_LINES, 0, sizeof(PrimitiveHelpers::BoundingBoxVertices) / 3, mNumInstances);
                mVertexArray.Unbind();
            }
            else
            {
                mVertexArray.Bind();
                glDrawArrays(GL_LINES, 0, sizeof(PrimitiveHelpers::BoundingBoxVertices) / 3);
                mVertexArray.Unbind();
            }
        }

        void Destroy() override
        {
        }

    private:
        bool mInstanced = false;
        uint32_t mNumInstances = 0;
        uint32_t mPositionBufferHandler = 0;
    };
} // namespace Epsilon