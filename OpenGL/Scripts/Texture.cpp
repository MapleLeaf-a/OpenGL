#include "Texture.h"

#include "stb_image/stb_image.h"

#include <iostream>

Texture::Texture(const std::string& path)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1); //1表示加载时需要垂直翻转图像
	//对图像来说,原点(0,0)是左上角,而OpenGL原点在左下,故需要垂直翻转,否则会上下颠倒

	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4); //加载时会填写2~4,最后一个参数表示加载几个通道(4:RGBA)

	if (!m_LocalBuffer) {
		const char* error = stbi_failure_reason();
		std::cout << "stbi_load failed: " << (error ? error : "unknown") << std::endl;
	}

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	/*设置纹理过滤方式（Filtering）
	作用：告诉 OpenGL 当纹理被放大（Magnify）或缩小（Minify）时，如何计算像素颜色。
		参数                   	含义	                  说明
	GL_TEXTURE_MAG_FILTER	纹理放大时的过滤方式	当纹理比屏幕小时
	GL_TEXTURE_MIN_FILTER	纹理缩小时的过滤方式	当纹理比屏幕大时
	GL_LINEAR	            线性插值（平滑模糊）	取周围像素的加权平均
	直观对比：
	过滤方式						效果					适用场景
	GL_NEAREST					锯齿、像素风（最近邻）	像素风格游戏、UI 图标
	GL_LINEAR					平滑、模糊				照片、3D 场景
	GL_NEAREST_MIPMAP_NEAREST	锯齿+性能好				老游戏
	GL_LINEAR_MIPMAP_LINEAR	    平滑+性能好				现代 3D 游戏*/

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	/*设置纹理包裹方式（Wrapping）
	作用：当纹理坐标（UV）超出 0-1 范围时，如何处理。
		参数					含义						说明
	GL_TEXTURE_WRAP_S	S 方向（水平/U 轴）的包裹方式	U 坐标 0-1
	GL_TEXTURE_WRAP_T	T 方向（垂直/V 轴）的包裹方式	V 坐标 0-1
	GL_CLAMP			夹紧到边缘（边缘像素拉伸）	超出范围使用边缘颜色
	其他包裹方式：
	包裹方式				效果				说明
	GL_CLAMP			边缘拉伸			超出范围固定为边缘颜色
	GL_REPEAT			重复平铺			纹理像瓷砖一样重复
	GL_MIRRORED_REPEAT	镜像重复			纹理像镜子一样来回反射
	GL_CLAMP_TO_EDGE	边缘拉伸（更常用）	比 GL_CLAMP 更标准
	*/

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	//上传纹理数据 : 将 CPU 内存中的图像数据（m_LocalBuffer）上传到 GPU 显存中，创建纹理。
	/*参数				值					含义
	target			GL_TEXTURE_2D		纹理类型：2D 纹理
	level			0					Mipmap 级别（0 = 基础级别）
	internalformat	GL_RGBA8			GPU 内部存储格式（8 位/通道 = 32 BPP）
	width			m_Width				纹理宽度（像素）
	height			m_Height			纹理高度（像素）
	border			0					边框（必须为 0）(历史遗留原因)
	format			GL_RGBA	CPU			数据格式（传入数据的排列方式）
	type			GL_UNSIGNED_BYTE	每个通道的数据类型（8 位无符号整数）
	data			m_LocalBuffer		CPU 内存中的图像数据指针*/
	
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); //解绑

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); //激活第slot纹理插槽(0起始)
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
