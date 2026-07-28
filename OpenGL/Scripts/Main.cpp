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

#include "Tests/TestClearColor.h"



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
    float positions[] =
    {
        0.0f,    0.0f,    0.0f, 0.0f, // 0   16:9
        960.0f,  0.0f,    1.0f, 0.0f, // 1
        960.0f,  540.0f,  1.0f, 1.0f, // 2
        0.0f,    540.0f,  0.0f, 1.0f ,// 3
    };

    //为了使顶点不重复存储,引入index buffer(索引缓冲区),说明三角形的按索引排列方式
    unsigned int indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    GLCall(glEnable(GL_BLEND)); //开启 OpenGL 的透明度混合（Alpha Blending）功能，让半透明物体能够正确地与背景融合显示
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); //设置混合方式, 告诉 OpenGL 如何计算混合后的颜色
    /*混合函数                                  	公式	        用途
    (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)	src*a + dst*(1-a)	标准透明度（最常用）
    (GL_ONE, GL_ZERO)	                    src*1 + dst*0	    不透明（默认）
    (GL_SRC_ALPHA, GL_ONE)	                src*a + dst*1	    叠加发光效果
    (GL_ONE, GL_ONE)	                    src*1 + dst*1	    加性混合（火焰、光晕）
    (GL_ZERO, GL_SRC_COLOR)             	src*0 + dst*src	    正片叠底（阴影）
    (GL_ONE_MINUS_DST_COLOR, GL_ONE)	    src*(1-dst) + dst*1	屏幕混合（高光）*/

    VertexArray va;
    VertexBuffer vb(positions, sizeof(positions));
    VertexBufferLayout layout;

    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    //启用索引为 0 的顶点属性数组
    //glEnableVertexAttribArray(0);

    //绑定数据以后需要告诉OpenGL如何使用这些数据
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    /*参数:
        1.指定要配置的属性位置，必须与 glEnableVertexAttribArray 的索引一致
        2.每个顶点的属性分量个数。2表示这个属性由2个值组成（X和Y坐标）。合法值是1、2、3、4
        3.每个分量的数据类型
        4.是否将数据归一化。GL_FALSE 表示保持原始值不变（如果传 GL_TRUE，整数类型会被映射到 0~1 或 -1~1 范围）
        5.步长，即相邻两个顶点之间相隔多少字节。这里 = 8 字节，因为每个顶点有 2 个 float，每个 float 占 4 字节
        6.偏移量，即该属性在顶点数据起始位置后的字节偏移。0 表示从顶点数据的开头读取
    */


    IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

    // 此时，VAO 已经记录了所有配置（从开始记录到此处）
    // 可以解绑 VAO 了（可选）
    //GLCall(glBindVertexArray(0));

    glm::mat4 proj = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    std::string vertFilePath = "Shaders/vert.shader";
    std::string fragFilePath = "Shaders/frag.shader";

    Shader shader(vertFilePath, fragFilePath);

    shader.Bind();

    std::string name_U4f = "u_Color";
    shader.SetUniform4f(name_U4f,1.0f, 0.0f, 0.0f, 1.0f);

    Texture texture("../Resources/Textures/2.png");
    unsigned int slot = 0;
    texture.Bind(slot);
    shader.SetUniform1i("u_Texture", slot);


    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind(); //清理绑定状态，防止后续不小心使用

    Renderer renderer;


    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init();


    glm::vec3 translation(200, 200, 0);

    Test::Test* currentTest = nullptr;
    Test::TestMenu* testMenu = new Test::TestMenu(currentTest);
    currentTest = testMenu;

 
    testMenu->RegisterTest<Test::TestClearColor>("Clear Color");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //设置默认颜色

        ///* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);
        renderer.Clear();

    
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (currentTest)
        {
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-")) //不是在主测试菜单就加一个返回按钮
            {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }



        ////重新绑定VAO
        //glBindVertexArray(vao);  // ← 如果这行被注释了，就会报错！

        //va.Bind();
        shader.Bind();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 u_MVP = proj * view * model;
        shader.SetUniformMat4f("u_MVP", u_MVP);


        //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        //参数:
        // 1.图元类型 
        // 2.索引个数 
        // 3.索引缓冲区中数据类型 
        // 4.指向索引缓冲区的指针(此处由于前面已经绑定了GL_ELEMENT_ARRAY_BUFFER,故传入nullptr)
        renderer.Draw(va, ib, shader);


        {
            ImGui::SliderFloat3("translation", &translation.x, -1920.0f, 1920.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    delete currentTest;
    if (currentTest != testMenu)
        delete testMenu;
} //必须加这个作用域，否则在调用glfwTerminate之后就没有了OpenGL上下文，对vb和ib的析构会失败
    


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();  

    return 0;
}