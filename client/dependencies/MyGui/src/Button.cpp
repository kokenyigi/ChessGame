#include "Button.h"


void Button::SetCallback(void (*callBack)(void *))
{
    _callBack = callBack;
}

bool Button::ButtonClick(MouseButtonType button, MouseActionType action)
{
    bool isControlClicked = ControlClick(button, action);

    if(button == MouseButtonType::MOUSE_BUTTON_LEFT && action == MouseActionType::MOUSE_ACTION_PRESS)
    {
        if(GetIsHovered() == true)
        {
            if(_callBack != nullptr && _callBackContext != nullptr)
            {
                _callBack(_callBackContext);   
            }
        }
    }
   

    return isControlClicked;
}
