#include "ToggleButton.h"

void ToggleButton::SetIsToggled(bool isToggled)
{
    if(GetIsToggled() == isToggled) return;

    if(isToggled == true)
    {
        EnableStateBit((uint32_t)ToggleButtonStateBitmask::ISTOGGLED_BIT);
    }
    else
    {
        DisableStateBit((uint32_t)ToggleButtonStateBitmask::ISTOGGLED_BIT);
    }
}

void ToggleButton::ImitateToggle(bool isToggledOn)
{
    SetIsToggled(isToggledOn);

    if(this->_callBack != nullptr && this->_callBackContext != nullptr)
    {
        this->_callBack(this->_callBackContext,isToggledOn);
    }
}

bool ToggleButton::ToggleButtonClick(MouseButtonType button, MouseActionType action)
{
    bool isControlClicked = ControlClick(button, action);

    if(button == MouseButtonType::MOUSE_BUTTON_LEFT && action == MouseActionType::MOUSE_ACTION_PRESS)
    {
        if(GetIsHovered() == true && _callBack != nullptr && _callBackContext != nullptr)
        {
            SetIsToggled(!GetIsToggled());
            _callBack(_callBackContext,GetIsToggled());
        }
    }

    return isControlClicked;
}

void ToggleButton::ToggleButtonRender()
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
    else if(GetIsToggled() == true)
    {
        renderColor = _toggleColor;
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
	
}
