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

	
	
	if(this->m_guiContext.topLevelControl != nullptr) this->m_guiContext.topLevelControl->Update();
	
}

void GUI::Render()
{
	/*
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	*/

	
	if(this->m_guiContext.topLevelControl != nullptr) this->m_guiContext.topLevelControl->Render();
	

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

	
	
	if(this->m_guiContext.topLevelControl != nullptr) this->m_guiContext.topLevelControl->Resize(); 
	
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
		
		
		if(this->m_guiContext.topLevelControl != nullptr) this->m_guiContext.topLevelControl->MouseMove();
		
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
		if(this->m_guiContext.topLevelControl != nullptr) this->m_guiContext.topLevelControl->Click(button,action);
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
		if(this->m_guiContext.topLevelControl != nullptr) this->m_guiContext.topLevelControl->MouseWheel(amount,direction);
	}
}

void GUI::KeyInput(int key,int action, int mods)
{
	if(m_guiContext.focusedControl != nullptr)
	{	
		m_guiContext.focusedControl->VirtualKeyInput(key,action,mods);
	}

	if(this->m_guiContext.topLevelControl != nullptr) this->m_guiContext.topLevelControl->KeyInput(key,action,mods);
}

void GUI::AddControl(Control* control)
{
	//controls.push_back(control);
	control->SetGuiContext(GetContext());
}






