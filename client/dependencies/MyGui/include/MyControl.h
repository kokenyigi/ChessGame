#pragma once
#ifndef MYCONTROL_H
#define MYCONTROL_H

#include <vector>

#include "MyOpenGL.h"
#include "Renderer2D.h"
#include "Utils.h"

#include "GuiContext.h"


enum class MarginType
{
	MARGIN_TOP = 0,
	MARGIN_BOTTOM = 1,
	MARGIN_LEFT = 2,
	MARGIN_RIGHT = 3
};

enum class SizeType
{
	SIZE_WIDTH = 0,
	SIZE_HEIGHT = 1
};

enum class ControlStateBitmask : uint32_t
{
	ISHOVERED_BIT = (1u << 0u),
	ISCLICKED_BIT = (1u << 1u),
	ISACTIVE_BIT = (1u << 2u),
	ISFOCUSED_BIT = (1u << 3u),
	ISVISIBLE_BIT = (1u << 4u)
};




class Control
{
protected:
	//We have to have a gui context from which we derive information 
	GuiContext* guiContext = nullptr;

	//Every Control can have a parent, this value being null means no parent
	Control* parent = nullptr;

	//These variables are updated when certain I/O events happen
	uint32_t _stateBitmask = 0u | (uint32_t)ControlStateBitmask::ISVISIBLE_BIT | (uint32_t)ControlStateBitmask::ISACTIVE_BIT;

	// This value can be only modified when the control is added to a container, for safety reasons
	// Controls with higher priority will be drawn over the ones with lower priority
	// Controls with higher priority will get events first, instead of the ones with lower.
	int _zPriority = 0;

	//This variable stores the base background color of the control
	glm::vec3 _baseColor = glm::vec3(0.8,0.0,0.3);
	//This color is used when the mouse hovers above the control
	glm::vec3 _hoverColor = glm::vec3(0.9,0.0,0.4);
	//This value is shown when we click the control, but have not released the mouse yet.
	glm::vec3 _clickColor = glm::vec3(0.5,0.0,0.0);

	//The final color of the base control's rectangle when rendering.
	//glm::vec3 _renderColor = glm::vec3(0.6,0,0.2);

	//These variables store the control's basic box, where it is rendered and where it can be interracted with
	AABB2 _box = {glm::vec2(0,0),glm::vec2(1,1)};

	AABB2 _cutBox = {glm::vec2(0,0),glm::vec2(1,1)};

	
	Texture* _dynamicTexture = nullptr;
	TextureHandle _staticTextureHandle = -1;

	ValueDefinition _marginDefinitions[4];
	ValueDefinition _widthDefinition;
	ValueDefinition _heightDefinition;

public:
	Control();
	virtual ~Control();

	void Update(){VirtualUpdate(); }

	void Render()
	{
		//std::cout<<"Controlrender claaed\n";
		if(GetIsActive())
		{
			VirtualRender();
		}
	}

	void Resize() {VirtualResize();}

	bool MouseMove()
	{
		if(!GetIsFocused() && GetIsActive())
		{
			return VirtualMouseMove();
		}
		return false;
	}

	bool Click(MouseButtonType buttonType, MouseActionType actionType)
	{
		if(!GetIsFocused() && GetIsActive())
		{
			return VirtualClick(buttonType,actionType);
		}

		return false;
	}

	//TODO:change this to bool
	bool MouseWheel(float amount, MouseWheelDirection direction)
	{
		if(!GetIsFocused() && GetIsActive())
		{
			return VirtualMouseWheel(amount,direction);
		}

		return false;
	}

	//Maybe we should do this a bit differently, idk.
	void KeyInput(int key,int action, int mods)
	{
		if(!GetIsFocused() && GetIsActive())
		{
			VirtualKeyInput(key,action,mods);
		}
	}


	virtual void VirtualUpdate() {ControlUpdate();}
	virtual void VirtualRender() {ControlRender();}
	virtual void VirtualResize() {ControlResize();}
	virtual bool VirtualMouseMove() {return ControlMouseMove();}
	virtual bool VirtualClick(MouseButtonType button, MouseActionType action) {return ControlClick(button,action);}
	virtual bool VirtualMouseWheel(float amount, MouseWheelDirection direction) {/*Control doesn't do shit on mousewheel.*/ return false;}
	virtual void VirtualKeyInput(int key, int action, int mods) {/* A Control doesn't do anything when keypress happens.*/}
	

	
	//Kinda weird thing, brokes some SOLID stuff, whateva
	//This function calls a virtual subfunction,
	//which propagates the change in guicontext ptr to potential children of the Control.
	virtual void SetGuiContext(GuiContext* guiContext)
	{
		this->guiContext = guiContext;

		SubControlSetGuicontext(guiContext);
		Resize();
	}

	//Sets the parent of the Control.
	void SetParent(Control* parent);

	//Sets the base background color of the Control.
	void SetBaseColor(float r, float g, float b);
	void SetHoverColor(float r, float g, float b);
	void SetClickColor(float r, float g, float b);
	void SetDynamicTexture(Texture* dynamicTexture);
	void SetStaticTexture(TextureHandle staticTexture);

	//Sets the width/height of the Control.
	// Margins will be invalidated if called in a given way.
	void SetSize(SizeType sizeType, float value, ValueType type = ValueType::FIXED);

	//Sets the margin of the Control in a specific direction.
	//If Both top-bottom or both left-right margins are set, the height or width of the Control will be invalidated.
	void SetMargin(MarginType margin, float value, ValueType type = ValueType::FIXED);

	void SetRelativePlacement(const glm::vec2& minPos,const glm::vec2& maxPos);


	void SetIsActive(bool isActive);
	void SetIsHovered(bool isHovered);
	void SetIsClicked(bool isClicked);
	void SetIsFocused(bool isFocused);
	void SetIsVisible(bool isVisible);
	void SetZPriority(int newZPriority){_zPriority = newZPriority;}

	

	inline AABB2 GetBox() const {return this->_box;}
	inline AABB2 GetCutBox() const {return this->_cutBox;}
	inline ValueDefinition GetMarginDefinition(MarginType type) const {return _marginDefinitions[(int)type];}
	inline ValueDefinition GetWidthDefinition() const {return this->_widthDefinition;}
	inline ValueDefinition GetHeightDefinition() const {return this->_heightDefinition;}

	bool GetIsHovered() const { return (this->_stateBitmask & (uint32_t)ControlStateBitmask::ISHOVERED_BIT) != 0u; }
	bool GetIsClicked() const { return (this->_stateBitmask & (uint32_t)ControlStateBitmask::ISCLICKED_BIT) != 0u; }
	bool GetIsActive() const { return (this->_stateBitmask & (uint32_t)ControlStateBitmask::ISACTIVE_BIT) != 0u; }
	bool GetIsFocused() const { return (this->_stateBitmask & (uint32_t)ControlStateBitmask::ISFOCUSED_BIT) != 0u; }
	bool GetIsVisible() const  {return (this->_stateBitmask & (uint32_t)ControlStateBitmask::ISVISIBLE_BIT) != 0u; }

	inline int GetZPriority() const {return _zPriority;}
	inline glm::vec3 GetBgColor() const {return _baseColor;}
	inline Control* Getparent() const {return this->parent;}

private:
	//A control doesn't have any exclusive private methods, cuz why would it have..idk.
protected:

	//This method recalculates the minimum and maximum positions of the Control.
	//Its best practice to call this each time the control is resized, or its variables are tinkerred with.
	void RecalculatePosition();

	
	void CalculateCutBox();
	
	void ControlUpdate();
	void ControlRender();
	void ControlResize();
	bool ControlClick(MouseButtonType button, MouseActionType action);
	bool ControlMouseMove();

	float GetDepth() {return (float)this->_zPriority / 256.0f;}
	bool DoesCutBoxHaveArea();

	virtual void SubControlSetGuicontext(GuiContext* guiContext){}
	virtual void SubControlSetIsActive(bool isActive){};
	virtual void SubControlTurnOffHovered(){}
	virtual void SubControlTurnOffClicked(){}
	virtual void SubControlTurnOffFocused(){}

	void EnableStateBit(uint32_t bitmask) {this->_stateBitmask |= bitmask;}
	void DisableStateBit(uint32_t bitmask) {this->_stateBitmask &= ~bitmask;}

	bool IsCursorOnControl(const glm::vec2& cursorPos);
	float CalculateValuebasedOnType(ValueType type, float value,int axis);
	float CalculateValuebasedOnType(ValueType type, float value,float relativeTo);

	Control* GetOldestParent();
	void TrySetAsTopLevelControl();
};

#endif