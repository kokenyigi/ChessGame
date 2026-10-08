#include "TextInput.h"

TextInput::TextInput()
{
    _textLabel.SetSize(SizeType::SIZE_HEIGHT,0.5f,ValueType::RELATIVE);
    _textLabel.SetMargin(MarginType::MARGIN_LEFT,5.0f);
    _textLabel.SetMargin(MarginType::MARGIN_RIGHT,5.0f);
    _textLabel.SetTextAllignment(TextAllignmentType::TEXT_ALLIGNMENT_LEFT);
    _textLabel.SetText("[null]");

    _textLabel.SetParent(this);
}

void TextInput::TextInputResize()
{
    ControlResize();

    _textLabel.Resize();

    RecalculateEditHead();    
}

void TextInput::TextInputRender()
{
    Rectangle ndcControlRenderRect = {_cutBox.min.x,_cutBox.min.y,
									  _cutBox.max.x - _cutBox.min.x,
									  _cutBox.max.y-_cutBox.min.y};

	guiContext->renderer.RenderRectangleInstanced(ndcControlRenderRect,_baseColor,this->GetDepth());

    _textLabel.Render();

    
    if(GetIsFocused() == true)
    {
        AABB2 editHeadRenderBox;
        bool doesEditHeadIntersectRenderBox = IntersectAABB(_cutBox,_editHeadBox,editHeadRenderBox);
        if(doesEditHeadIntersectRenderBox)
        {
            AABB2 editHeadSrcBox = GetRectangleOfCharacter('|');

            AABB2 transformedEditHeadSrcBox = editHeadSrcBox;
            TransformBasedOnTwoRelativeAABB(_editHeadBox,editHeadRenderBox,transformedEditHeadSrcBox);

            Rectangle editHeadDstRect = {editHeadRenderBox.min.x,editHeadRenderBox.min.y,
                editHeadRenderBox.max.x-editHeadRenderBox.min.x,
                editHeadRenderBox.max.y-editHeadRenderBox.min.y};

            Rectangle editHeadSrcRect = {transformedEditHeadSrcBox.min.x,transformedEditHeadSrcBox.min.y,
                transformedEditHeadSrcBox.max.x-transformedEditHeadSrcBox.min.x,
                transformedEditHeadSrcBox.max.y-transformedEditHeadSrcBox.min.y};

            guiContext->renderer.RenderColoredTexturedRectangleInstanced(editHeadDstRect,editHeadSrcRect,
                _editHeadColor,guiContext->alphabetTextureHandles[guiContext->currentAlphabetIndex],this->GetDepth());
        }
    }
}

bool TextInput::TextInputClick(MouseButtonType button, MouseActionType action)
{
    if(button == MouseButtonType::MOUSE_BUTTON_LEFT)
	{
		if(action == MouseActionType::MOUSE_ACTION_PRESS)
		{
			if(GetIsHovered() == true)
			{
				//isClicked = true;

                if(GetIsFocused() == false)
                {
                    SetIsFocused(true);

                    _toRollbackText = _textLabel.GetText();
                    SetEditHeadAt(_toRollbackText.length());
                }

                return true;
			}
			else
			{
				//isClicked = false;

                if(GetIsFocused() == true)
                {
                    SetIsFocused(false);

                    //_toRollbackText = "";

                    TryFinalizeText();
                }
			}
		}
		else
		{
			//isClicked = false;
		}
	}

    return false;
}

void TextInput::TextInputKeyInput(int key,int action, int mods)
{
    if(GetIsFocused() == false) return;

    if(key == 'Y')
    {
        key = 'Z';
    }
    else if(key == 'Z')
    {
        key = 'Y';
    }
    else if(key=='`')
    {
        key = '0';
    }
    else if(key == '/')
    {
        key = '-';	
    }

    if(key>='A' && key <='Z' && !(mods & 0x0001 || mods & 0x0010))
    {
        key += 'a' - 'A';
        
    }

    if(action != 0)
    {
        if(key >= 32 && key <=126)
        {
            //Character keys
            TryInsertCharacterAtHead(key);
        }
        else
        {
            if(key == 257) // ENTER
            {
                SetIsFocused(false);
                //_toRollbackText = "";
                TryFinalizeText();
            }
            else if(key == 259) //BACKSPACE
            {
                TryDeleteCharacterAtHead();
            }
            else if(key == 262) //Right key
            {
                TryMoveEditHeadRight();
            }
            else if(key == 263) // Left key
            {
                TryMoveEditHeadLeft();
            }
        }
    }
    
}


void TextInput::CalculateLabelProperties()
{
    //As with the current settings these variables store the discrate interval of substring indices that are visible
    int visibleBoundaryLeft;
    int visibleBoundaryRight;
    
    _textLabel.GetFullyVisibleInterval(visibleBoundaryLeft,visibleBoundaryRight);
    //std::cout<<"Text length: "<<_textLabel.GetText().length() << "\n";
    //std::cout<<"Calculated visible spectrum: "<<visibleBoundaryLeft << " " << visibleBoundaryRight << '\n';
    //std::cout<<"EditheadIndex: "<<_editHeadIndex << "\n";

    TextAllignmentType textAllignment = _textLabel.GetTextAllignmentType();


    

    if(_editHeadIndex >= visibleBoundaryLeft && _editHeadIndex <=visibleBoundaryRight + 1)
    {
        //We dont have to alter any properties of the label here, since edithead is in the right interval
    }
    else
    {
        if(_editHeadIndex < visibleBoundaryLeft )
        {
            if(textAllignment != TextAllignmentType::TEXT_ALLIGNMENT_LEFT)
            {
                _textLabel.SetTextAllignment(TextAllignmentType::TEXT_ALLIGNMENT_LEFT);
            }

            //Set proper renderstart index
            _textLabel.SetRenderStartIndex(_editHeadIndex);
        }
        else if(_editHeadIndex > visibleBoundaryRight + 1)
        {
            if(textAllignment != TextAllignmentType::TEXT_ALLIGNMENT_RIGHT)
            {
                _textLabel.SetTextAllignment(TextAllignmentType::TEXT_ALLIGNMENT_RIGHT);
            }

            int textLength = _textLabel.GetText().length();
            _textLabel.SetRenderStartIndex(textLength - _editHeadIndex);
            //std::cout<<"New RenderStartIndex: " << textLength - _editHeadIndex<<"\n";
            
        }
        else
        {
            //Should Not reach This
        }
    }

    
}

void TextInput::SetEditHeadAt(int newIndex)
{
    _editHeadIndex = newIndex;

    RecalculateEditHead();
}

void TextInput::RecalculateEditHead()
{
    if(guiContext == nullptr) return;

    CalculateLabelProperties();

    AABB2 labelBoxRelative = _textLabel.GetBox();
    glm::vec2 labelMinRelative = labelBoxRelative.min;
    glm::vec2 labelMaxRelative = labelBoxRelative.max;
    float labelHeightRelative = labelMaxRelative.y - labelMinRelative.y;
    
    float charWidthBasedOnHeightRelative =labelHeightRelative * guiContext->windowSize.y / guiContext->windowSize.x;

    float textLengthRelative = _textLabel.GetTextRelativeLength() * charWidthBasedOnHeightRelative ;
    float labelWidthRelative = labelMaxRelative.x - labelMinRelative.x;

    //std::cout<<"Text length = "<<textLengthRelative<< " LabelWidth = " << labelWidthRelative << "\n";
    //Lets do a quick check, if after deletion the string size left after deletion is too short we change stuff up
    std::string currentlyVisibleText = _textLabel.GetText();
    if(_textLabel.GetTextAllignmentType() == TextAllignmentType::TEXT_ALLIGNMENT_LEFT)
    {
        float accumDistFromBackRelative = 0.0f;
        for(int i=currentlyVisibleText.length() - _textLabel.GetRenderStartIndex() -1;i>=0;--i)
        {
            char c = currentlyVisibleText[i];
            if(c != 'M' && c!= 'W' && c!= 'm' && c!= 'w')
            {
                accumDistFromBackRelative += charWidthBasedOnHeightRelative * 0.5f;
            }
            else
            {
                accumDistFromBackRelative += charWidthBasedOnHeightRelative * 0.75f;
            }
        }

        
        if(accumDistFromBackRelative < labelWidthRelative)
        {
            _textLabel.SetTextAllignment(TextAllignmentType::TEXT_ALLIGNMENT_LEFT);
            _textLabel.SetRenderStartIndex(0);
        }
    }

    /*
    if(textLengthRelative <= labelWidthRelative)
    {
        //The text is fully visible
        if(_textLabel.GetTextAllignmentType() != TEXT_ALLIGNMENT_LEFT)
        {
            _textLabel.SetTextAllignment(TEXT_ALLIGNMENT_LEFT);
        }
        _textLabel.SetRenderStartIndex(0);
    }
    */

    //Lets calculate The EditHead box, and where it should go.
    //First, this variable stores the reltive float distance from either left/right side of the label
    //the side is determined by the text allignment.
    float editHeadMiddleRelative = 0.0f;

    

    int renderStartIndex = _textLabel.GetRenderStartIndex();
    TextAllignmentType labelAllignment =  _textLabel.GetTextAllignmentType();
    if(labelAllignment == TextAllignmentType::TEXT_ALLIGNMENT_RIGHT)
    {
        //We have to calculate the distance to put the head at from the back.
        float accumDistFromBackRelative = 0.0f;
        for(int i=currentlyVisibleText.length() - renderStartIndex -1;i>=_editHeadIndex;--i)
        {
            char c = currentlyVisibleText[i];
            if(c != 'M' && c!= 'W' && c!= 'm' && c!= 'w')
            {
                accumDistFromBackRelative += charWidthBasedOnHeightRelative * 0.5f;
            }
            else
            {
                accumDistFromBackRelative += charWidthBasedOnHeightRelative * 0.75f;
            }
        }

        editHeadMiddleRelative = labelMaxRelative.x - accumDistFromBackRelative;
    }
    else
    {
        //calculate head from front.
        float accumDistFromBackRelative = 0.0f;
        for(int i=renderStartIndex;i<_editHeadIndex;++i)
        {
            char c = currentlyVisibleText[i];
            if(c != 'M' && c!= 'W' && c!= 'm' && c!= 'w')
            {
                accumDistFromBackRelative += charWidthBasedOnHeightRelative * 0.5f;
            }
            else
            {
                accumDistFromBackRelative += charWidthBasedOnHeightRelative * 0.75f;
            }
        }

        editHeadMiddleRelative = labelMinRelative.x + accumDistFromBackRelative;
    }

    float widthOfEditHeadRelative = charWidthBasedOnHeightRelative * 0.5f;
    float heightOfEditHeadRelative = labelHeightRelative;

    _editHeadBox.min.x = editHeadMiddleRelative - widthOfEditHeadRelative * 0.5f;
    _editHeadBox.min.y = labelMinRelative.y;
    _editHeadBox.max.x = editHeadMiddleRelative + widthOfEditHeadRelative * 0.5f;
    _editHeadBox.max.y = labelMaxRelative.y;


}

void TextInput::TryMoveEditHeadLeft()
{
    int newIndex = _editHeadIndex - 1;
    if(newIndex >= 0)
    {
        SetEditHeadAt(newIndex);
    }
}

void TextInput::TryMoveEditHeadRight()
{
    int newIndex = _editHeadIndex + 1;
    int textLength = _textLabel.GetText().length();
    if(newIndex <= textLength)
    {
        SetEditHeadAt(newIndex);
    }
}

void TextInput::TryFinalizeText()
{
    if(_textLabel.GetTextAllignmentType() != TextAllignmentType::TEXT_ALLIGNMENT_LEFT)
    {
        _textLabel.SetTextAllignment(TextAllignmentType::TEXT_ALLIGNMENT_LEFT);
    }

    _textLabel.SetRenderStartIndex(0);

    bool isCurrentlyPresentTextAccepted = VirtualTextAnalyzer();
    if(isCurrentlyPresentTextAccepted)
    {
        if(_callbackContext != nullptr && _callback != nullptr)
        {
            _callback(_callbackContext,_textLabel.GetText());
        }
    }
    else
    {
        _textLabel.SetText(_toRollbackText);
    }

    _toRollbackText = "";
}

void TextInput::TryInsertCharacterAtHead(char c)
{
    bool isCharAllowed = VirtualIsCharAllowed(c);
    if(!isCharAllowed) return;

    std::string currentText = _textLabel.GetText();

    int insertionIndex = _editHeadIndex;

    if(insertionIndex >= currentText.length())
    {
        currentText.append(1,c);
    }
    else
    {
        currentText.insert(insertionIndex,1,c);
    }

    _textLabel.SetText(currentText);
    ++_editHeadIndex;

    RecalculateEditHead();
}

void TextInput::TryDeleteCharacterAtHead()
{
    std::string currentText = _textLabel.GetText();

    if(_editHeadIndex != 0)
    {
        int deletionIndex = _editHeadIndex-1;

        currentText.erase(deletionIndex,1);

        --_editHeadIndex;

        _textLabel.SetText(currentText);

        RecalculateEditHead();
    }
}
