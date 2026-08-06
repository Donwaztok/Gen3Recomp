#pragma once

// Resolve SDL3 entry points via dlopen/LoadLibrary so they are never bound
// to the submodule's SDL2 (same symbol names, incompatible return values).

#include <SDL3/SDL.h>

#include <string>

namespace gen3recomp::sdl3 {

bool load(std::string& error);
void* lib();

extern bool (*Init)(SDL_InitFlags flags);
extern void (*QuitSubSystem)(SDL_InitFlags flags);
extern const char* (*GetError)(void);
extern SDL_Window* (*CreateWindow)(const char* title, int w, int h, SDL_WindowFlags flags);
extern void (*DestroyWindow)(SDL_Window* window);
extern bool (*GetWindowSize)(SDL_Window* window, int* w, int* h);
extern SDL_Renderer* (*CreateRenderer)(SDL_Window* window, const char* name);
extern void (*DestroyRenderer)(SDL_Renderer* renderer);
extern bool (*SetRenderDrawColor)(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
extern bool (*RenderClear)(SDL_Renderer* renderer);
extern bool (*RenderPresent)(SDL_Renderer* renderer);
extern bool (*RenderDebugText)(SDL_Renderer* renderer, float x, float y, const char* str);
extern bool (*RenderFillRect)(SDL_Renderer* renderer, const SDL_FRect* rect);
extern bool (*RenderRect)(SDL_Renderer* renderer, const SDL_FRect* rect);
extern bool (*SetRenderLogicalPresentation)(
    SDL_Renderer* renderer,
    int w,
    int h,
    SDL_RendererLogicalPresentation mode);
extern SDL_Texture* (*CreateTexture)(
    SDL_Renderer* renderer,
    SDL_PixelFormat format,
    SDL_TextureAccess access,
    int w,
    int h);
extern void (*DestroyTexture)(SDL_Texture* texture);
extern bool (*SetTextureScaleMode)(SDL_Texture* texture, SDL_ScaleMode scaleMode);
extern bool (*UpdateTexture)(SDL_Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch);
extern bool (*RenderTexture)(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    const SDL_FRect* srcrect,
    const SDL_FRect* dstrect);
extern bool (*PollEvent)(SDL_Event* event);
extern void (*Delay)(Uint32 ms);
extern SDL_MouseButtonFlags (*GetMouseState)(float* x, float* y);
extern void (*ShowOpenFileDialog)(
    SDL_DialogFileCallback callback,
    void* userdata,
    SDL_Window* window,
    const SDL_DialogFileFilter* filters,
    int nfilters,
    const char* default_location,
    bool allow_many);
extern bool (*InitSubSystem)(SDL_InitFlags flags);
extern SDL_AudioStream* (*OpenAudioDeviceStream)(
    SDL_AudioDeviceID devid,
    const SDL_AudioSpec* spec,
    SDL_AudioStreamCallback callback,
    void* userdata);
extern SDL_AudioDeviceID (*GetAudioStreamDevice)(SDL_AudioStream* stream);
extern bool (*ResumeAudioDevice)(SDL_AudioDeviceID devid);
extern void (*DestroyAudioStream)(SDL_AudioStream* stream);
extern bool (*PutAudioStreamData)(SDL_AudioStream* stream, const void* buf, int len);
extern bool (*SetHint)(const char* name, const char* value);

}  // namespace gen3recomp::sdl3
