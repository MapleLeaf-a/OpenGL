#include "Test.h"

#include "imgui/imgui.h"



namespace Test
{
	TestMenu::TestMenu(Test*& currentTestPointer)
		: m_CurrentTest(currentTestPointer)
	{
	}

	TestMenu::~TestMenu()
	{
	}

	void TestMenu::OnImGuiRender()
	{
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
			{
				m_CurrentTest = test.second();  //调用函数,返回的指针赋值给m_CurrentTest
			}
		}
	}
}