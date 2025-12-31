#pragma once

#include "iron/world/object.hpp"

namespace Iron {

    class IronDLL Camera : public Object {

        NumberItem* FOV;

        public:

        void SetFOV(float fov);
        float GetFOV();

        void Init();
        
    };
    
}