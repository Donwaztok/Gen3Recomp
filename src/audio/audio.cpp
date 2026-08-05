#include "audio.hpp"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

namespace gen3recomp {

struct AudioDevice::Impl {
    SDL_AudioStream* stream = nullptr;
    SDL_AudioDeviceID device = 0;
};

bool AudioDevice::init(int sample_rate) {
    shutdown();
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        spdlog::error("host audio init failed: {}", SDL_GetError());
        return false;
    }

    impl_ = new Impl();
    const SDL_AudioSpec spec{
        .format = SDL_AUDIO_S16,
        .channels = 1,
        .freq = sample_rate,
    };
    impl_->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (impl_->stream == nullptr) {
        spdlog::error("host audio device unavailable: {}", SDL_GetError());
        shutdown();
        return false;
    }
    impl_->device = SDL_GetAudioStreamDevice(impl_->stream);
    if (!SDL_ResumeAudioDevice(impl_->device)) {
        spdlog::error("host audio resume failed: {}", SDL_GetError());
        shutdown();
        return false;
    }
    available_ = true;
    return true;
}

void AudioDevice::shutdown() {
    available_ = false;
    if (impl_ == nullptr) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    if (impl_->stream != nullptr) {
        SDL_DestroyAudioStream(impl_->stream);
    }
    delete impl_;
    impl_ = nullptr;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioDevice::queue(std::span<const std::int16_t> samples) {
    if (!available_ || impl_ == nullptr || impl_->stream == nullptr || samples.empty()) {
        return;
    }
    SDL_PutAudioStreamData(
        impl_->stream,
        samples.data(),
        static_cast<int>(samples.size_bytes()));
}

}
