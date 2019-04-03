#pragma once
#include <PostProcess/Effect.h>

namespace PostProcess {

    class MotionBlur : public Effect{

    public:
        MotionBlur(int w, int h);

        ~MotionBlur() {}

        virtual GLuint Apply(GLuint, GLuint, double);

        virtual void Update(){}

    private:
        bool m_Activated;
        float m_Intesity;
    };
}

