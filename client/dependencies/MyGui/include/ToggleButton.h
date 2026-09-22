#pragma once
#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include "Gui.h"

#include "ButtonBase.h"

enum class ToggleButtonStateBitmask : uint32_t
{
    ISTOGGLED_BIT = (1u << 5u)
};

class ToggleButton : public ButtonBase
{
private:
    //In the case of a simple Button simply a basic handler function is called with the context.
    void(* _callBack)(void*,bool) = nullptr;

    glm::vec3 _toggleColor = glm::vec3(0.3,0.3,0.3);
public:

    bool VirtualClick(MouseButtonType button, MouseActionType action) override {return ToggleButtonClick(button, action);}
    void VirtualRender() override {ToggleButtonRender();}

    void SetCallback(void(*callBack)(void*,bool)){this->_callBack = callBack;}
    void SetIsToggled(bool isToggled);
    void SetToggledColor(float r, float g, float b){this->_toggleColor = glm::vec3(r,g,b);}

    bool GetIsToggled() {return this->_stateBitmask & (uint32_t)ToggleButtonStateBitmask::ISTOGGLED_BIT != 0; }
    
    void ImitateToggle(bool isToggledOn);
protected:

    bool ToggleButtonClick(MouseButtonType button, MouseActionType action);
    void ToggleButtonRender();
};





#endif