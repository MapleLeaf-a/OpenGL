#include <GL/glew.h> //最先include
#include <GLFW/glfw3.h>

#include <iostream>


#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include <imgui/imgui_impl_opengl3.h>

#include "GameObject.h"
#include "Component.h"
#include "CameraComponent.h"
#include "Mesh.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);//设置垂直同步（V-Sync，垂直同步）的函数。它的作用是：控制屏幕刷新率与帧率之间的同步关系。
    /*参数值	含义	效果
        0	关闭 V-Sync	帧率不受限制，GPU 全力渲染
        1	开启 V-Sync	帧率与屏幕刷新率同步（如 60Hz 显示器 = 60 FPS）
        > 1	延迟多帧	每 N 个屏幕刷新周期交换一次缓冲区（如 2 = 30 FPS）*/


        //移动到已创建OpenGL上下文处
    if (glewInit() != GLEW_OK)
    {
        return -2;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

{
    GLCall(glEnable(GL_BLEND)); //开启 OpenGL 的透明度混合（Alpha Blending）功能，让半透明物体能够正确地与背景融合显示
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); //设置混合方式, 告诉 OpenGL 如何计算混合后的颜色
    /*混合函数                                  	公式	        用途
    (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)	src*a + dst*(1-a)	标准透明度（最常用）
    (GL_ONE, GL_ZERO)	                    src*1 + dst*0	    不透明（默认）
    (GL_SRC_ALPHA, GL_ONE)	                src*a + dst*1	    叠加发光效果
    (GL_ONE, GL_ONE)	                    src*1 + dst*1	    加性混合（火焰、光晕）
    (GL_ZERO, GL_SRC_COLOR)             	src*0 + dst*src	    正片叠底（阴影）
    (GL_ONE_MINUS_DST_COLOR, GL_ONE)	    src*(1-dst) + dst*1	屏幕混合（高光）*/

    std::shared_ptr<Mesh> cubeMesh = Mesh::CreateCube();

    GameObject mainCamera("MainCamera");
    mainCamera.GetTransform().SetPosition(glm::vec3(0.0f, 1.0f, 5.0f));

    CameraComponent* cameraComp = mainCamera.AddComponent<CameraComponent>();
    cameraComp->SetPerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);


    std::string vertFilePath = "Shaders/vert.shader";
    std::string fragFilePath = "Shaders/frag.shader";

    Shader shader(vertFilePath, fragFilePath);

    shader.Bind();

    std::string name_U4f = "u_Color";
    shader.SetUniform4f(name_U4f, 0.0f, 0.0f, 0.0f, 1.0f);

    Texture texture("../Resources/Textures/2.png");
    unsigned int slot = 0;
    texture.Bind(slot);
    shader.SetUniform1i("u_Texture", slot);

    shader.Unbind(); //清理绑定状态，防止后续不小心使用

    Renderer renderer;


    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init();


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f); //设置默认颜色

        ///* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);
        renderer.Clear();

    
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader.Bind();
            

        glm::mat4 proj = cameraComp->GetProjectionMatrix();
        glm::mat4 view = cameraComp->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 u_MVP = proj * view * model;
        shader.SetUniformMat4f("u_MVP", u_MVP);

        cubeMesh->Draw(shader, renderer);

        Transform& camTransform = mainCamera.GetTransform();

        {
            ImGui::Begin("Camera Transform");
            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            
            // ---- 位置控制 ----
            ImGui::Text("Position");
            glm::vec3 pos = camTransform.GetPosition();
            if (ImGui::DragFloat3("Pos", &pos.x, 0.1f)) {
                camTransform.SetPosition(pos);
            }
            // ---- 旋转控制（欧拉角） ----
            ImGui::Text("Rotation (degrees)");
            glm::vec3 rot = camTransform.GetEulerAngles();
            if (ImGui::DragFloat3("Rot", &rot.x, 0.5f, -180.0f, 180.0f)) {
                camTransform.SetRotation(rot);
            }
            // ---- 缩放控制 ----
            ImGui::Text("Scale");
            glm::vec3 scale = camTransform.GetScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.01f, 10.0f)) {
                camTransform.SetScale(scale);
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
} //必须加这个作用域，否则在调用glfwTerminate之后就没有了OpenGL上下文，对vb和ib的析构会失败
    


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();  

    return 0;
}