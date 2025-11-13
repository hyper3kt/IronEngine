#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

using namespace Iron;

WindowSystemMetadata Window::wsm = WindowSystemMetadata();

Window::Window(int width, int height, std::string name) {
    auto rendererChoice = Engine::ShouldUseVulkan() ? SDL_WINDOW_VULKAN : SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow(name.c_str(), width, height, rendererChoice);
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

bool Window::AttemptLoadVulkan() {
    return SDL_Vulkan_LoadLibrary(NULL);
}

bool Window::AttemptLoadOpenGL() {
    return SDL_GL_LoadLibrary(NULL);
}

void Window::KillSystem() {
    if(Engine::ShouldUseVulkan()) {
        SDL_Vulkan_UnloadLibrary();
    } else SDL_GL_UnloadLibrary();

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