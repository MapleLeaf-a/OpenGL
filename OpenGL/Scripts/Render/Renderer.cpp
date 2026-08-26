#include "Renderer.h"
#include <iostream>

#include "MeshRenderer.h"
#include "Scene.h"
#include "Texture.h"

/// <summary>
/// 清空错误队列
/// </summary>
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
    /*OpenGL 采用异步错误处理机制：
    错误不会立即抛出，而是存储在错误队列中
    调用 glGetError() 会弹出队列中的一个错误
    如果有多个错误，需要多次调用才能清空队列*/
}

/// <summary>
/// 打印错误
/// </summary>
bool GLPrintError(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error Occurred!] (" << error << ")：" << function << " " << file << " line:" << line << std::endl;
        return false;
    }
    return true;
}


Renderer::Renderer()
{
    m_GizmoQuadMesh = Mesh::CreatePlane_XY(1.0f);
}

Renderer::~Renderer()
{
}

void Renderer::Clear() const
{
    //同时清除颜色缓冲和深度缓冲:若不清理深度,上一帧的深度值会残留,导致新帧遮挡关系错乱
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

//DrawCall
void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    va.Bind();
    shader.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}


void Renderer::RenderAllMeshRenderers(const Scene& scene, Shader& shader) const
{
    std::vector<MeshRenderer*> meshRenderers = scene.GetMeshRenderers();
    for (const MeshRenderer* meshRenderer: meshRenderers)
    {
        meshRenderer->Render(shader, *this);
    }
}

void Renderer::RenderAllLights(const Scene& scene, LightUBO& lightUBO) const
{
    std::vector<LightComponent*> m_Lights = scene.GetLights();

    if (m_Lights.empty()) return;

    std::vector<LightData> lights;

    for (auto& i : m_Lights)
    {
        lights.push_back(i->GetLightData());
    }

    lightUBO.Update(lights);
}

void Renderer::RenderLightGizmos(const Scene& scene, CameraComponent* camera, Shader& gizmoShader, const Texture* pointLightIconTexture, const Texture* dirLightIconTexture)
{
    std::vector<LightComponent*> m_Lights = scene.GetLights();

    if (m_Lights.empty()) return;

    if (pointLightIconTexture == nullptr && dirLightIconTexture == nullptr) return;

    //设置绑定shader
    gizmoShader.Bind();
    gizmoShader.SetUniformMat4f("u_View", camera->GetViewMatrix());
    gizmoShader.SetUniformMat4f("u_Projection", camera->GetProjectionMatrix());

    //设置渲染状态
    glEnable(GL_BLEND); //开启透明度混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //设置混合方式
    glDisable(GL_DEPTH_TEST); //不写入深度测试

    for (int i = 0; i < m_Lights.size(); i++)
    {
        const LightComponent* light = m_Lights[i];

        LightType type = light->GetLightType();
        switch (type)
        {
            case LightType::Directional:
                if (dirLightIconTexture != nullptr)
                {
                    dirLightIconTexture->Bind(1); //和主渲染的贴图插槽区分开
                    gizmoShader.SetUniform1i("u_IconTexture", 1);
                }
                break;
            case LightType::Point:
                if (pointLightIconTexture != nullptr)
                {
                    pointLightIconTexture->Bind(1); 
                    gizmoShader.SetUniform1i("u_IconTexture", 1);
                }
                break;

            default:
                break;
        }

        //获取光源位置
        glm::vec3 lightPos = light->GetTransform()->GetWorldPosition();

        //Billboarding(公告牌技术)矩阵计算
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, lightPos);

        //让四边形始终朝向相机的-forward方向
        glm::vec3 lookDir = glm::normalize(-camera->GetTransform()->GetForward());
        glm::vec3 right = glm::normalize(glm::cross(lookDir, glm::vec3(0, 1, 0))); //通过叉乘lookDir和世界坐标的Up(0,1,0)得到right
        glm::vec3 up = glm::normalize(glm::cross(right, lookDir));

        glm::mat4 rotMatrix = glm::mat4(1.0f);
        rotMatrix[0] = glm::vec4(right, 0.0f);
        rotMatrix[1] = glm::vec4(up, 0.0f);
        rotMatrix[2] = glm::vec4(-lookDir, 0.0f);
        //列主序的旋转矩阵

        model = model * rotMatrix;
        model = glm::scale(model, glm::vec3(0.5f)); // 图标大小  

        gizmoShader.SetUniformMat4f("u_Model", model);

        m_GizmoQuadMesh->Draw(gizmoShader, *this);
    }

    //恢复状态
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
