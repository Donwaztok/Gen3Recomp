#include "sdl3_dyn.hpp"

#include <string>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace gen3recomp::sdl3 {
namespace {

void* g_lib = nullptr;

template <typename T>
bool load_sym(T& out, const char* name, std::string& error) {
#if defined(_WIN32)
    FARPROC sym = GetProcAddress(static_cast<HMODULE>(g_lib), name);
    if (sym == nullptr) {
        error = std::string("SDL3 missing symbol ") + name;
        return false;
    }
    out = reinterpret_cast<T>(sym);
    return true;
#else
    dlerror();
    void* sym = dlsym(g_lib, name);
    const char* err = dlerror();
    if (sym == nullptr) {
        error = std::string("SDL3 missing symbol ") + name;
        if (err != nullptr && *err != '\0') {
            error += ": ";
            error += err;
        }
        return false;
    }
    out = reinterpret_cast<T>(sym);
    return true;
#endif
}

}  // namespace

bool (*Init)(SDL_InitFlags flags) = nullptr;
void (*QuitSubSystem)(SDL_InitFlags flags) = nullptr;
const char* (*GetError)(void) = nullptr;
SDL_Window* (*CreateWindow)(const char* title, int w, int h, SDL_WindowFlags flags) = nullptr;
void (*DestroyWindow)(SDL_Window* window) = nullptr;
bool (*GetWindowSize)(SDL_Window* window, int* w, int* h) = nullptr;
SDL_Renderer* (*CreateRenderer)(SDL_Window* window, const char* name) = nullptr;
void (*DestroyRenderer)(SDL_Renderer* renderer) = nullptr;
bool (*SetRenderDrawColor)(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = nullptr;
bool (*RenderClear)(SDL_Renderer* renderer) = nullptr;
bool (*RenderPresent)(SDL_Renderer* renderer) = nullptr;
bool (*RenderDebugText)(SDL_Renderer* renderer, float x, float y, const char* str) = nullptr;
bool (*RenderFillRect)(SDL_Renderer* renderer, const SDL_FRect* rect) = nullptr;
bool (*RenderRect)(SDL_Renderer* renderer, const SDL_FRect* rect) = nullptr;
bool (*SetRenderLogicalPresentation)(
    SDL_Renderer* renderer,
    int w,
    int h,
    SDL_RendererLogicalPresentation mode) = nullptr;
SDL_Texture* (*CreateTexture)(
    SDL_Renderer* renderer,
    SDL_PixelFormat format,
    SDL_TextureAccess access,
    int w,
    int h) = nullptr;
void (*DestroyTexture)(SDL_Texture* texture) = nullptr;
bool (*SetTextureScaleMode)(SDL_Texture* texture, SDL_ScaleMode scaleMode) = nullptr;
bool (*UpdateTexture)(SDL_Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch) =
    nullptr;
bool (*RenderTexture)(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    const SDL_FRect* srcrect,
    const SDL_FRect* dstrect) = nullptr;
bool (*PollEvent)(SDL_Event* event) = nullptr;
void (*Delay)(Uint32 ms) = nullptr;
SDL_MouseButtonFlags (*GetMouseState)(float* x, float* y) = nullptr;
void (*ShowOpenFileDialog)(
    SDL_DialogFileCallback callback,
    void* userdata,
    SDL_Window* window,
    const SDL_DialogFileFilter* filters,
    int nfilters,
    const char* default_location,
    bool allow_many) = nullptr;
bool (*InitSubSystem)(SDL_InitFlags flags) = nullptr;
SDL_AudioStream* (*OpenAudioDeviceStream)(
    SDL_AudioDeviceID devid,
    const SDL_AudioSpec* spec,
    SDL_AudioStreamCallback callback,
    void* userdata) = nullptr;
SDL_AudioDeviceID (*GetAudioStreamDevice)(SDL_AudioStream* stream) = nullptr;
bool (*ResumeAudioDevice)(SDL_AudioDeviceID devid) = nullptr;
void (*DestroyAudioStream)(SDL_AudioStream* stream) = nullptr;
bool (*PutAudioStreamData)(SDL_AudioStream* stream, const void* buf, int len) = nullptr;
bool (*SetHint)(const char* name, const char* value) = nullptr;

void* lib() {
    return g_lib;
}

bool load(std::string& error) {
    if (g_lib != nullptr) {
        return true;
    }
#if defined(_WIN32)
    g_lib = LoadLibraryA("SDL3.dll");
    if (g_lib == nullptr) {
        error = "failed to LoadLibrary SDL3.dll";
        return false;
    }
#else
    dlerror();
#if defined(__linux__)
    // Prefer DEEPBIND on Linux so SDL3 symbols do not collide with SDL2
    // already mapped by gba-recomp. macOS has no RTLD_DEEPBIND.
    g_lib = dlopen("libSDL3.so.0", RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
    if (g_lib == nullptr) {
        g_lib = dlopen("libSDL3.so.0", RTLD_NOW | RTLD_LOCAL);
    }
#else
    g_lib = dlopen("libSDL3.dylib", RTLD_NOW | RTLD_LOCAL);
    if (g_lib == nullptr) {
        g_lib = dlopen("libSDL3.so.0", RTLD_NOW | RTLD_LOCAL);
    }
#endif
    if (g_lib == nullptr) {
        const char* detail = dlerror();
        error = std::string("failed to dlopen libSDL3");
        if (detail != nullptr) {
            error += ": ";
            error += detail;
        }
        return false;
    }
#endif

#define GEN3_SDL3_SYM(ptr, name)       \
    if (!load_sym(ptr, name, error)) { \
        return false;                  \
    }

    GEN3_SDL3_SYM(Init, "SDL_Init");
    GEN3_SDL3_SYM(QuitSubSystem, "SDL_QuitSubSystem");
    GEN3_SDL3_SYM(GetError, "SDL_GetError");
    GEN3_SDL3_SYM(CreateWindow, "SDL_CreateWindow");
    GEN3_SDL3_SYM(DestroyWindow, "SDL_DestroyWindow");
    GEN3_SDL3_SYM(GetWindowSize, "SDL_GetWindowSize");
    GEN3_SDL3_SYM(CreateRenderer, "SDL_CreateRenderer");
    GEN3_SDL3_SYM(DestroyRenderer, "SDL_DestroyRenderer");
    GEN3_SDL3_SYM(SetRenderDrawColor, "SDL_SetRenderDrawColor");
    GEN3_SDL3_SYM(RenderClear, "SDL_RenderClear");
    GEN3_SDL3_SYM(RenderPresent, "SDL_RenderPresent");
    GEN3_SDL3_SYM(RenderDebugText, "SDL_RenderDebugText");
    GEN3_SDL3_SYM(RenderFillRect, "SDL_RenderFillRect");
    GEN3_SDL3_SYM(RenderRect, "SDL_RenderRect");
    GEN3_SDL3_SYM(SetRenderLogicalPresentation, "SDL_SetRenderLogicalPresentation");
    GEN3_SDL3_SYM(CreateTexture, "SDL_CreateTexture");
    GEN3_SDL3_SYM(DestroyTexture, "SDL_DestroyTexture");
    GEN3_SDL3_SYM(SetTextureScaleMode, "SDL_SetTextureScaleMode");
    GEN3_SDL3_SYM(UpdateTexture, "SDL_UpdateTexture");
    GEN3_SDL3_SYM(RenderTexture, "SDL_RenderTexture");
    GEN3_SDL3_SYM(PollEvent, "SDL_PollEvent");
    GEN3_SDL3_SYM(Delay, "SDL_Delay");
    GEN3_SDL3_SYM(GetMouseState, "SDL_GetMouseState");
    GEN3_SDL3_SYM(ShowOpenFileDialog, "SDL_ShowOpenFileDialog");
    GEN3_SDL3_SYM(InitSubSystem, "SDL_InitSubSystem");
    GEN3_SDL3_SYM(OpenAudioDeviceStream, "SDL_OpenAudioDeviceStream");
    GEN3_SDL3_SYM(GetAudioStreamDevice, "SDL_GetAudioStreamDevice");
    GEN3_SDL3_SYM(ResumeAudioDevice, "SDL_ResumeAudioDevice");
    GEN3_SDL3_SYM(DestroyAudioStream, "SDL_DestroyAudioStream");
    GEN3_SDL3_SYM(PutAudioStreamData, "SDL_PutAudioStreamData");
    GEN3_SDL3_SYM(SetHint, "SDL_SetHint");
#undef GEN3_SDL3_SYM
    return true;
}

}  // namespace gen3recomp::sdl3
