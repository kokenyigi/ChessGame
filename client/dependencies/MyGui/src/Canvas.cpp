#include "Canvas.h"

void Canvas::CanvasResize()
{
    ControlResize();

    if(_resizeCallback != nullptr && _callbackContext != nullptr)
    {
        int newWidth = (int)((_box.max.x - _box.min.x) * guiContext->windowSize.x);
        int newHeight = (int)((_box.max.y - _box.min.y) * guiContext->windowSize.y);

        _resizeCallback(_callbackContext,newWidth,newHeight);
    }
}

void Canvas::CanvasRender()
{
    if(_renderCallback != nullptr && _callbackContext != nullptr)
    {
        _renderCallback(_callbackContext);
    }

    ControlRender();
}

void Canvas::CanvasUpdate()
{
    ControlUpdate();

    if(_updateCallback != nullptr && _callbackContext != nullptr)
    {
        _updateCallback(_callbackContext,guiContext->deltaTime);
    }
}

bool Canvas::CanvasMouseMove()
{
    bool isMouseOnControl = ControlMouseMove();

    if(_mouseMoveCallback != nullptr && _callbackContext != nullptr)
    {
        //Relative to the fixedbox of canvas
        glm::vec2 windowRes = this->guiContext->windowSize;
        glm::vec2 fixedMin = _box.min * windowRes;
        glm::vec2 newMouseRelativeToBox = guiContext->currentMousePos - fixedMin;
        float newMouseX = newMouseRelativeToBox.x;
        float newMouseY = newMouseRelativeToBox.y;

        _mouseMoveCallback(_callbackContext,newMouseX,newMouseY);
    }

    return isMouseOnControl;
}

bool Canvas::CanvasMouseClick(MouseButtonType button, MouseActionType action)
{
    //focus stuff
    //bool isMouseOnControl = IsCursorOnControl(guiContext->currentMousePos);

    
    if(button == MouseButtonType::MOUSE_BUTTON_LEFT && action == MouseActionType::MOUSE_ACTION_PRESS)
    {
        if(GetIsHovered())
        {
            if(GetIsFocused() == false)
            {
                SetIsFocused(true);
            }
        }
        else 
        {
            if(GetIsFocused() == true)
            {
                SetIsFocused(false);
            }
        }
    }
    

    if(_mouseClickCallback != nullptr && _callbackContext  != nullptr)
    {
        _mouseClickCallback(_callbackContext,button,action);
    }

    return GetIsHovered();
}

bool Canvas::CanvasMouseWheel(float amount, MouseWheelDirection direction)
{
    if(this->GetIsHovered() == true || this->GetIsFocused() == true)
    {
        if(this->_callbackContext != nullptr && this->_mouseWheelCallback != nullptr)
        {
            this->_mouseWheelCallback(_callbackContext,amount,direction);
        }
        return true;
    }

    return false;
}

void Canvas::CanvasKeyInput(int key,int action, int mods)
{
    if(!GetIsFocused()) return;

    if(_keyCallback!=nullptr && _callbackContext != nullptr)
    {
        _keyCallback(_callbackContext,key,action,mods);
    }
}