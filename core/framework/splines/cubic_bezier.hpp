#pragma once

#include "spline.hpp"

namespace framework::splines
{
    class CubicBezier : public Spline
    {
        using BezierSegment = CurveSegment<4>;

    public:
        CubicBezier() : Bezier<4>()
        {
            type = DrawableType::CUBIC_BEZIER;
            mPoints.reserve(12);
            addInitialSegment();
            createSamplePoints(mNumSamplesPerSegment);
            createGeometry();
        }

        virtual void Update() override {}

        std::vector<ControlPoint> getSamples();

        glm::vec3 p_fderivative(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);
        
        glm::vec3 p_sderivative(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);
        
        uint32_t numSamples();

        uint32_t numSamplesPerSegment();
    private:
        void addInitialSegment();

        void createGeometry();
        
        void updateGeometry();

        void createSamplePoints(int numSamples);

        glm::vec3 P_lerp(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);

        glm::vec3 p_polinomial(ControlPoint P0, ControlPoint P1, ControlPoint P2, ControlPoint P3, float t);

        glm::vec3 interpolate(glm::vec3 a, glm::vec3 b, float t);
        
        void addSegment(ControlPoint p0, ControlPoint p1, ControlPoint p2, ControlPoint p3);

        void concatenateSegment(ControlPoint p2, ControlPoint p3);


    private:
        std::vector<ControlPoint> mSamples;
        uint32_t mNumSamples = 0;
        const uint32_t mNumSamplesPerSegment = 30;
    };
};