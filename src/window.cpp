#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"

#include <SDL3/SDL_vulkan.h>
#include <glad/glad.h>

using namespace Iron;

WindowSystemMetadata Window::wsm = WindowSystemMetadata();

Window::Window(int width, int height, std::string name) {
    auto rendererChoice = Engine::ShouldUseVulkan() ? SDL_WINDOW_VULKAN : SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow(name.c_str(), width, height, rendererChoice | SDL_WINDOW_RESIZABLE);

    if(!window) {
        status = Failure(SDL_GetError(), IRON_RESULT_FAILED);
        return;
    }

    status = IRON_RESULT_CREATED;

    if(!Engine::ShouldUseVulkan()) {
        glContext = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, glContext);
        SDL_GL_SetSwapInterval(1);

        return;
    }
}

Result<EngineResult> Window::GetStatus() {
    return status;
}

void Window::Kill() {
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
}

bool Window::InitSystem() {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }

    wsm.name = Engine::GetGameName();

    // TODO

    return true;
}

bool Window::AttemptLoadVulkan() {
    return SDL_Vulkan_LoadLibrary(NULL);
}

bool Window::AttemptLoadOpenGL() {
    if(!SDL_GL_LoadLibrary(NULL)) return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

    return true;
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
