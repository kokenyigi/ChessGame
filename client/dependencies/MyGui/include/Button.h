#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include "Gui.h"
#include "ButtonBase.h"

class Button : public ButtonBase
{
private:

    //In the case of a simple Button simply a basic handler function is called with the context.
    void(* _callBack)(void*) = nullptr;


public:
    
    bool VirtualClick(MouseButtonType button, MouseActionType action) override
    {
        return ButtonClick(button, action);
    }

    void SetCallback(void(*callBack)(void*));
protected:

    bool ButtonClick(MouseButtonType button, MouseActionType action);
};





#endif