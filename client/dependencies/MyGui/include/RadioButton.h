#pragma once
#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include "ButtonBase.h"

//forward declaration
class RadioButtonGroup;

class RadioButton : public ButtonBase
{
protected:

    // Radiobuttons dont come alone, and this context is whats keeping them together
    RadioButtonGroup* _groupContext = nullptr;

    //The index of a RadioButton makes the callback behave potentially differently when called.
    int _index = 0;

    glm::vec3 _toggleColor = glm::vec3(0.3,0.3,0.3);

public:

    bool VirtualClick(MouseButtonType button, MouseActionType action) override
    {
        return RadioButtonClick(button,action);
    }

    void VirtualRender() override
    {
        RadioButtonRender();
    }

    
    void SetIndex(int index) {_index = index;}
    void SetGroup(RadioButtonGroup* context) {_groupContext = context;}
    void SetIsToggled(bool isToggled);
    void SetToggleColor(float r, float g, float b) {this->_toggleColor = glm::vec3(r,g,b);} 

    inline int GetIndex(){return _index;}
    inline RadioButtonGroup* GetGroup() {return this->_groupContext;}
    bool GetIsToggled();

    void ImitateToggle();

protected:
        
    bool RadioButtonClick(MouseButtonType button, MouseActionType action);
    void RadioButtonRender();
    //rest is the same as a buttonbase & control
};


/**
 * A class that holds the radiobuttons shared context
 */
class RadioButtonGroup
{
private:

    // The currently toggled radiobutton of the group
    RadioButton* _currentToggled = nullptr;

    //The Main callback is called with the specified index when the radiobutton is clicked.
   
    
public:

    void (*_callBack)(void* , int) = nullptr; 
    

    void AddToGroup(RadioButton* radioButton)
    {
        radioButton->SetGroup(this);
    }

    void RemoveFromGroup(RadioButton* radioButton)
    {
        radioButton->SetGroup(nullptr);
    }

    void TrySetToggled(RadioButton* radioButton);
    void TrySetToggledOff();

    void SetCallback(void(*callBack)(void*,int)) {_callBack = callBack;} 

    inline RadioButton* GetToggled(){return this->_currentToggled;}
    
    
private:

};


#endif
