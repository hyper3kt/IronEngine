#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"

#include <SDL3/SDL.h>

using namespace Iron;

WindowSystemMetadata Window::wsm = WindowSystemMetadata();

Window::Window() {
    window = SDL_CreateWindow(Engine::GetGameName().c_str(), 640, 480, SDL_WINDOW_OPENGL);
    if(!window) return; // TODO: impl bug reports
}

void Window::PollEvents() {
    
}

void Window::Kill() {
    SDL_DestroyWindow(window);
}

bool Window::InitSystem() {
    return SDL_Init(SDL_INIT_VIDEO);
}

void Window::KillSystem() {
    SDL_Quit();
}

void Window::SetWindowMetadata(WindowSystemMetadata wsm) {
    Window::wsm = wsm;

    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, wsm.name.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, wsm.version.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, wsm.appId.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, wsm.developer.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, wsm.copyright.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");
}

WindowSystemMetadata Window::GetWindowMetadata() {
    return wsm;
}