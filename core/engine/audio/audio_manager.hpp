#pragma once
#include "../object_pool.hpp"

namespace engine::audio {
	class AudioSource;
	class AudioBuffer;
	class AudioListener;

	struct BufferInfo {
		int numChannels{};
		std::size_t size{};
		std::size_t bps{};
		std::size_t bitrate{};
		unsigned char* data = nullptr;
	};

	class AudioManager {
	public:
		virtual void Init() = 0;
		virtual void CleanUp() = 0;

		virtual Ref<AudioBuffer> createBuffer(const std::string&, const BufferInfo&) = 0;
		virtual Ref<AudioSource> createSource(const std::string&) = 0;
		virtual Ref<AudioListener> createListener(const std::string&) = 0;


		virtual void deleteBuffer(Ref<AudioBuffer>) = 0;
		virtual void deleteSource(Ref<AudioSource>) = 0;
		virtual void deleteListener(Ref<AudioListener>) = 0;
	};
}