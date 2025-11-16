#pragma once

#include "iron/result.hpp"

#include <iostream>

struct SDL_Window;
struct SDL_GLContextState* SDL_GLContext;

namespace Iron {

    struct WindowSystemMetadata {
        std::string name = "Iron Engine";
        std::string version = "v1.0.0";
        std::string appId = "com.ironengine.game";
        std::string developer = "Developer";
        std::string copyright = "All rights reserved (c)";
    };

    class Window {

        static WindowSystemMetadata wsm;
        SDL_Window* window;
        SDL_GLContext glContext;
        Result<EngineResult> status = IRON_RESULT_UNINITIALIZED;

        public:
        Window(int width, int height);
        Window(int width, int height, std::string name);
        
        static bool InitSystem();
        static bool AttemptLoadVulkan();
        static bool AttemptLoadOpenGL();
        static void KillSystem();
        static void SetWindowMetadata(WindowSystemMetadata wsm);
        static WindowSystemMetadata GetWindowMetadata();
        
        Result<EngineResult> GetStatus();
        void Kill();

    };

}