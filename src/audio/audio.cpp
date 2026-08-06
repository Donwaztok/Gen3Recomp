#include "audio.hpp"
#include "sdl3_dyn.hpp"

#include <spdlog/spdlog.h>

#include <string>

namespace gen3recomp {

struct AudioDevice::Impl {
    SDL_AudioStream* stream = nullptr;
    SDL_AudioDeviceID device = 0;
};

bool AudioDevice::init(int sample_rate) {
    shutdown();
    std::string error;
    if (!sdl3::load(error)) {
        spdlog::error("host audio SDL3 load failed: {}", error);
        return false;
    }
    if (!sdl3::InitSubSystem(SDL_INIT_AUDIO)) {
        spdlog::error("host audio init failed: {}", sdl3::GetError());
        return false;
    }

    impl_ = new Impl();
    const SDL_AudioSpec spec{
        .format = SDL_AUDIO_S16,
        .channels = 1,
        .freq = sample_rate,
    };
    impl_->stream = sdl3::OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (impl_->stream == nullptr) {
        spdlog::error("host audio device unavailable: {}", sdl3::GetError());
        shutdown();
        return false;
    }
    impl_->device = sdl3::GetAudioStreamDevice(impl_->stream);
    if (!sdl3::ResumeAudioDevice(impl_->device)) {
        spdlog::error("host audio resume failed: {}", sdl3::GetError());
        shutdown();
        return false;
    }
    available_ = true;
    return true;
}

void AudioDevice::shutdown() {
    available_ = false;
    if (sdl3::QuitSubSystem == nullptr) {
        delete impl_;
        impl_ = nullptr;
        return;
    }
    if (impl_ == nullptr) {
        sdl3::QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    if (impl_->stream != nullptr) {
        sdl3::DestroyAudioStream(impl_->stream);
    }
    delete impl_;
    impl_ = nullptr;
    sdl3::QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioDevice::queue(std::span<const std::int16_t> samples) {
    if (!available_ || impl_ == nullptr || impl_->stream == nullptr || samples.empty()) {
        return;
    }
    sdl3::PutAudioStreamData(
        impl_->stream,
        samples.data(),
        static_cast<int>(samples.size_bytes()));
}

}
