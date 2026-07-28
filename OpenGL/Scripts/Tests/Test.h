#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>

namespace Test {
	class Test
	{
	public:
		Test() {} 
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestPointer);
		~TestMenu();

		void OnImGuiRender() override;

		//模板函数的**定义**必须放在头文件中
		template<typename T>
		void RegisterTest(const std::string& name) 
		{
			std::cout << "Register test" << name << std::endl;

			m_Tests.push_back({ name, []() { return new T(); } });
		}
	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	/*std::function 是 C++ 标准库提供的一个"万能函数包装器"——它可以存储、复制和调用任何可调用对象（函数指针、lambda 表达式、函数对象等），只要它们的签名（参数类型和返回类型）匹配。
		基本语法
		#include <functional>
		std::function<返回类型(参数类型1, 参数类型2, ...)> 变量名;*/
	};
}