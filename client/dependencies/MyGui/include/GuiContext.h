#pragma once
#ifndef MYGUICONTEXT_H
#define MYGUICONTEXT_H

#include <vector>

//#include "MyOpenGL.h"
#include "Renderer2D.h"
#include "Utils.h"

enum class MouseButtonType
{
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT = 1
};

enum class MouseActionType
{
    MOUSE_ACTION_PRESS = 0,
    MOUSE_ACTION_RELEASE = 1,
    MOUSE_ACTION_HOLD = 2
};

enum class MouseWheelDirection
{
	MOUSEWHEEL_FORWARD = 0,
	MOUSEWHEEL_BACKWARD = 1,
};

class Control;

struct GuiContext
{
	glm::vec2 windowSize;

	glm::vec2 oldMousePos;
	glm::vec2 currentMousePos;

	Renderer2D renderer;

	float deltaTime = 1.0f;

	int currentAlphabetIndex = -1;
	std::vector<TextureHandle> alphabetTextureHandles;

	Control* focusedControl = nullptr;

	Control* topLevelControl = nullptr;

	bool isMouseDisabled = false;
	/*
	Texture guiTexture;
	Texture alphaBetTexture;
	*/
};

#endif