#pragma once

#include <iostream>

typedef struct SDL_Window;

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

        public:
        Window(int width, int height);
        Window(int width, int height, std::string name);
        
        static bool InitSystem();
        static bool AttemptLoadVulkan();
        static bool AttemptLoadOpenGL();
        static void KillSystem();
        static void SetWindowMetadata(WindowSystemMetadata wsm);
        static WindowSystemMetadata GetWindowMetadata();

        void PollEvents();
        void Kill();

    };

}