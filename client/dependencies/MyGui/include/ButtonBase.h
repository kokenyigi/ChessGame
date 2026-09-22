#pragma once
#ifndef BUTTONBASE_H
#define BUTTONBASE_H

#include "Gui.h"

#include "Label.h"

class ButtonBase : public Control
{
protected:

    //Each buttonBase subtype has a function that internally uses a context pointer
    void* _callBackContext = nullptr;

    //Each button can have a Label on top of it.
    Label _label;


public:
    ButtonBase();

    void VirtualRender() override
    {
        ButtonBaseRender();
    }

    void VirtualResize() override
    {
        ButtonBaseResize();
    }

    void SetCallBackContext(void* callBackContext) {_callBackContext = callBackContext;}
    inline void* GetCallBackContext() {return _callBackContext;}

    void SetText(const std::string& newText) {_label.SetText(newText);}
    void SetTextSize(float value, ValueType type = ValueType::FIXED) {_label.SetSize(SizeType::SIZE_HEIGHT,value,type);}
    void SetTextColor(float r, float g, float b) {_label.SetTextColor(r,g,b);}
    void SetTextAllignment(TextAllignmentType allignemnt) {_label.SetTextAllignment(allignemnt);}

    std::string GetText() const {return _label.GetText();}
protected:

    //void ButtonBaseUpdate(){}
    void ButtonBaseRender();
    void ButtonBaseResize();
    //void ButtonBaseMouseMove();
    //void ButtonBaseClick(int button, int action);

    void SubControlSetGuicontext(GuiContext* guiContext) override
    {
        _label.SetGuiContext(guiContext);
    }

};




#endif
