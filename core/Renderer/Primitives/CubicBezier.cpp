#include "CubicBezier.hpp"

namespace Epsilon::Renderer
{
    void CubicBezier::Render()
    {
        mVertexArray->Bind();

        glDrawArrays(GL_LINE_STRIP, 0, mSamples.size());

        mVertexArray->Unbind();
    }

    void CubicBezier::Draw(std::shared_ptr<Shader> shader, bool force_draw, std::initializer_list<unsigned int> mesh_index_list)
    {
        shader->Use();
        mVertexArray->Bind();

        glLineWidth(2.0);
        glEnable(GL_LINE_SMOOTH);
        glDrawArrays(GL_LINE_LOOP, 0, mSamples.size());

        mVertexArray->Unbind();

        shader->Free();
    }

    std::vector<ControlPoint> CubicBezier::getSamples()
    {
        return mSamples;
    }

    glm::vec3 CubicBezier::p_fderivative(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t)
    {
        return (P1.position - P0.position) * (3.0f * std::pow(1.0f - t, 2.0f)) +
               (P2.position - P1.position) * (6.0f * t * (1.0f - t)) +
               (P3.position - P2.position) * (3.0f * std::pow(t, 2.0f));
    }

    glm::vec3 CubicBezier::p_sderivative(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t)
    {
        return (P2.position - 2.0f * P1.position + P0.position) * (6.0f * (1.0f - t)) +
               (P3.position - 2.0f * P2.position + P1.position) * (6.0f * t);
    }

    uint32_t CubicBezier::numSamples() { return mNumSamples; }

    uint32_t CubicBezier::numSamplesPerSegment() { return mNumSamplesPerSegment; }

    void CubicBezier::addInitialSegment()
    {
        addSegment(ControlPoint(glm::vec3(-2.0, 4.0, 0.0) * 2.0f),
                   ControlPoint(glm::vec3(-2.0, 3.0, 2.0) * 2.0f),
                   ControlPoint(glm::vec3(2.0, 3.0, 2.0) * 2.0f),
                   ControlPoint(glm::vec3(2.0, 2.0, 0.0) * 2.0f));

        addSegment(ControlPoint(glm::vec3(2.0, 2.0, 0.0) * 2.0f),
                   ControlPoint(glm::vec3(-2.0, 3.0, -2.0) * 2.0f),
                   ControlPoint(glm::vec3(-2.0, 3.0, 3.0) * 2.0f),
                   ControlPoint(glm::vec3(2.0, 4.0, 0.0) * 2.0f));
        /*
concatenateSegment(glm::vec3(3.0, 2.0, 3.0) * 2.0f,
                glm::vec3(1.0, 3.0, 1.0) * 2.0f);
*/
        concatenateSegment(ControlPoint(glm::vec3(-5.0, 2.0, 0.0) * 2.0f),
                           ControlPoint(glm::vec3(-2.0, 4.0, 0.0) * 2.0f));
    }

    void CubicBezier::createGeometry()
    {
        mVertexArray = std::make_shared<API::OpenGL::VertexArrayObject>();

        VertexBufferId = mVertexArray->addBuffer(mSamples.size() * sizeof(ControlPoint), &mSamples[0], GL_STATIC_DRAW);
        mVertexArray->setAttribute(VERTEX_POINTER_INDEX::POSITION, 3, sizeof(ControlPoint), (GLvoid *)0);
        mVertexArray->setAttribute(1, 3, sizeof(ControlPoint), (void *)offsetof(ControlPoint, tangent));
        mVertexArray->setAttribute(VERTEX_POINTER_INDEX::NORMAL, 3, sizeof(ControlPoint), (void *)offsetof(ControlPoint, normal));
    }

    void CubicBezier::updateGeometry()
    {
        if (mSamples.size() * sizeof(ControlPoint) != mVertexArray->getBuffer(VertexBufferId)->Size())
        {
            mVertexArray->DestroyBuffer(VertexBufferId);
            VertexBufferId = mVertexArray->addBuffer(mSamples.size() * sizeof(ControlPoint), &mSamples[0], GL_STATIC_DRAW);
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::POSITION, 3, sizeof(ControlPoint), (GLvoid *)0);
            mVertexArray->setAttribute(1, 3, sizeof(ControlPoint), (void *)offsetof(ControlPoint, tangent));
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::NORMAL, 3, sizeof(ControlPoint), (void *)offsetof(ControlPoint, normal));
            return;
        }
        else
        {
            mVertexArray->UpdateBuffer(mVertexArray->getBuffer(VertexBufferId)->Get(), 0, mSamples.size() * sizeof(ControlPoint), &mSamples[0]);
        }
    }

    void CubicBezier::createSamplePoints(int numSamples)
    {
        mSamples.clear();
        mSamples.shrink_to_fit();
        int curr_segment = 0;
        for (auto seg : mSegments)
        {
            float t = 0.0f;
            float step = 1.0f / numSamples;
            auto P0 = seg.a();
            auto P1 = seg.b();
            auto P2 = seg.c();
            auto P3 = seg.d();

            for (int i = 0; i < numSamples; i++)
            {
                glm::vec3 sample = p_polinomial(P0, P1, P2, P3, t);
                auto tangent = glm::normalize(p_fderivative(P0, P1, P2, P3, t));
                auto tangent2 = glm::normalize(p_fderivative(P0, P1, P2, P3, t + 0.0001));

                auto c = glm::normalize(glm::cross(tangent2, tangent));

                auto normal = glm::normalize(glm::cross(c, tangent));

                std::cout << "sample: " << sample.x << ", " << sample.y << ", " << sample.z << std::endl;

                mSamples.push_back({sample, tangent, normal});
                t += step;
            }
            curr_segment++;
        }
        mNumSamples = mSamples.size();
    }

    glm::vec3 CubicBezier::P_lerp(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t)
    {
        glm::vec3 a_b = interpolate(P0.position, P1.position, t);
        glm::vec3 b_c = interpolate(P1.position, P2.position, t);
        glm::vec3 c_d = interpolate(P2.position, P3.position, t);

        glm::vec3 ab_bc = interpolate(a_b, b_c, t);
        glm::vec3 bc_cd = interpolate(b_c, c_d, t);

        return interpolate(ab_bc, bc_cd, t);
    }

    glm::vec3 CubicBezier::p_polinomial(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t)
    {
        return P0.position * (std::pow(1.0f - t, 3.0f)) +
               P1.position * (3.0f * t * std::pow(1.0f - t, 2.0f)) +
               P2.position * (3.0f * std::pow(t, 2.0f) * (1.0f - t)) +
               P3.position * (std::pow(t, 3.0f));
    }

    glm::vec3 CubicBezier::interpolate(glm::vec3 a, glm::vec3 b, float t)
    {
        return (1.0f - t) * a + t * b;
    }

    void CubicBezier::addSegment(ControlPoint p0, ControlPoint p1, ControlPoint p2, ControlPoint p3)
    {
        BezierSegment segment;

        mPoints.push_back(p0);
        mPoints.push_back(p1);
        mPoints.push_back(p2);
        mPoints.push_back(p3);

        segment.set(&mPoints[mNumSegments * 4], 0);
        segment.set(&mPoints[mNumSegments * 4 + 1], 1);
        segment.set(&mPoints[mNumSegments * 4 + 2], 2);
        segment.set(&mPoints[mNumSegments * 4 + 3], 3);

        segment.index = mNumSegments;

        mSegments.push_back(segment);
        mNumSegments++;
    }

    void CubicBezier::concatenateSegment(ControlPoint p2, ControlPoint p3)
    {

        BezierSegment segment;

        segment.set(&mPoints.back(), 0);

        mPoints.push_back(mPoints.back().position + (mPoints.back().position - mPoints[mPoints.size() - 2].position));
        mPoints.push_back(p2);
        mPoints.push_back(p3);

        segment.set(&mPoints[mNumSegments * 4], 1);
        segment.set(&mPoints[mNumSegments * 4 + 1], 2);
        segment.set(&mPoints[mNumSegments * 4 + 2], 3);

        segment.index = mNumSegments;

        mSegments.push_back(segment);
        mNumSegments++;
    }

}