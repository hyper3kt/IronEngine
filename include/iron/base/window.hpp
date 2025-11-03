#pragma once

typedef struct SDL_Window;

namespace Iron {

    class Window {

        SDL_Window* window;

        public:
        Window();
        
        static bool InitSystem();
        static void KillSystem();

        void PollEvents();
        void Kill();

    };

}