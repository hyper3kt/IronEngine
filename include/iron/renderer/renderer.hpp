#pragma once

#include "iron/world/camera.hpp"
#include "iron/macros.hpp"

namespace Iron {

    class IronDLL Renderer {

        Camera* camera;

        public:

        void SetCamera(Camera* camera);
        Camera* GetCamera();

        virtual void Tick() = 0;

    };

}