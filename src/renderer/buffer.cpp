#include "iron/renderer/buffer.hpp"

using namespace Iron;

Buffer::Buffer(std::vector<float> vertices, size_t vertexSize) {
    usesEBO = false;
    this->vertexSize = vertexSize;
    numTriangles = vertices.size() / 3;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Buffer::Buffer(std::vector<float> vertices, std::vector<GLuint> indices, size_t vertexSize) {
    usesEBO = true;
    this->vertexSize = vertexSize;
    numTriangles = vertices.size() / 3;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Buffer::~Buffer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Buffer::AddAttribute(AttributeSize size, bool normalize) {
    if(attributeOffset >= vertexSize) {
        return;
    }

    glBindVertexArray(VAO);
    glVertexAttribPointer(attributes, size, GL_FLOAT, normalize ? GL_TRUE : GL_FALSE, vertexSize * sizeof(float), (void*) (attributeOffset * sizeof(float)));
    glBindVertexArray(0);

    attributes++;
    attributeOffset += size;
}

void Buffer::Draw(int triOffset, int triCount) {
    glBindVertexArray(VAO);

    if(usesEBO) {
        glDrawElements(GL_TRIANGLES, triCount * 3, GL_UNSIGNED_INT, 0);
        return;
    }

    if(triCount == BUFFER_DRAW_ALL_TRIS) {
        triCount = numTriangles;
    }

    glDrawArrays(GL_TRIANGLES, triOffset * 3, triCount * 3);
}