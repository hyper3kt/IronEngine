#pragma once

#include <iostream>

namespace Iron {

    class Component {

        public:

        virtual void Init();
        virtual void Tick(float dt);
        inline virtual std::string GetComponentName() {
            return "GenericComponent";
        }
    };

}