#pragma once

#include "iron/macros.hpp"

#include <iostream>

namespace Iron {

    class IronDLL Component {

        public:

        virtual void Init();
        virtual void Tick(float dt);
        inline virtual std::string GetComponentName() {
            return "GenericComponent";
        }
    };

}