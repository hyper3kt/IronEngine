#include "iron/base/input_manager.hpp"

#include <SDL3/SDL.h>

using namespace Iron;

void InputManager::PollEvents() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            // TODO
        }
    }
}