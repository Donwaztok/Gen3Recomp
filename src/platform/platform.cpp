#include "platform.hpp"
#include "sdl3_dyn.hpp"

#include <string>

namespace gen3recomp {

struct Platform::Impl {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    int texture_width = 0;
    int texture_height = 0;
};

namespace {

void apply_key(HostKeys& keys, SDL_Keycode key, bool down) {
    switch (key) {
    case SDLK_UP:
        keys.up = down;
        break;
    case SDLK_DOWN:
        keys.down = down;
        break;
    case SDLK_LEFT:
        keys.left = down;
        break;
    case SDLK_RIGHT:
        keys.right = down;
        break;
    case SDLK_X:
        keys.a = down;
        break;
    case SDLK_Z:
        keys.b = down;
        break;
    case SDLK_RETURN:
        keys.start = down;
        break;
    case SDLK_RSHIFT:
        keys.select = down;
        break;
    case SDLK_C:
        keys.l = down;
        break;
    case SDLK_V:
        keys.r = down;
        break;
    case SDLK_ESCAPE:
    case SDLK_Q:
        keys.quit = down;
        break;
    default:
        break;
    }
}

}  // namespace

bool Platform::init(const char* title, int window_width, int window_height) {
    shutdown();
    std::string error;
    if (!sdl3::load(error) || !sdl3::Init(SDL_INIT_VIDEO)) {
        return false;
    }

    impl_ = new Impl();
    impl_->window = sdl3::CreateWindow(title, window_width, window_height, 0);
    if (impl_->window == nullptr) {
        shutdown();
        return false;
    }

    impl_->renderer = sdl3::CreateRenderer(impl_->window, "software");
    if (impl_->renderer == nullptr) {
        impl_->renderer = sdl3::CreateRenderer(impl_->window, nullptr);
    }
    if (impl_->renderer == nullptr) {
        shutdown();
        return false;
    }

    sdl3::SetRenderLogicalPresentation(impl_->renderer, 240, 160, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    keys_ = {};
    return true;
}

void Platform::shutdown() {
    keys_ = {};
    if (sdl3::QuitSubSystem == nullptr) {
        delete impl_;
        impl_ = nullptr;
        return;
    }
    if (impl_ == nullptr) {
        sdl3::QuitSubSystem(SDL_INIT_VIDEO);
        return;
    }
    if (impl_->texture != nullptr) {
        sdl3::DestroyTexture(impl_->texture);
    }
    if (impl_->renderer != nullptr) {
        sdl3::DestroyRenderer(impl_->renderer);
    }
    if (impl_->window != nullptr) {
        sdl3::DestroyWindow(impl_->window);
    }
    delete impl_;
    impl_ = nullptr;
    sdl3::QuitSubSystem(SDL_INIT_VIDEO);
}

bool Platform::poll_quit() {
    SDL_Event event;
    while (sdl3::PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return true;
        }
        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            apply_key(keys_, event.key.key, event.type == SDL_EVENT_KEY_DOWN);
            if (keys_.quit) {
                return true;
            }
        }
    }
    return false;
}

bool Platform::present_rgba32(const std::uint32_t* pixels, int width, int height) {
    if (impl_ == nullptr || impl_->renderer == nullptr || pixels == nullptr || width <= 0 || height <= 0) {
        return false;
    }

    if (impl_->texture == nullptr || impl_->texture_width != width || impl_->texture_height != height) {
        if (impl_->texture != nullptr) {
            sdl3::DestroyTexture(impl_->texture);
        }
        impl_->texture = sdl3::CreateTexture(
            impl_->renderer,
            SDL_PIXELFORMAT_XRGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height);
        if (impl_->texture == nullptr) {
            return false;
        }
        sdl3::SetTextureScaleMode(impl_->texture, SDL_SCALEMODE_NEAREST);
        impl_->texture_width = width;
        impl_->texture_height = height;
    }

    if (!sdl3::UpdateTexture(impl_->texture, nullptr, pixels, width * static_cast<int>(sizeof(std::uint32_t)))) {
        return false;
    }
    if (!sdl3::RenderClear(impl_->renderer)) {
        return false;
    }
    if (!sdl3::RenderTexture(impl_->renderer, impl_->texture, nullptr, nullptr)) {
        return false;
    }
    return sdl3::RenderPresent(impl_->renderer);
}

void Platform::delay_ms(std::uint32_t milliseconds) {
    if (sdl3::Delay != nullptr) {
        sdl3::Delay(milliseconds);
    }
}

const HostKeys& Platform::keys() const {
    return keys_;
}

}
