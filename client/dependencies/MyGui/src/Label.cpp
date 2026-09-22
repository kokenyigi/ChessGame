#include "Label.h"

#include <iostream>

void Label::SetText(const std::string &newText)
{
    _text = newText;
    CalculateRelativeTextLength();
}

void Label::SetTextColor(float r, float g, float b)
{
    _textColor = glm::vec3(r,g,b);
}

void Label::SetTextAllignment(TextAllignmentType type)
{
    _textAllignmentType = type;
}

void Label::SetRenderStartIndex(int index)
{
    _renderStartIndex = index;
}

void Label::GetFullyVisibleInterval(int &outStart, int &outEnd)
{
    float renderStartPos = 0.0f;
    float labelHeight = _box.max.y - _box.min.y;
    float labelWidth = _box.max.x - _box.min.x;

    float charWidthBasedOnHeight =labelHeight * guiContext->windowSize.y / guiContext->windowSize.x;

    if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_LEFT)
    {
        renderStartPos = _box.min.x;
    }
    else if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT)
    {
        renderStartPos = _box.max.x;
        //plus other stuff, like rendering backwards, aaaand thats it yeah
    }

    int textLength = _text.length();

    outStart = _textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT ? -1 : _renderStartIndex; 
    outEnd = _textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT ? textLength-1 - _renderStartIndex : -1; 


    float accumulatedWidth = 0.0f;
    bool hasFoundNonFullyVisible = false;
    int i = _renderStartIndex;
    while(i<_text.length() && !hasFoundNonFullyVisible)
    {
        int drawCharIndex = _textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT? _text.length() - 1 - i:i;

        float currWidth = 0.0f;
        char c = _text[drawCharIndex];
        if(c != 'M' && c!= 'W' && c!= 'm' && c!= 'w')
        {
            currWidth = charWidthBasedOnHeight * 0.5f;
        }
        else
        {
            currWidth = charWidthBasedOnHeight * 0.75f;
        }

        //std::cout<<"ran\n " << accumulatedWidth;
        accumulatedWidth += currWidth;
        if(accumulatedWidth < labelWidth)
        {
            if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_LEFT)
            {
                outEnd = drawCharIndex;
            }
            else
            {
                outStart = drawCharIndex;
            }
        }
        else
        {
            hasFoundNonFullyVisible = true;
        }

        ++i;
    }    
}

void Label::LabelRender()
{
    if(this->GetIsVisible() == false) return;

	bool doesCutBoxHaveAnyArea = this->DoesCutBoxHaveArea();
	if(!doesCutBoxHaveAnyArea) return;

    float renderStartPos = 0.0f;
    float labelHeight = _box.max.y - _box.min.y;
    float labelWidth = _box.max.x - _box.min.x;

    float charWidthBasedOnHeight =labelHeight * guiContext->windowSize.y / guiContext->windowSize.x;

    if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_LEFT)
    {
        renderStartPos = _box.min.x;
    }
    else if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_MID)
    {
        float midPoint = (_box.max.x + _box.min.x) * 0.5f;
        renderStartPos = midPoint - charWidthBasedOnHeight * relativeTextLength * 0.5f;
    }
    else if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT)
    {
        renderStartPos = _box.max.x;
        //plus other stuff, like rendering backwards, aaaand thats it yeah
    }


    
    
    AABB2 charDstBox = {glm::vec2(renderStartPos,_box.min.y),glm::vec2(renderStartPos,_box.min.y + labelHeight)};

    Rectangle srcRect;
    Rectangle dstRect;
    for(int i=_renderStartIndex;i<_text.length();++i)
    {   
        int drawCharIndex = _textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT? _text.length() - 1 - i:i;
        float currWidth = 0.0f;
        char c = _text[drawCharIndex];
            if(c != 'M' && c!= 'W' && c!= 'm' && c!= 'w')
            {
                currWidth = charWidthBasedOnHeight * 0.5f;
            }
            else
            {
                currWidth = charWidthBasedOnHeight * 0.75f;
            }

        if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT)
        {
            charDstBox.min.x = charDstBox.max.x - currWidth;
        }
        else
        {
            charDstBox.max.x = charDstBox.min.x + currWidth;
        }

        
        AABB2 charSrcBox = GetRectangleOfCharacter(c);

        AABB2 clippedDstBox;
        AABB2 clippedSrcBox = charSrcBox;

        bool doesCharIntersectLabel = IntersectAABB(charDstBox,_cutBox,clippedDstBox);
        if(doesCharIntersectLabel)
        {

            
            TransformBasedOnTwoRelativeAABB(charDstBox,clippedDstBox,clippedSrcBox);

            dstRect = {clippedDstBox.min.x,clippedDstBox.min.y,clippedDstBox.max.x - clippedDstBox.min.x, clippedDstBox.max.y - clippedDstBox.min.y};
            srcRect = {clippedSrcBox.min.x,clippedSrcBox.min.y,clippedSrcBox.max.x - clippedSrcBox.min.x,clippedSrcBox.max.y - clippedSrcBox.min.y};

            //Rectangle testRect = {charDstBox.min.x, charDstBox.min.y,charDstBox.max.x - charDstBox.min.x, charDstBox.max.y-charDstBox.min.y};
           
            //guiContext->renderer.RenderRectangle(srcRect,glm::vec3(0.5,0.1,0.8));
            guiContext->renderer.RenderColoredTexturedRectangleInstanced(dstRect,srcRect,_textColor,
                guiContext->alphabetTextureHandles[guiContext->currentAlphabetIndex],this->GetDepth());
        }

        if(_textAllignmentType == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT)
        {
            charDstBox.max.x -= currWidth;
        }
        else
        {
            charDstBox.min.x += currWidth;
        }
        
    }
}

void Label::CalculateRelativeTextLength()
{
    float accumlength = 0.0f;
    for(int i= 0;i<_text.length();++i)
    {
        char c  = _text[i];
        if(c != 'M' && c!= 'W' && c!= 'w' && c!= 'm')
        {
            accumlength += 0.5f;
        }
        else
        {
            accumlength += 0.75f;
        }
    }

    relativeTextLength = accumlength;
}

/*
AABB Label::GetRectangleOfCharacter(char c)
{
    AABB retval = {glm::vec2(0.0f,0.0f),glm::vec2(0.1f,0.1f)};

    if(c >= ' ' && c<= '~')
    {    
        if(c != 'M' && c!= 'W' && c!= 'w' && c!= 'm')
        {
            int charDiff = c - ' ' - (char)1;

            float x = 0.1f * (charDiff % 10);
            float y = 0.1f * (charDiff / 10);

            retval.min = glm::vec2(x,y);
            retval.max = retval.min + glm::vec2(0.1f,0.1f);
        }
        else
        {
            if(c== 'M')
            {
                retval.min=glm::vec2(0.4f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'W')
            {
                retval.min=glm::vec2(0.55f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'm')
            {
                retval.min=glm::vec2(0.70f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'w')
            {
                retval.min=glm::vec2(0.85f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
        }
    }

    return retval;
}
*/