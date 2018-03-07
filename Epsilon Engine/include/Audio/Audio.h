#pragma once
#include <vector>
#include <memory>
#include <Audio/AudioElement.h>


namespace Audio {

	class Audio {
		public:
			Audio();
			~Audio();

		public:

			void addAudioElement(std::shared_ptr<AudioElement> element);

			void PlayAudio();

			bool setMasterVolume(float);
			bool setMusicVolume(float);
			bool setGameVolume(float);

		private:
			float m_MasterVolume;
			float m_MusicVolume;
			float m_GameVolume;


			ALCdevice* device;
			ALCcontext* context;

			std::vector<std::shared_ptr<AudioElement>> m_AudioElementsCollection;
	};
}
