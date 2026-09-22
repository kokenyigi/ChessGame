#pragma once
#ifndef CHESSCLIENTAPP_H
#define CHESSCLIENTAPP_H

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Gui.h"
#include "Container.h"
#include "Button.h"

enum class ChessAppMenuType
{
    MENU_NONE = 0,
    MENU_START = 1,
    MENU_SINGLEPLAYER = 2,
    MENU_MULTIPLAYER = 3,
    MENU_ONLINE_LOBBY = 4,
    MENU_SAME_COMPUTER = 5,
    MENU_GAME = 6
};


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

    //Gui member variables
    GUI _gui;


    // Start menu's gui component
    Container _containerStartMenu;

    Container _containerStartButtons;
    Button _buttonStartSinglePlayer;
    Button _buttonStartMultiPlayer;

    // The singleplayer menu's gui component
    Container _containerSinglePlayerMenu;
    Button _buttonSingleplayerMenuToStartMenu;


    //The multiplayer menu's gui component
    Container _containerMultiplayerMenu;
    Button _buttonMultiplayerMenuToStartMenu;

    Container _containerMultiplayerGameOptions;
    Button _buttonStartGameSameComputer;
    Button _buttonEnterOnlineLobby;

    // The multiplayer menu and its associated gui component
    Container _containerOnlineLobbyMenu;
    Button _buttonOnlineLobbyMenuToMultiplayerMenu;

    //The same computer menu
    Container _containerSameComputerMenu;
    Button _buttonSameComputerMenuToMultiplayerMenu;

    // Finally, the Game menu, where the magic happens
    Container _containerGameMenu;
    Button _buttonGameMenuToPreviousMenu; //just simply "previous", because we dont actually know if the game started from singleplayer,
                                          // MUltiplayer online, or same computer.
    Container _container3DViewPort;
    //Canvas _canvasChessViewport;


    //Variables related to the menu state of the chess application
    ChessAppMenuType _previousMenu = ChessAppMenuType::MENU_NONE;
    ChessAppMenuType _currentMenu = ChessAppMenuType::MENU_START;
    
public:
    ChessApp();
    ~ChessApp();

    void Run();

    // GLFW3 window- callback function pointers
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);
	static void WindowMaximizationCallback(GLFWwindow* window, int maximized);
	static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	/*
	static void WindowIconifiedCallback(GLFWwindow* window, int isIconified);
    */

    //My own gui's callback functions
    static void SwapToStartMenuCallback(void* context);
    static void SwapToSinglePlayerMenuCallback(void* context);
    static void SwapToMultiplayerMenuCallback(void* context);
    static void SwapToOnlineLobbyMenuCallback(void* context);
    static void SwapToSameComputerMenuCallback(void* context);
    static void SwapToGameMenuCallback(void* context);
    static void SwapToPreviousMenuCallback(void* context);

private:

    bool InitGlfw3();
    bool InitGlad();
    bool InitMyGui();

    void Update();
    void Render();

    void SwapToMenu(ChessAppMenuType menuType);

};

#endif