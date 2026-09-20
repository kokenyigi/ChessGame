#pragma once
#ifndef CONTAINERBASE_H
#define CONTAINERBASE_H

#include "Gui.h"
#include <vector>

class ContainerBase : public Control
{
protected:
    std::vector<Control* > _children;

public:

    void VirtualUpdate() override {ContainerBaseUpdate();}
    void VirtualRender() override {ContainerBaseRender();}
    void VirtualResize() override {ContainerBaseResize();}
    bool VirtualMouseMove() override {return ContainerBaseMouseMove();}
    bool VirtualClick(MouseButtonType button, MouseActionType action) override {return ContainerBaseClick(button,action);}
    bool VirtualMouseWheel(float amount, MouseWheelDirection direction) override {return ContainerBaseMouseWheel(amount,direction);}

    virtual void AddControl(Control* control)
    {   
        ContainerBaseAddControl(control);
    }
    
    std::vector<Control*>& GetChildren() {return _children;}

protected:
    void ContainerBaseUpdate();
    void ContainerBaseRender();
    void ContainerBaseResize();
    bool ContainerBaseMouseMove();
    bool ContainerBaseClick(MouseButtonType button, MouseActionType action);
    bool ContainerBaseMouseWheel(float amount, MouseWheelDirection direction);
    
    void ContainerBaseAddControl(Control* control);

    void SubControlSetGuicontext(GuiContext* guicontext) override
    {
        //std::cout<<"Children count: "<<_children.size() << "\n";
        for(int i=0;i<_children.size();++i)
        {
            //std::cout<<"Setting Child's guicontext\n";
            _children[i]->SetGuiContext(guicontext);

            
        }
    }
};


#endif