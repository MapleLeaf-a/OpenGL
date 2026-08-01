#pragma once

#include "Component.h"
#include "Mesh.h"
#include "Material.h"

#include <memory>

class MeshRenderer : public Component
{
public:
    MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
        : m_Mesh(mesh), m_Material(material) {}

    void SetMesh(std::shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
    void SetMaterial(std::shared_ptr<Material> material) { m_Material = material; }

    std::shared_ptr<Mesh> GetMesh() const { return m_Mesh; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

    void Render(Shader& shader, const Renderer& renderer) const
    {
        if (!m_Mesh || !m_Material) return; 
        
        Transform* transform = GetTransform();
        if (transform) 
        {
            shader.SetUniformMat4f("u_Model", transform->GetModelMatrix());
        }
        
        m_Material->Apply(shader); //将材质属性应用到着色器中
        
        m_Mesh->Draw(shader, renderer); //绘制网格   
    }

    void OnRender() override
    {
        
    }
    
private:
    std::shared_ptr<Mesh> m_Mesh;
    std::shared_ptr<Material> m_Material;
};