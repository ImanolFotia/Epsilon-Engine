#pragma once

#include "Bezier.hpp"

namespace Epsilon::Renderer
{
    class CubicBezier : public Bezier<4>
    {
        using BezierSegment = CurveSegment<4>;

    public:
        CubicBezier() : Bezier<4>()
        {
            mPoints.reserve(12);
            addInitialSegment();
            createSamplePoints(mNumSamplesPerSegment);
            createGeometry();
        }

        virtual void Update() override {}

        virtual void Render() override;

        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) override;

        std::vector<ControlPoint> getSamples();

        glm::vec3 p_fderivative(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);
        
        glm::vec3 p_sderivative(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);
        
        uint32_t numSamples();

        uint32_t numSamplesPerSegment();
    private:
        void addInitialSegment();

        void createGeometry();

        void createSamplePoints(int numSamples);

        glm::vec3 P_lerp(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);

        glm::vec3 p_polinomial(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);


        glm::vec3 interpolate(glm::vec3 a, glm::vec3 b, float t);


    private:
        std::vector<ControlPoint> mSamples;
        uint32_t mNumSamples = 0;
        const uint32_t mNumSamplesPerSegment = 30;
    };
};