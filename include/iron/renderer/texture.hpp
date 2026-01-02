#pragma once

#include "iron/fs/fs_map.hpp"
#include "iron/macros.hpp"

#include <glad/glad.h>

namespace Iron {

    class IronDLL Texture {

        GLuint texture;

        public:

        Texture(FileSystem::Map map, bool generateMipMaps = true);
        ~Texture();

        void Use(int offset);

    };

}