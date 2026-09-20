#include "MyControl.h"

#include <iostream>

/*
* Here, below lies the super class, the Control class functions
* These are the same exact functions inside every SubControl class
*/

Control::Control()
{
	
}

Control::~Control()
{
	//Nothing yet
}


void Control::SetRelativePlacement(const glm::vec2 &minPos, const glm::vec2 &maxPos)
{
    this->_box.min = minPos;
    this->_box.max = maxPos;
}



void Control::SetIsFocused(bool isFocused)
{
	if(guiContext != nullptr)
	{
		if(isFocused == true)
		{
			if(guiContext->focusedControl != nullptr && guiContext->focusedControl != this)
			{
				guiContext->focusedControl->SetIsFocused(false);
			}

			guiContext->focusedControl = this;

			EnableStateBit((uint32_t)ControlStateBitmask::ISFOCUSED_BIT);
		}
		else
		{
			if(GetIsFocused() == true || guiContext->focusedControl == this)
			{
				DisableStateBit((uint32_t)ControlStateBitmask::ISFOCUSED_BIT);
				guiContext->focusedControl = nullptr;
			}


		}
	}
}

void Control::SetIsActive(bool isActive)
{
	if(isActive == true)
	{
		if(GetIsActive() == false) // we only do anything if it wasnt active before
		{
			EnableStateBit((uint32_t)ControlStateBitmask::ISACTIVE_BIT);
		}
	}
	else
	{
		if(GetIsActive() == true)
		{
			DisableStateBit((uint32_t)ControlStateBitmask::ISACTIVE_BIT);
		}
	}

	this->SubControlSetIsActive(isActive); // to propagate to subclasses
}


void Control::SetIsVisible(bool isVisible)
{
	if(isVisible == true)
	{
		EnableStateBit((uint32_t)ControlStateBitmask::ISVISIBLE_BIT);
	}
	else
	{
		DisableStateBit((uint32_t)ControlStateBitmask::ISVISIBLE_BIT);
	}
}

bool Control::IsCursorOnControl(const glm::vec2& cursorPos)
{
	if(guiContext == nullptr)
	{
		return false;
	}

	glm::vec2 cursorPosRelative = cursorPos / glm::vec2(guiContext->windowSize.x,guiContext->windowSize.y);

	return cursorPosRelative.x >= _cutBox.min.x && cursorPosRelative.x <= _cutBox.max.x &&
		   cursorPosRelative.y >= _cutBox.min.y && cursorPosRelative.y <= _cutBox.max.y;
}

float Control::CalculateValuebasedOnType(ValueType type, float value, float relativeTo)
{
    if(type == ValueType::FIXED)
	{
		return value;
	}
	else if(type == ValueType::RELATIVE)
	{
		return value * relativeTo;
	}

    return 0.0f;
}

float Control::CalculateValuebasedOnType(ValueType type, float value,int axis)
{
	if(guiContext == nullptr) return 0.0f;

	float multiplier = axis == 0 ? guiContext->windowSize.x : guiContext->windowSize.y;

	if(type == ValueType::FIXED)
	{
		return value;
	}
	else if(type == ValueType::RELATIVE)
	{
		return value * multiplier;
	}

    return 0.0f;
}

void Control::SetParent(Control *parent)
{
	this->parent = parent;

	//TODO: Recalculate minmaxPos
	RecalculatePosition();
}

void Control::SetBaseColor(float r, float g, float b)
{
    this->_baseColor =glm::vec3(r,g,b);
}

void Control::SetHoverColor(float r, float g, float b)
{
	this->_hoverColor = glm::vec3(r,g,b);
}

void Control::SetClickColor(float r, float g, float b)
{
	_clickColor = glm::vec3(r,g,b);
}

void Control::SetDynamicTexture(Texture *dynamicTexture)
{
	if(dynamicTexture != nullptr)
	{
		if(this->_staticTextureHandle != -1)
		{
			this->_staticTextureHandle = -1;
		}

		this->_dynamicTexture = dynamicTexture;
	}
}

void Control::SetStaticTexture(TextureHandle staticTexture)
{
	if(staticTexture >= 0)
	{
		if(this->_dynamicTexture != nullptr)
		{
			this->_dynamicTexture = nullptr;
		}

		this->_staticTextureHandle = staticTexture;
	}
}

void Control::SetSize(SizeType sizeType, float value, ValueType type)
{
	if(sizeType == SizeType::SIZE_WIDTH)
	{
		_widthDefinition.value = value;
		_widthDefinition.type = type;

		//If both margins have been previously declared to be something other than ValueType::AUTO
		//then they become invalidated.
		if(_marginDefinitions[(int)MarginType::MARGIN_LEFT].type != ValueType::AUTO && 
		_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type != ValueType::AUTO)
		{
			_marginDefinitions[(int)MarginType::MARGIN_LEFT].value = 0.0f;
			_marginDefinitions[(int)MarginType::MARGIN_LEFT].type = ValueType::AUTO;

			_marginDefinitions[(int)MarginType::MARGIN_RIGHT].value = 0.0f;
			_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type = ValueType::AUTO;
		}
	}
	else
	{
		_heightDefinition.value = value;
		_heightDefinition.type = type;

		//If both margins have been previously declared to be something other than ValueType::AUTO
		//then they become invalidated.
		if(_marginDefinitions[(int)MarginType::MARGIN_TOP].type != ValueType::AUTO && 
		_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type != ValueType::AUTO)
		{
			_marginDefinitions[(int)MarginType::MARGIN_TOP].value = 0.0f;
			_marginDefinitions[(int)MarginType::MARGIN_TOP].type = ValueType::AUTO;

			_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].value = 0.0f;
			_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type = ValueType::AUTO;
		}
	}
	

	//TODO: Recalculate minmaxPos
	RecalculatePosition();
}



void Control::SetMargin(MarginType margin, float value, ValueType type)
{
	_marginDefinitions[(int)margin].value = value;
	_marginDefinitions[(int)margin].type = type;

	if(margin == MarginType::MARGIN_BOTTOM || margin == MarginType::MARGIN_TOP)
	{
		if(_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type != ValueType::AUTO &&
			 _marginDefinitions[(int)MarginType::MARGIN_TOP].type != ValueType::AUTO && 
			 _heightDefinition.type != ValueType::AUTO)
		{
			//if both margins on this axis have been defined already, we invalidate the height definition.
			_heightDefinition.type = ValueType::AUTO;
			_heightDefinition.value = 0.0f;
		}
	}
	else
	{
		if(_marginDefinitions[(int)MarginType::MARGIN_LEFT].type != ValueType::AUTO &&
			 _marginDefinitions[(int)MarginType::MARGIN_RIGHT].type != ValueType::AUTO && 
			 _widthDefinition.type != ValueType::AUTO)
		{
			//same here as above
			_widthDefinition.type = ValueType::AUTO;
			_widthDefinition.value = 0.0f;
		}
	}

	//TODO: Recalculate minmaxPos
	RecalculatePosition();
}

void Control::RecalculatePosition()
{
	//If the guiContext hasn't been set yet, we can't process any data.
	if(guiContext == nullptr)
	{
		return;
	}

	int windowWidth = guiContext->windowSize.x;
	int windowHeight = guiContext->windowSize.y;

	glm::vec2 windowResolution = glm::vec2(windowWidth,windowHeight);

	//If the Control has no parent, it still calculates with parentMin and parentMax,
	//but now these values span the whole window.
	glm::vec2 parentMinRelative = glm::vec2(0,0);
	glm::vec2 parentMaxRelative = glm::vec2(1,1);

	if(parent != nullptr)
	{
		//If the Control does actually have a parent we aquire the necessary informations.
		AABB2 parentBox = parent->GetBox();

		parentMinRelative = parentBox.min;
		parentMaxRelative = parentBox.max;
	}

	glm::vec2 parentMinFixed = parentMinRelative * windowResolution;
	glm::vec2 parentMaxFixed = parentMaxRelative * windowResolution;

	glm::vec2 parentFixedResolution = glm::vec2(parentMaxFixed.x - parentMinFixed.x, parentMaxFixed.y - parentMinFixed.y);

	glm::vec2 ownMinFixed = _box.min * windowResolution;
	glm::vec2 ownMaxFixed = _box.max * windowResolution;

	//If there is a marginDefinition for Left margin, we determine its impact on the control.
	if(_marginDefinitions[(int)MarginType::MARGIN_LEFT].type == ValueType::FIXED)
	{
		ownMinFixed.x = parentMinFixed.x + _marginDefinitions[(int)MarginType::MARGIN_LEFT].value;
	}
	else if(_marginDefinitions[(int)MarginType::MARGIN_LEFT].type == ValueType::RELATIVE)
	{
		ownMinFixed.x = parentMinFixed.x + _marginDefinitions[(int)MarginType::MARGIN_LEFT].value * parentFixedResolution.x;
	}

	//Same procedure for the top margin.
	if(_marginDefinitions[(int)MarginType::MARGIN_TOP].type == ValueType::FIXED)
	{
		ownMinFixed.y = parentMinFixed.y + _marginDefinitions[(int)MarginType::MARGIN_TOP].value;
	}
	else if(_marginDefinitions[(int)MarginType::MARGIN_TOP].type == ValueType::RELATIVE)
	{
		ownMinFixed.y = parentMinFixed.y + _marginDefinitions[(int)MarginType::MARGIN_TOP].value * parentFixedResolution.y;
	}


	//Now it calculates the other two margins
	if(_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type == ValueType::FIXED)
	{
		ownMaxFixed.x = parentMaxFixed.x - _marginDefinitions[(int)MarginType::MARGIN_RIGHT].value;
	}
	else if(_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type == ValueType::RELATIVE)
	{
		ownMaxFixed.x = parentMaxFixed.x - _marginDefinitions[(int)MarginType::MARGIN_RIGHT].value * parentFixedResolution.x;
	}

	if(_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type == ValueType::FIXED)
	{
		ownMaxFixed.y = parentMaxFixed.y - _marginDefinitions[(int)MarginType::MARGIN_BOTTOM].value;
	}
	else if(_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type == ValueType::RELATIVE)
	{
		ownMaxFixed.y = parentMaxFixed.y - _marginDefinitions[(int)MarginType::MARGIN_BOTTOM].value * parentFixedResolution.y;
	}

	//Properly adjusting to the width
	if(_widthDefinition.type != ValueType::AUTO)
	{
		if(_widthDefinition.type == ValueType::FIXED)
		{
			if(_marginDefinitions[(int)MarginType::MARGIN_LEFT].type != ValueType::AUTO)
			{
				ownMaxFixed.x = ownMinFixed.x + _widthDefinition.value;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type != ValueType::AUTO)
			{
				ownMinFixed.x = ownMaxFixed.x - _widthDefinition.value;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type == ValueType::AUTO &&
			 _marginDefinitions[(int)MarginType::MARGIN_LEFT].type == ValueType::AUTO)
			{
				//we center the control based on its width
				float centerValueX = parentMinFixed.x + parentFixedResolution.x * 0.5f;
				ownMinFixed.x = centerValueX - _widthDefinition.value / 2.0f;
				ownMaxFixed.x = centerValueX + _widthDefinition.value / 2.0f;
			}
		}
		else if(_widthDefinition.type == ValueType::RELATIVE)
		{
			if(_marginDefinitions[(int)MarginType::MARGIN_LEFT].type != ValueType::AUTO)
			{
				ownMaxFixed.x = ownMinFixed.x + _widthDefinition.value * parentFixedResolution.x;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type != ValueType::AUTO)
			{
				ownMinFixed.x = ownMaxFixed.x - _widthDefinition.value * parentFixedResolution.x;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_RIGHT].type == ValueType::AUTO && _marginDefinitions[(int)MarginType::MARGIN_LEFT].type == ValueType::AUTO)
			{
				float centerValueX = parentMinFixed.x + parentFixedResolution.x * 0.5f;
				ownMinFixed.x = centerValueX - _widthDefinition.value * parentFixedResolution.x / 2.0f;
				ownMaxFixed.x = centerValueX + _widthDefinition.value * parentFixedResolution.x / 2.0f;
			}
		}
	}

	//Properly Adjusting height
	if(_heightDefinition.type != ValueType::AUTO)
	{
		if(_heightDefinition.type == ValueType::FIXED)
		{
			if(_marginDefinitions[(int)MarginType::MARGIN_TOP].type != ValueType::AUTO)
			{
				ownMaxFixed.y = ownMinFixed.y + _heightDefinition.value;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type != ValueType::AUTO)
			{
				ownMinFixed.y = ownMaxFixed.y - _heightDefinition.value;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_TOP].type == ValueType::AUTO && 
			_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type == ValueType::AUTO)
			{
				float centerValueY = parentMinFixed.y + parentFixedResolution.y * 0.5f;
				ownMinFixed.y = centerValueY - _heightDefinition.value / 2.0f;
				ownMaxFixed.y = centerValueY + _heightDefinition.value / 2.0f;
			}
		}
		else if(_heightDefinition.type == ValueType::RELATIVE)
		{
			if(_marginDefinitions[(int)MarginType::MARGIN_TOP].type != ValueType::AUTO)
			{
				ownMaxFixed.y = ownMinFixed.y + _heightDefinition.value * parentFixedResolution.y;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type != ValueType::AUTO)
			{
				ownMinFixed.y = ownMaxFixed.y - _heightDefinition.value * parentFixedResolution.y;
			}
			else if(_marginDefinitions[(int)MarginType::MARGIN_TOP].type == ValueType::AUTO && 
			_marginDefinitions[(int)MarginType::MARGIN_BOTTOM].type == ValueType::AUTO)
			{
				float centerValueY = parentMinFixed.y + parentFixedResolution.y * 0.5f;
				ownMinFixed.y = centerValueY - _heightDefinition.value * parentFixedResolution.y / 2.0f;
				ownMaxFixed.y = centerValueY + _heightDefinition.value * parentFixedResolution.y / 2.0f;
			}
		}
	}
	
	//Finally, we normalize the newly calculated positions.
	_box.min = ownMinFixed / windowResolution;
	_box.max = ownMaxFixed / windowResolution;

}

void Control::ControlUpdate()
{
	//Not yet implemented

	
}

void Control::CalculateCutBox()
{
	if(parent != nullptr)
	{
		bool doesIntersectWithParent = IntersectAABB(_box,parent->GetCutBox(),_cutBox);
		
	}
	else
	{
		_cutBox = _box;
	}
}

void Control::ControlRender()
{
	if(this->GetIsVisible() == false) return;

	bool doesCutBoxHaveAnyArea = this->DoesCutBoxHaveArea();
	if(!doesCutBoxHaveAnyArea) return;
	
    Rectangle ndcControlRenderRect = {_cutBox.min.x,_cutBox.min.y,
									  _cutBox.max.x - _cutBox.min.x,
									  _cutBox.max.y-_cutBox.min.y};

	glm::vec3 renderColor = _baseColor;
	if(GetIsClicked() == true)
	{
		renderColor = _clickColor;
	}
	else if(GetIsHovered() == true)
	{
		renderColor = _hoverColor;
	}
	else
	{
		renderColor = _baseColor;
	}

	if(_dynamicTexture == nullptr)
	{
		if(this->_staticTextureHandle >= 0)
		{	
			//yes texture
			AABB2 srcBox = {glm::vec2(0,0),glm::vec2(1,1)};
			TransformBasedOnTwoRelativeAABB(_box,_cutBox,srcBox);

			Rectangle srcRect = {srcBox.min.x,srcBox.min.y,srcBox.max.x - srcBox.min.x, srcBox.max.y-srcBox.min.y};
			
			guiContext->renderer.RenderColoredTexturedRectangleInstanced(ndcControlRenderRect,srcRect,renderColor,
				this->_staticTextureHandle,this->GetDepth());
		}
		else
		{
			//no texture
			guiContext->renderer.RenderRectangleInstanced(ndcControlRenderRect,renderColor,this->GetDepth());
		}
		
	}
	else
	{
		//Textured rendering
		AABB2 srcBox = {glm::vec2(0,0),glm::vec2(1,1)};
		TransformBasedOnTwoRelativeAABB(_box,_cutBox,srcBox);

		Rectangle srcRect = {srcBox.min.x,srcBox.min.y,srcBox.max.x - srcBox.min.x, srcBox.max.y-srcBox.min.y};

		//guiContext->renderer.RenderTexturedRectangle(ndcControlRenderRect,srcRect,*_renderTexture);

		guiContext->renderer.RenderColoredTexturedRectangleStandalone(ndcControlRenderRect,srcRect,renderColor,_dynamicTexture,GetDepth());
	}
	

	//TODO: Add texture rendering in an efficient manner
}

void Control::ControlResize()
{
	RecalculatePosition();
	CalculateCutBox();
}

bool Control::ControlClick(MouseButtonType button, MouseActionType action)
{
	if(button == MouseButtonType::MOUSE_BUTTON_LEFT)
	{
		if(action == MouseActionType::MOUSE_ACTION_PRESS)
		{
			if(GetIsHovered() == true)
			{
				EnableStateBit((uint32_t)ControlStateBitmask::ISCLICKED_BIT);

				return true;
			}
		}
		
		DisableStateBit((uint32_t)ControlStateBitmask::ISCLICKED_BIT);
	}

	return false;
}

bool Control::ControlMouseMove()
{
	bool isCurrentCursorOnControl = IsCursorOnControl(guiContext->currentMousePos);

	if(isCurrentCursorOnControl)
	{
		EnableStateBit((uint32_t)ControlStateBitmask::ISHOVERED_BIT);
	}
	else
	{
		DisableStateBit((uint32_t)ControlStateBitmask::ISHOVERED_BIT);
	}

	return isCurrentCursorOnControl;
}

bool Control::DoesCutBoxHaveArea()
{
	float cutWidth = _cutBox.max.x - _cutBox.min.x;
	float cutHeight = _cutBox.max.y - _cutBox.min.y;

	const float minEps = 0.000001f;

    return cutWidth > minEps && cutHeight > minEps;
}
