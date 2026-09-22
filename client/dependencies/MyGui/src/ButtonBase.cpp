#include "ButtonBase.h"

ButtonBase::ButtonBase()
{
    _label.SetSize(SizeType::SIZE_HEIGHT,0.5f,ValueType::RELATIVE);
    _label.SetMargin(MarginType::MARGIN_LEFT,0.0f);
    _label.SetMargin(MarginType::MARGIN_RIGHT,0.0f);
    _label.SetTextAllignment(TextAllignmentType::TEXT_ALLIGNMENT_MID);
    _label.SetParent(this);
}

void ButtonBase::ButtonBaseRender()
{
    ControlRender();

    _label.Render();
}

void ButtonBase::ButtonBaseResize()
{
    ControlResize();

    _label.Resize();
}
