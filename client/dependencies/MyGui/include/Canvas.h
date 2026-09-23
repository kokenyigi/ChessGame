#pragma once
#ifndef CANVAS_H
#define CANVAS_H

#include "Gui.h"

/**
 * The Canvas is basically a Control that has function pointers for all kinds of events, 
 * since its basically a mediator between the GUI system and an independet Model of some sorts(e.g.: Pathtracer Scene).
 * The Canvas is the first Control that renders a texture as its background
 */
class Canvas : public Control
{
protected:

    // Shared Callback context between all of the callbacks
    void* _callbackContext = nullptr;

    //Has a callback for all kinds of things
    /**
     * This callback is called when the Canvas gets resized, 
     * and gets passed the new Width and Height as extra parameters
     */
    void (*_resizeCallback)(void*,int, int) = nullptr;

    /**
     * This is just a callback which basically says: Hey! The canvas got a render ping, some rendering should be done.
     * In The Pathtracer app, it will be used to mainly call the pathtracer's GPU-side kernel code -> and render the scene
     */
    void (*_renderCallback)(void*) = nullptr;

    /**
     * This callback is the one thats called when the Canvas gets an update tick, with the current deltatime.
     * This deltatime is then passed to the scene, and this way deltatime dependent updates happen inside it like:
     * Camera movement.
     */
    void (*_updateCallback)(void* , float) = nullptr;

    /**
     * This callback is called when the Canvas recieves a mouseMove tick.
     * The Canvas will pass the new mouse x and y coordinates as parameters (IDK what coordinate-system)
     */
    void (*_mouseMoveCallback)(void* ,float ,float) = nullptr;

    /**
     * This callback is called when the Canvas recieves a mouseClick tick.
     * It will be called with first arg button (0 == left / 1 == right)
     * and second arg action (0 == down / 1 == up)
     */
    void (*_mouseClickCallback)(void*,MouseButtonType,MouseActionType) = nullptr;


    /**
     * This callback is called when the canvas recieves a mousewheel event tick
     * It will be called with the scrolled amount and the direction (1,-1)
     */
    void (*_mouseWheelCallback)(void*,float,MouseWheelDirection) = nullptr;

    /**
     * This callback is called when the Canvas recieves a key input
     * first arg is the key (32-96 ascii + different key, check glfw3 doc)
     * second arg is the action (0 == press / 1 == release)
     * third arg is mods bitmask (check glfw3 for that, contains stuff like alt, ctrl, lshift, capsl)
     */
    void (*_keyCallback)(void* , int, int,int) = nullptr;





public:
    Canvas(){_baseColor = glm::vec3(1,1,1);}
    ~Canvas(){}

    void VirtualResize() override {CanvasResize();}
    void VirtualRender() override {CanvasRender();}
    void VirtualUpdate() override {CanvasUpdate();}
    bool VirtualMouseMove() override {return CanvasMouseMove();}
    bool VirtualClick(MouseButtonType button, MouseActionType action) override {return CanvasMouseClick(button,action);}
    bool VirtualMouseWheel(float amount, MouseWheelDirection direction) override {return CanvasMouseWheel(amount,direction);}
    void VirtualKeyInput(int key, int action, int mods) override {CanvasKeyInput(key,action,mods);}

    void SetCallbackContext(void* context) {_callbackContext = context;}
    void SetResizeCallback(void(*resizeCallback)(void*,int,int)) {_resizeCallback = resizeCallback;}
    void SetRenderCallback(void(*renderCallback)(void*)) {_renderCallback = renderCallback;}
    void SetKeyInputCallback(void(*keyCallback)(void*,int,int,int)){_keyCallback = keyCallback;}
    void SetUpdateCallback(void(*updateCallback)(void*,float)){_updateCallback = updateCallback;}
    void SetMouseMoveCallback(void(*mouseMoveCallback)(void*,float,float)){_mouseMoveCallback = mouseMoveCallback;}
    void SetMouseClickCallback(void(*mouseClickCallback)(void*,MouseButtonType,MouseActionType)){_mouseClickCallback = mouseClickCallback;}
    void SetMouseWheelCallback(void(*mouseWheelCallback)(void*,float,MouseWheelDirection)){_mouseWheelCallback = mouseWheelCallback;}

protected:
    void CanvasResize();
    void CanvasRender();
    void CanvasUpdate();
    bool CanvasMouseMove();
    bool CanvasMouseClick(MouseButtonType button, MouseActionType action);
    bool CanvasMouseWheel(float amount, MouseWheelDirection direction);
    void CanvasKeyInput(int key,int action, int mods); // Needs more agrs like is key pressed/released
};

#endif