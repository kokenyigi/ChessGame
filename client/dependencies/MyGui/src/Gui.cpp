#include "Gui.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp> // transzformációkhoz

/*
* Functions Related To the Graphical Use Interface class
* This class basically acts as a container for our Controls
* 
*/
GUI::GUI()
{
	//Nothing yet
}

GUI::~GUI()
{
	//Nothing yet
}

bool GUI::Init(int windowWidth, int windowHeight)
{
	m_guiContext.windowSize.x = windowWidth;
	m_guiContext.windowSize.y = windowHeight;

    bool rendererInitializationResult = m_guiContext.renderer.Init();
	if(rendererInitializationResult == false) return false;

	TextureHandle alphabetTextureHandle = m_guiContext.renderer.LoadAndRegisterTexture("assets/textures/alphabets/alpha_alphabet.png");

	m_guiContext.alphabetTextureHandles.push_back(alphabetTextureHandle);
	m_guiContext.currentAlphabetIndex = 0;

	return true;
}

void GUI::Update(float deltaTime)
{
	m_guiContext.deltaTime = deltaTime;

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Update();
	}
}

void GUI::Render()
{
	/*
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	*/

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Render();
	}

	this->m_guiContext.renderer.Render();

	
	/*
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	*/
}

void GUI::Resize(int newWindowWidth, int newWindowHeight)
{
	m_guiContext.windowSize.x = newWindowWidth;
	m_guiContext.windowSize.y = newWindowHeight;

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Resize(); 
	}
}

void GUI::MouseMove(float newMousePosX, float newMousePosY)
{
	m_guiContext.oldMousePos = m_guiContext.currentMousePos;
	m_guiContext.currentMousePos.x = newMousePosX;
	m_guiContext.currentMousePos.y = newMousePosY;

	if(m_guiContext.focusedControl != nullptr)
	{
		m_guiContext.focusedControl->VirtualMouseMove();
	}

	if(!m_guiContext.isMouseDisabled)
	{
		for(int i=0;i<controls.size();++i)
		{
			controls[i]->MouseMove();
		}
	}
    

	
}

void GUI::MouseClick(MouseButtonType button, MouseActionType action)
{
	if(m_guiContext.focusedControl != nullptr)
	{
		m_guiContext.focusedControl->VirtualClick(button,action);
	}

	if(!m_guiContext.isMouseDisabled)
	{
		for (int i = 0;i < controls.size();++i)
		{
			controls[i]->Click(button,action);
			//std::cout<<"Calling click " << i<< ".th time\n";
		}
	}
	
}

void GUI::MouseWheel(float amount, MouseWheelDirection direction)
{
	if(m_guiContext.focusedControl != nullptr)
	{
		
		m_guiContext.focusedControl->VirtualMouseWheel(amount,direction);
	}

	if(!m_guiContext.isMouseDisabled)
	{
		for (int i = 0;i < controls.size();++i)
		{
			controls[i]->MouseWheel(amount,direction);
		}
	}
}

void GUI::KeyInput(int key,int action, int mods)
{
	if(m_guiContext.focusedControl != nullptr)
	{	
		m_guiContext.focusedControl->VirtualKeyInput(key,action,mods);
	}

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->KeyInput(key,action,mods);
	}
}

void GUI::AddControl(Control* control)
{
	controls.push_back(control);
	control->SetGuiContext(GetContext());
}






