#include "ContainerBase.h"

void ContainerBase::ContainerBaseUpdate()
{
    for(int i=0; i < _children.size() ; ++i)
    {
        _children[i]->Update();
    }
}

void ContainerBase::ContainerBaseRender()
{	
    ControlRender();

    for(int i=_children.size()-1; i>=0 ; --i)
    {
        _children[i]->Render();
    }
}

void ContainerBase::ContainerBaseResize()
{
    ControlResize();

    for(int i=0; i < _children.size() ; ++i)
    {
        _children[i]->Resize();
    }
}

bool ContainerBase::ContainerBaseMouseMove()
{
    bool wasContainerHoveredBeforeMouseMove = GetIsHovered();
    bool isCursorOnControl = IsCursorOnControl(guiContext->currentMousePos);
    if(isCursorOnControl)
    {
        EnableStateBit((uint32_t)ControlStateBitmask::ISHOVERED_BIT);
    }
    else
    {
        DisableStateBit((uint32_t)ControlStateBitmask::ISHOVERED_BIT);
    }
    
    // because mousemove only matters for controls which have been hovered before, or currently are.(this is an optimization)
    if(!(wasContainerHoveredBeforeMouseMove || isCursorOnControl)) return false;

    bool hasAChildHandledMouseMove = false;
    int priorityOfFirstChildThatHandledMouseMove = -1; // is only set when a child handles mousemove
    int childIndex = 0;
    while(childIndex < _children.size() && 
        (!hasAChildHandledMouseMove || priorityOfFirstChildThatHandledMouseMove == _children[childIndex]->GetZPriority()))
    {
        bool doesThisChildHandleMouseMove = _children[childIndex]->MouseMove();
        if(doesThisChildHandleMouseMove && !hasAChildHandledMouseMove)
        {
            hasAChildHandledMouseMove = true;
            priorityOfFirstChildThatHandledMouseMove = _children[childIndex]->GetZPriority();
        }
        ++childIndex;
    }

    return hasAChildHandledMouseMove;
}

bool ContainerBase::ContainerBaseClick(MouseButtonType button, MouseActionType action)
{
    if(GetIsHovered() == true)
    {
        bool hasAChildHandledMouseClick = false;
        int priorityOfFirstChildThatHandledMouseClick = -1;
        int childIndex = 0;
        while(childIndex < _children.size() && 
            (!hasAChildHandledMouseClick || priorityOfFirstChildThatHandledMouseClick == _children[childIndex]->GetZPriority()))
        {
            bool doesThisChildHandleMouseClick = _children[childIndex]->Click(button,action);
            if(doesThisChildHandleMouseClick && !hasAChildHandledMouseClick)
            {
                hasAChildHandledMouseClick = true;
                priorityOfFirstChildThatHandledMouseClick = _children[childIndex]->GetZPriority();
            }
            ++childIndex;
        }

        return hasAChildHandledMouseClick;
    }

    return false;
}

bool ContainerBase::ContainerBaseMouseWheel(float amount, MouseWheelDirection direction)
{
    if(GetIsHovered() == true)
    {
        bool hasAChildHandledMouseWheel = false;
        int priorityOfFirstChildThatHandledMouseClick = -1;
        int childIndex = 0;
        while(childIndex < _children.size() && 
            (!hasAChildHandledMouseWheel || priorityOfFirstChildThatHandledMouseClick == _children[childIndex]->GetZPriority()))
        {
            bool doesThisChildHandleMouseClick = _children[childIndex]->MouseWheel(amount,direction);
            if(doesThisChildHandleMouseClick && !hasAChildHandledMouseWheel)
            {
                hasAChildHandledMouseWheel = true;
                priorityOfFirstChildThatHandledMouseClick = _children[childIndex]->GetZPriority();
            }
            ++childIndex;
        }

        return hasAChildHandledMouseWheel;
    }

    return false;
}

void ContainerBase::ContainerBaseAddControl(Control *control)
{
    _children.push_back(control);
    control->SetParent(this);
    if(this->guiContext != nullptr)
    {
        control->SetGuiContext(this->guiContext); 
    }

    int controlPriority = control->GetZPriority();

    int newControlIndex = _children.size()-1;
    int backwardsFirst = _children.size() - 2;
    while(backwardsFirst >= 0 && _children[backwardsFirst]->GetZPriority() < controlPriority)
    {
        int priorityOfFirst = _children[backwardsFirst]->GetZPriority();
        if(backwardsFirst >= 1)
        {
            int backwardSecond = backwardsFirst - 1;
            int priorityOfSecond = _children[backwardSecond]->GetZPriority();
            
            if(priorityOfFirst < priorityOfSecond)
            {
                // we are at a breakpoint in priority, we must swap our new control into the first slot
                Control* tmp = _children[backwardsFirst];
                _children[backwardsFirst] = _children[newControlIndex];
                _children[newControlIndex] = tmp;
                newControlIndex = backwardsFirst;
            }
        }
        else // backwardsfirst == 0
        {
            Control* tmp = _children[backwardsFirst];
            _children[backwardsFirst] = _children[newControlIndex];
            _children[newControlIndex] = tmp;
            newControlIndex = backwardsFirst;
        }
        
        --backwardsFirst;
    }
}


