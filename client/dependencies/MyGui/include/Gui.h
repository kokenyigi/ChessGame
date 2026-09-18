#pragma once
#ifndef MYGUI_H
#define MYGUI_H

#include <vector>

#include "MyOpenGL.h"
#include "Renderer2D.h"
#include "Utils.h"

#include "GuiContext.h"
#include "MyControl.h"

class GUI
{
private:
	//All previous member variables have been compressed into the context below
	//Every single control there is, has to have a guiContext at the time of creation, because then this given control knows
	//How to properly calculate its internal state using the context, this context pointer also enables it to render,
	//without having to be passed render parameters
	GuiContext m_guiContext;


	//The main Control container
	//Note: Later on its better for this to be a universal tree, since
	//Controls can have subcontrols implanted inside them, like a layoutpanel or some shit
	//For now, a linear data structure can be enough since no subcontainer is needed
	std::vector<Control*> controls;
	
public:
	GUI();
	~GUI();

	bool Init(int windowWidth, int windowHeight);

	GuiContext* GetContext() { return &m_guiContext; }

	void ToggleMouseEnabledDisabled() {m_guiContext.isMouseDisabled =  m_guiContext.isMouseDisabled == false ? true : false;}

	void Update(float deltaTime);
	void Render();
	void Resize(int newWindowWidth, int newWindowHeight);
	void MouseMove(float newMousePosX, float newMousePosY );
	void MouseClick(MouseButtonType button, MouseActionType action);
	void MouseWheel(float amount,MouseWheelDirection direction);
	void KeyInput(int key,int action, int mods);

	//Needs still something like:
	//void KeyPressed(char key);

	void AddControl(Control* control);
};


#endif