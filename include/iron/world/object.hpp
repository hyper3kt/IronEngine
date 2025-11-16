#pragma once

#include "iron/base/component.hpp"
#include "iron/base/serializable.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <vector>

namespace Iron {

    class Object : public Serializable {

        std::vector<Component*> components;
        std::vector<Object*> children;

        std::string name = "Object";
        int id;

        public:

        template<typename T>
        bool AttachComponent();
        template<typename T>
        bool RemoveComponent();
        template<typename T>
        Result<T*> GetComponent();
        template<typename T>
        bool HasComponent();
        
    };

}