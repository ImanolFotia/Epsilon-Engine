#pragma once

#include <PostProcess/Effect.h>
namespace PostProcess {
    class ScreenSpaceReflections : public Effect
    {
    public:
        ScreenSpaceReflections(int, int);

        ~ScreenSpaceReflections(){}

        virtual GLuint Apply(GLuint, GLuint, GLuint, GLuint, GLuint,glm::mat4, glm::mat4);

        virtual void Update(){}

        /**----------------------------------------*/
        /**           SSR Specific Code           */
        /**----------------------------------------*/

        void PrepareSSRTexture();
    };
}
