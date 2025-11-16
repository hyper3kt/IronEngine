#pragma once

#include "iron/renderer/renderer.hpp"

namespace Iron {

    class VulkanRenderer : public Renderer {

        public:

        VulkanRenderer();

        void Tick();

    };

}