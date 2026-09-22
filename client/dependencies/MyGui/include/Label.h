#pragma once
#ifndef LABEL_H
#define LABEL_H

#include <string>

#include "Gui.h"


enum class TextAllignmentType
{
    TEXT_ALLIGNMENT_LEFT = 0,
    TEXT_ALLIGNMENT_MID = 1,
    TEXT_ALLIGNMENT_RIGHT = 2
};

class Label : public Control
{

protected:
    //Every Label has its own string internally stored.
    std::string _text = "";

    //Helper variable, is recalculated each time the text has been tempered with.
    //Normal characters => add 1 to this value
    //M,W,m,w => add 1.5f to this value
    float relativeTextLength = 0.0f;

    //Every Label stores the way its text is rendered in.
    TextAllignmentType _textAllignmentType = TextAllignmentType::TEXT_ALLIGNMENT_MID;

    //By default each Label stores the color its text should be rendered in.
    glm::vec3 _textColor = glm::vec3(0.7,0.0,0.3);

    //Rendering starts from this index
    int _renderStartIndex = 0;


public:
    Label(){}
    ~Label(){}
    
    void VirtualRender() override
    {
        LabelRender();
    }

    void SetText(const std::string& newText);
    void SetTextColor(float r, float g, float b);
    void SetTextAllignment(TextAllignmentType type);

    void SetRenderStartIndex(int index);

    std::string GetText() const {return _text;}
    TextAllignmentType GetTextAllignmentType() const {return _textAllignmentType;}
    int GetRenderStartIndex() const {return _renderStartIndex;}

    /**
     * This function determines the first and last fully visible characters indices, based on its internal variables.
     * Returns whether or not the the whole string is visible
     */
    void GetFullyVisibleInterval(int& outStart, int& outEnd);
    float GetTextRelativeLength() const {return relativeTextLength;}

protected:
    void LabelRender();

    //Helper Label functions
    void CalculateRelativeTextLength();
    //AABB GetRectangleOfCharacter(char c);
};



#endif