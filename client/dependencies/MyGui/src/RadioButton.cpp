#include "RadioButton.h"

void RadioButton::SetIsToggled(bool isToggled)
{
    if(_groupContext != nullptr)
    {
        if(GetIsToggled() == isToggled) return; // no state change

        if(isToggled == true)
        {
            _groupContext->TrySetToggled(this);
        }
        else
        {
            _groupContext->TrySetToggledOff(); //because here we know the toggled is this class
        }

        /*
        if(_groupContext->_currentToggled != nullptr && _groupContext->_currentToggled != this)
        {
            _groupContext->_currentToggled->SetToggledOff();
        }

        _groupContext->_currentToggled = this;
        */
       
    }


}

bool RadioButton::GetIsToggled()
{
    if(this->_groupContext->GetToggled() == this)
    {
        return true;
    }

    return false;
}

void RadioButton::ImitateToggle()
{
    if(_groupContext != nullptr)
    {
        if(GetIsToggled() == false) //cant imitatetoggle if its already toggled
        {
            if(_callBackContext != nullptr && _groupContext->_callBack != nullptr)
            {
                SetIsToggled(true);

                _groupContext->_callBack(_callBackContext,_index);
            }
        }
    } 
}

bool RadioButton::RadioButtonClick(MouseButtonType button, MouseActionType action)
{
    if(button == MouseButtonType::MOUSE_BUTTON_LEFT &&action == MouseActionType::MOUSE_ACTION_PRESS)
    {
        if(GetIsHovered() == true)
        {
            ImitateToggle();

            return true;
        }
    }

    return false;
}

void RadioButton::RadioButtonRender()
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

void RadioButtonGroup::TrySetToggledOff()
{
    this->_currentToggled = nullptr;
}

void RadioButtonGroup::TrySetToggled(RadioButton *radioButton)
{
    if(radioButton->GetGroup() == this) //if in fact the button belongs to this group
    {
        this->_currentToggled = radioButton;
    }
}
