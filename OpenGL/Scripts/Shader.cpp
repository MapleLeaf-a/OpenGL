#include <GL/glew.h> //最先include

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "Renderer.h"

Shader::Shader(const std::string& vertFilePath, const std::string& fragFilePath)
    : m_RendererID(0)
{
    std::string vert = PraseShader(vertFilePath);
    std::string frag = PraseShader(fragFilePath);

    m_RendererID = CreateShader(vert, frag);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3)); /* 设置对应的统一变量 */
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
    /*
    参数位置	参数名	    类型	            含义
    第 1 个	    location	GLint	        uniform 变量的位置索引（告诉 OpenGL 数据存到哪里）
    第 2 个	    count		GLsizei	        要上传的矩阵数量（1 表示传 1 个矩阵）
    第 3 个	    transpose	GLboolean	    是否转置矩阵（GL_FALSE = 不转置，因为 glm 和 OpenGL 都是列主序）
    第 4 个	    value	    const GLfloat*	矩阵数据的首地址指针（16 个 float 连续排列）
    */  
}

std::string Shader::PraseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    std::stringstream ss;
    std::string line;
    ss << stream.rdbuf();

    return ss.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); //创建着色器对象.参数shaderType:指定要创建的着色器类型.返回值:无符号整数,作为新创建的着色器对象的唯一标识符(ID)
    const char* src = source.c_str(); //将 C++ 的 std::string 对象转换为 C 风格的空字符结尾字符串（const char*）
    glShaderSource(id, 1, &src, nullptr); //将着色器源代码传递给着色器对象
    //将 src 指向的源代码（1 个字符串）传递给着色器对象 id，字符串以空字符结尾（nullptr 表示自动检测长度）

    glCompileShader(id); //编译着色器对象，将已经设置了源代码的着色器对象 id 编译成可执行的着色器程序

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //查询着色器对象 id 的编译状态，将结果存入 result 变量中。GL_COMPILE_STATUS：编译是否成功
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //获取错误信息长度
        char* message = new char[length];
        glGetShaderInfoLog(id, length, nullptr, message); //参数三为实际写入的长度,此处忽略
        std::cout << "Fail to compile " << ((type == GL_VERTEX_SHADER) ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        delete[] message;
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); //创建着色器程序对象的函数。它本身不执行着色器代码，而是创建一个"容器"，用来链接和管理多个着色器（顶点着色器、片段着色器等）
    //返回值：一个无符号整数，作为新创建的程序对象的唯一标识符（ID）

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs); //将shader附到program上
    glAttachShader(program, fs);

    glLinkProgram(program); //链接

    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetProgramInfoLog(program, length, nullptr, message);
        std::cout << "Failed to link shader program!" << std::endl;
        std::cout << message << std::endl;
        glDeleteProgram(program);
        delete[] message;
        return 0;
    }

    glValidateProgram(program);

    int validateResult;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &validateResult);
    if (validateResult == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetProgramInfoLog(program, length, nullptr, message);
        std::cout << "Program validation failed!" << std::endl;
        std::cout << message << std::endl;
        delete[] message;
        return 0;
    }

    glDeleteShader(vs); //删除着色器对象,释放其占用的GPU内存资源
    glDeleteShader(fs);

    return program;
}


int Shader::GetUniformLocation(const std::string& name) const
{
    if (m_UniformlocationCache.count(name))
        return m_UniformlocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str()); //查询着色器程序中名为 "u_Color" 的 uniform 变量的位置索引
    if (location == -1)
    {
        std::cout << "Warning: uniform '" << name << "' doesn't exist" << std::endl;
    }

    m_UniformlocationCache[name] = location;
    return location;
}