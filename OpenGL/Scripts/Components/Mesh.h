// Mesh.hpp
#pragma once
#include <vector>
#include <memory>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
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
    ~Mesh() = default;

    //禁止拷贝，允许移动
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;

    //绘制,要求有shader资源和renderer渲染器
    void Draw(const Shader& shader, const Renderer& renderer) const;

    //获取数据
    size_t GetVertexCount() const { return m_vertexCount; }
    size_t GetIndexCount() const { return m_indexCount; }

    // ---- 静态工厂方法 ----
    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreatePlane(float size = 5.0f);
    static std::shared_ptr<Mesh> CreateSphere(int segments = 24);

private:
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    std::unique_ptr<VertexArray> m_vertexArray;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    size_t m_vertexCount = 0;
    size_t m_indexCount = 0;
};