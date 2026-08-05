#include "platform.hpp"

#include <SDL3/SDL.h>

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
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }

    impl_ = new Impl();
    impl_->window = SDL_CreateWindow(title, window_width, window_height, 0);
    if (impl_->window == nullptr) {
        shutdown();
        return false;
    }

    impl_->renderer = SDL_CreateRenderer(impl_->window, nullptr);
    if (impl_->renderer == nullptr) {
        shutdown();
        return false;
    }

    SDL_SetRenderLogicalPresentation(impl_->renderer, 240, 160, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    keys_ = {};
    return true;
}

void Platform::shutdown() {
    keys_ = {};
    if (impl_ == nullptr) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return;
    }
    if (impl_->texture != nullptr) {
        SDL_DestroyTexture(impl_->texture);
    }
    if (impl_->renderer != nullptr) {
        SDL_DestroyRenderer(impl_->renderer);
    }
    if (impl_->window != nullptr) {
        SDL_DestroyWindow(impl_->window);
    }
    delete impl_;
    impl_ = nullptr;
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

bool Platform::poll_quit() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
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
            SDL_DestroyTexture(impl_->texture);
        }
        impl_->texture = SDL_CreateTexture(
            impl_->renderer,
            SDL_PIXELFORMAT_XRGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height);
        if (impl_->texture == nullptr) {
            return false;
        }
        SDL_SetTextureScaleMode(impl_->texture, SDL_SCALEMODE_NEAREST);
        impl_->texture_width = width;
        impl_->texture_height = height;
    }

    if (!SDL_UpdateTexture(impl_->texture, nullptr, pixels, width * static_cast<int>(sizeof(std::uint32_t)))) {
        return false;
    }
    if (!SDL_RenderClear(impl_->renderer)) {
        return false;
    }
    if (!SDL_RenderTexture(impl_->renderer, impl_->texture, nullptr, nullptr)) {
        return false;
    }
    return SDL_RenderPresent(impl_->renderer);
}

void Platform::delay_ms(std::uint32_t milliseconds) {
    SDL_Delay(milliseconds);
}

const HostKeys& Platform::keys() const {
    return keys_;
}

}
