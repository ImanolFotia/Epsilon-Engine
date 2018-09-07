#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <Types.h>
#include <Audio/Loaders/WAVFile.h>
#include <iostream>

namespace IO {
    namespace Audio {
        class AudioElement {
        public:
            AudioElement(const char* fileName, AUDIO_TYPE type, glm::vec3 AudioPosition, glm::vec3 AudioDirection) {
                try {
                    this->m_Type = type;
                    this->m_Position = AudioPosition;
                    this->m_Direction = AudioDirection;

                    IO::Audio::File::WAVfile SoundFile;
                    data = (std::shared_ptr<char>) SoundFile.Load(fileName);

                    alGenBuffers(1,&this->m_BufferID);

                    this->m_Channels = SoundFile.getNumberOfChannels();
                    std::cout << "Number of Channels: " << this->m_Channels << std::endl;

                    this->m_BytesPerSecond = SoundFile.getBPS();
                    std::cout << "Bytes per Second: " << this->m_BytesPerSecond << std::endl;


                    if(this->m_Channels==1) {
                        if(this->m_BytesPerSecond==8) {
                            this->m_Format = AL_FORMAT_MONO8;
                        } else {
                            this->m_Format = AL_FORMAT_MONO16;
                        }
                    } else {
                        if(this->m_BytesPerSecond==8) {
                            this->m_Format = AL_FORMAT_STEREO8;
                        } else {
                            this->m_Format = AL_FORMAT_STEREO16;
                        }
                    }

                    alBufferData(this->m_BufferID, this->m_Format, data.get(), SoundFile.getFileSize(), SoundFile.getSampleRate());

                    alGenSources(1,&this->m_AudioID);

                    if(this->m_Type == MUSIC) {
                        alSourcei(m_AudioID, AL_SOURCE_RELATIVE, AL_TRUE);
                        alSourcef(m_AudioID,AL_GAIN,1);
                        alSourcef(m_AudioID,AL_PITCH,1);
                        alSource3f(m_AudioID,AL_POSITION, 0,0,0);
                        alSource3f(m_AudioID, AL_DIRECTION, 0,0,0);
                        alSourcef(m_AudioID,AL_CONE_INNER_ANGLE,360);
                        alSourcef(m_AudioID,AL_CONE_OUTER_ANGLE,360);
                        alSource3f(m_AudioID, AL_VELOCITY, 0.0,0.0,0.0);
                    } else if(this->m_Type == STATIC_SOUND) {

                        alSourcei(m_AudioID, AL_SOURCE_RELATIVE, AL_FALSE);
                        alSourcef(m_AudioID,AL_GAIN,1);
                        alSourcef(m_AudioID,AL_PITCH,1);
                        alSource3f(m_AudioID,AL_POSITION, m_Position.x, m_Position.y, m_Position.z);
                        alSource3f(m_AudioID, AL_DIRECTION, m_Direction.x, m_Direction.y, m_Direction.z);
                        alSourcef(m_AudioID,AL_CONE_INNER_ANGLE,360);
                        alSourcef(m_AudioID,AL_CONE_OUTER_ANGLE,360);
                        alSource3f(m_AudioID, AL_VELOCITY, 0.0,0.0,0.0);
                    }

                    alSourcei(m_AudioID,AL_BUFFER,m_BufferID);
                } catch(...) {
                    throw;
                }

            }

            void Destroy() {
                std::cout << "Deleted AudioElement" << std::endl;
                //char* tmpData = data.reset();
                //delete tmpData;
                //tmpData = nullptr;
                alDeleteSources(1, &m_AudioID);
                alDeleteBuffers(1, &m_BufferID);

            }

            ~AudioElement() {
            }

        public:

            void Play() {
                ALint source_state;
                alGetSourcei(m_AudioID, AL_SOURCE_STATE, &source_state);

                if(source_state != AL_PLAYING) {


                    if(this->m_Type == DYNAMIC_SOUND) {
                        alSourcei(m_AudioID, AL_LOOPING,AL_TRUE);
                        alSourcei(m_AudioID, AL_SOURCE_RELATIVE, AL_FALSE);
                        alSourcef(m_AudioID,AL_GAIN,1);
                        alSourcef(m_AudioID,AL_PITCH,1);
                        alSource3f(m_AudioID,AL_POSITION, m_Position.x, m_Position.y, m_Position.z);
                        alSource3f(m_AudioID, AL_DIRECTION, m_Direction.x, m_Direction.y, m_Direction.z);
                        alSourcef(m_AudioID,AL_CONE_INNER_ANGLE,360);
                        alSourcef(m_AudioID,AL_CONE_OUTER_ANGLE,360);
                        alSource3f(m_AudioID, AL_VELOCITY, 0.0,0.0,0.0);
                        alSourcePlay(m_AudioID);
                    } else if(this->m_Type == STATIC_SOUND) {
                        alSourcei(m_AudioID, AL_LOOPING,AL_TRUE);
                        alSourcePlay(m_AudioID);
                    } else if(this->m_Type == MUSIC) {
                        alSourcei(m_AudioID, AL_LOOPING,AL_TRUE);
                        alSourcePlay(m_AudioID);
                    }

                }

                if(this->m_Type == MENU_SOUND) {
                    //if(source_state != AL_PLAYING){
                    alSourcef(m_AudioID,AL_GAIN,0);
                    alSourceStop(m_AudioID);
                    //}
                    //else{
                    alSourcei(m_AudioID, AL_LOOPING,AL_FALSE);
                    alSourcePlay(m_AudioID);
                    alSourcef(m_AudioID,AL_GAIN,1);
                    //}

                }
            }

            AUDIO_TYPE getType() {
                return m_Type;
            }

        private:

            unsigned int m_AudioID;
            unsigned int m_BufferID;
            float m_Volume;
            int m_Channels;
            unsigned int m_Format;
            int m_BytesPerSecond;
            std::shared_ptr<char> data;

            AUDIO_TYPE m_Type;

            glm::vec3 m_Position;
            glm::vec3 m_Direction;
            glm::vec3 m_Velocity;
            float m_Radius;
        };
    }
}
