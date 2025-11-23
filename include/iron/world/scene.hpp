#pragma once

#include "iron/world/object.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <vector>

namespace Iron {

    class Scene {

        std::vector<Object*> objects;

        public:

        static bool HasScene(const char* path);
        static Result<Scene> LoadScene(const char* path);

        Result<Object*> GetObject(int id);
        std::vector<Object*> GetObjectsByName(std::string name);
        template<typename T>
        std::vector<Object*> GetObjectsByComponent();
        template<typename T>
        std::vector<T*> GetObjectsByType();
        
        void AddObject(Object* object);
        void RemoveObject(int id);

        void Tick();
        void Archive();
        void FromState(const char* archivePath);
        void BareLoad();
        void Unload();

    };

}