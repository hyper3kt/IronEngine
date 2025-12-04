#pragma once

#include "iron/base/component.hpp"
#include "iron/base/archive.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <vector>

namespace Iron {

    class Object {

        Archive* archive;
        std::vector<Component*> components;

        protected:

        std::vector<Object*> children;
        Object* parent = nullptr;

        public:

        template<typename T>
        bool AttachComponent();
        template<typename T>
        bool RemoveComponent();
        template<typename T>
        Result<T*> GetComponent();
        template<typename T>
        bool HasComponent();

        Object();
        Object(Archive* archive);

        virtual void Init();
        std::string GetName();
        void SetName(std::string name);
        void SetParent(Object* parent);
        void AddChild(Object* child);
        void AddChildren(std::vector<Object*> children);
        void DeleteChild(std::string name);
        void Orphan();
        void Orphan(Object* orphan);
        void Delete();
        Archive* GetArchive();
        virtual void Tick(float dt);

        // TODO: refactor
        static void DeclareObject(std::string typeName, Object* instance);
    };

    struct ObjectRegistry {
        std::string typeName;
        Object* instance;
    };

}