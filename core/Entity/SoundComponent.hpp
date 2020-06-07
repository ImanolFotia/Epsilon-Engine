#pragma once

#include <Entity/Component.h>
#include <Audio/Audio.h>
#include <Audio/AudioElement.h>
#include <Types.h>
namespace Epsilon
{
    namespace Component
    {

        /// Rendering Components
        class SoundComponent : public Component
        {
        public:
            SoundComponent(const char *fileName, AUDIO_TYPE type, glm::vec3 AudioPosition, glm::vec3 AudioDirection)
            {
                std::cout << "creating sound component" << std::endl;
                mType = SOUNDCOMPONENT;
                mPosition = AudioPosition;
                mRotation = glm::quat(AudioDirection);
                mAudioElement = std::make_shared<IO::Audio::AudioElement>(fileName, type, AudioPosition, AudioDirection);
                mState = AUDIO_STATE::PLAYING;
            }

            COMPONENT_TYPE getType() { return mType; }

            void setState(AUDIO_STATE s) { mState = s; }

            ~SoundComponent()
            {
                std::cout << "SoundComponent Destructor" << std::endl;
            }

            void setPosition(glm::vec3 position)
            {
                mPosition = position;
            }

            glm::vec3 getPosition()
            {
                return mPosition;
            }

            glm::quat getRotation()
            {
                return mRotation;
            }

            glm::vec3 getScale()
            {
                return glm::vec3(1.0);
            }

        public:
            void Update()
            {
                mAudioElement->setPosition(mPosition);
            }

            void Render()
            {
            }

        private:
            AUDIO_STATE mState;
            COMPONENT_TYPE mType;
            std::shared_ptr<IO::Audio::AudioElement> mAudioElement;
            glm::vec3 mPosition;
            glm::quat mRotation;
        };

        using SoundComponent_ptr = std::shared_ptr<SoundComponent>;
    } // namespace Component
} // namespace Epsilon