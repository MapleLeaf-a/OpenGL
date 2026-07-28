#pragma once
#include <vector>
#include <memory>
// #include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    //禁止拷贝（OpenGL对象不能简单拷贝）
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    //允许移动
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // ---- 绘制 ----
    void draw() const;

    // ---- 获取数据 ----
    size_t getVertexCount() const { return m_vertexCount; }
    size_t getIndexCount() const { return m_indexCount; }

    // ---- 静态工厂方法 ----
    static std::shared_ptr<Mesh> createCube();
    static std::shared_ptr<Mesh> createPlane(float size = 5.0f);
    static std::shared_ptr<Mesh> createSphere(int segments = 24);

private:
    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void cleanup();

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    size_t m_vertexCount = 0;
    size_t m_indexCount = 0;
};