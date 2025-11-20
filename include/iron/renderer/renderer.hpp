#pragma once

#include "iron/world/camera.hpp"

namespace Iron {

    class Renderer {

        Camera* camera;

        public:

        void SetCamera(Camera* camera);
        Camera* GetCamera();

        virtual void Tick() = 0;

    };

}