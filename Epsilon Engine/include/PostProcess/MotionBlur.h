#pragma once
#include <PostProcess/Effect.h>

namespace PostProcess {
    class MotionBlur : public Effect
    {
    public:
        MotionBlur(int, int);
        ~MotionBlur() {}

        virtual void ApplyEffect(GLuint, GLuint, double);

        void Update(){}

    private:
        bool m_Activated;
        float m_Intesity;
    };
}

