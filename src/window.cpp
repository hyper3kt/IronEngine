#include "engine.h"
#include "window.h"

#include <SDL3/SDL.h>

using namespace Iron;

Window::Window() {
    window = SDL_CreateWindow(Engine::GetGameName().c_str(), 640, 480, SDL_WINDOW_OPENGL);
    if(!window) return; // TODO: impl bug reports
}

void Window::PollEvents() {
    
}

Window::~Window() {
    SDL_DestroyWindow(window);
}

bool Window::InitSystem() {
    return SDL_Init(SDL_INIT_VIDEO);
}

void Window::KillSystem() {
    SDL_Quit();
}