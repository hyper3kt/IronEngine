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
        static Result<Scene*> LoadScene(const char* path);

        Result<Object*> GetObject(ObjectId id);
        std::vector<Object*> GetObjectsByName(std::string name);
        std::vector<Object*> GetObjectsByComponent(std::string componentName);
        std::vector<Object*> GetObjectsByType(std::string typeName);
        
        void AddObject(Object* object);
        void RemoveObject(ObjectId id);

        void Tick();
        void Archive();
        void FromState(const char* archivePath);
        void BareLoad();
        void Unload();

    };

}