#include "Mesh.h"
#include "Renderer.h"
#include <cmath>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    SetupMesh(vertices, indices);
}

void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    m_VertexCount = vertices.size();
    m_IndexCount = indices.size();

    //创建vertexBuffer
    m_VertexBuffer = std::make_unique<VertexBuffer> (vertices.data(), static_cast<unsigned int>( vertices.size() * sizeof(Vertex) ));
    
    //创建VertexBufferLayout
    VertexBufferLayout layout;
    layout.Push<float>(3); //position:3个float
    layout.Push<float>(3); //normal:3个float
    layout.Push<float>(2); //texCoord:2个float

    //创建vertexArray,注意顺序,让VAO记住EBO(IBO)
    m_VertexArray = std::make_unique<VertexArray> ();
    m_VertexArray->AddBuffer(*m_VertexBuffer, layout);

    //创建indexBuffer
    m_IndexBuffer = std::make_unique<IndexBuffer> (indices.data(), static_cast<unsigned int>(indices.size()) );
}

void Mesh::Draw(const Shader& shader, const Renderer& renderer) const {
    renderer.Draw(*m_VertexArray, *m_IndexBuffer, shader);
}

// ---- 静态工厂方法实现 ----

std::shared_ptr<Mesh> Mesh::CreateCube() {
    // 6个面，每个面4个顶点，共24个顶点
    std::vector<Vertex> vertices = {
        // 前面 (Z+)
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        // 后面 (Z-)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        // 左面 (X-)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        // 右面 (X+)
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        // 下面 (Y-)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        // 上面 (Y+)
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };

    // 6个面，每个面2个三角形，每个三角形3个索引，共36个索引
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,     // 前面
        4, 5, 6, 6, 7, 4,     // 后面
        8, 9, 10, 10, 11, 8,  // 左面
        12, 13, 14, 14, 15, 12, // 右面
        16, 17, 18, 18, 19, 16, // 下面
        20, 21, 22, 22, 23, 20  // 上面
    };

    return std::make_shared<Mesh>(vertices, indices);
}

//size是总大小
std::shared_ptr<Mesh> Mesh::CreatePlane(float size) {
    float half = size * 0.5f;
    std::vector<Vertex> vertices = {
        {{-half, 0.0f, -half}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ half, 0.0f, -half}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ half, 0.0f,  half}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-half, 0.0f,  half}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };
    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Mesh::CreateSphere(int segments) {  //UV Sphere,利用球面坐标来生成球体的Mesh
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int y = 0; y <= segments; ++y) {
        float theta = glm::pi<float>() * static_cast<float>(y) / static_cast<float>(segments);
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int x = 0; x <= segments; ++x) {
            float phi = 2.0f * glm::pi<float>() * static_cast<float>(x) / static_cast<float>(segments);
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            glm::vec3 pos = glm::vec3(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
            glm::vec2 uv = glm::vec2(static_cast<float>(x) / segments, static_cast<float>(y) / segments);
            vertices.push_back({pos, pos, uv});
        }
    }

    for (int y = 0; y < segments; ++y) {
        for (int x = 0; x < segments; ++x) {
            int i0 = y * (segments + 1) + x;
            int i1 = i0 + 1;
            int i2 = (y + 1) * (segments + 1) + x;
            int i3 = i2 + 1;

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    return std::make_shared<Mesh>(vertices, indices);
}