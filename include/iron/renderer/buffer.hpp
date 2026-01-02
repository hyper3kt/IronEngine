#pragma once

#include "iron/macros.hpp"

#include <glad/glad.h>

#include <iostream>
#include <vector>

#define BUFFER_DRAW_ALL_TRIS -1

namespace Iron {

    enum AttributeSize {
        ATTR_SIZE_VEC2 = 2,
        ATTR_SIZE_VEC3,
        ATTR_SIZE_VEC4,
    };

    class IronDLL Buffer {

        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        size_t vertexSize;
        size_t numTriangles;
        int attributes = 0;
        int attributeOffset = 0;
        bool usesEBO;

        public:

        Buffer(std::vector<float> vertices, size_t vertexSize);
        Buffer(std::vector<float> vertices, std::vector<GLuint> indices, size_t vertexSize);
        ~Buffer();

        void AddAttribute(AttributeSize size, bool normalize = false);
        void Draw(int triOffset = 0, int triCount = BUFFER_DRAW_ALL_TRIS);

    };

}