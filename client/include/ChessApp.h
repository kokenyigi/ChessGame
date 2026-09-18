#pragma once
#ifndef CHESSCLIENTAPP_H
#define CHESSCLIENTAPP_H

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class ChessApp
{
private:
    bool _isRunning = true;

    const std::string _appTitle = "Chess App";
    const glm::uvec2 _windowMinSize = glm::uvec2(1000,800);

    GLFWwindow* _window;
    bool _isWindowFullScreen = false;

    float _lastFrameTime = 0.0f;
    float _deltaTime = 0.05f;
    float _timeSinceStartup = 0.0f;
    bool _isFirstMousePos = false;
    bool _isMouseBeingDragged = false;
    bool _isMouseDisabled = false;
    glm::uvec2 _windowSize = glm::uvec2(1000,800);
    glm::vec2 _previousMousePos = glm::vec2(0,0);
    glm::vec2 _currentMousePos = glm::vec2(0,0);
    
public:
    ChessApp();
    ~ChessApp();

    void Run();

private:

    bool InitGlfw3();
    bool InitGlad();
    bool InitMyGui();

    void Update();
    void Render();

};

#endif