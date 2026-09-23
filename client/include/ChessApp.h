#pragma once
#ifndef CHESSCLIENTAPP_H
#define CHESSCLIENTAPP_H

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MyOpenGL.h"

#include "Gui.h"
#include "Container.h"
#include "Button.h"
#include "Canvas.h"

#include "OrbitCamera.h"

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
    Button _buttonStartSameComputerChessGame;

    // Finally, the Game menu, where the magic happens
    Container _containerGameMenu;
    Button _buttonGameMenuToPreviousMenu; //just simply "previous", because we dont actually know if the game started from singleplayer,
                                          // MUltiplayer online, or same computer.
    Container _container3DViewPort;
    Canvas _canvasChessViewport;


    //Variables related to the menu state of the chess application
    ChessAppMenuType _previousMenu = ChessAppMenuType::MENU_NONE;
    ChessAppMenuType _currentMenu = ChessAppMenuType::MENU_START;

    // Variables related to the chess viewport
    glm::vec2 _chessViewportSize = glm::vec2(100,100);

    Texture _chessViewPortTexture;
    RenderBuffer _chessViewPortRenderBuffer;
    FrameBuffer _chessViewPortFrameBuffer;

    OrbitCamera _orbitCamera;
    bool _isDraggingChessViewport = false;
    glm::vec2 _chessViewportPreviousMousePos = glm::vec2(0,0);
    glm::vec2 _chessViewportCurrentMousePos = glm::vec2(0,0);

    Shader _boardShader;
    Mesh<VertexP3N3> _boardMesh;
    const float _boardWidth = 8.0f;
    const float _boardHeight = 1.0f;
    glm::vec3 _boardLightColor = glm::vec3(0.8,0.8,0.8);
    glm::vec3 _boardDarkColor = glm::vec3(0.2,0.2,0.2);
    glm::mat4 _boardTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0,-_boardHeight/2,0)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(_boardWidth,_boardHeight,_boardWidth));

    Shader _pieceShader;
    std::vector<Mesh<VertexP3N3>> _pieceMeshes;
    
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

    // the viewport canvas callbacks
    static void ViewPortCanvasResizeCallback(void* context, int newWidth, int newHeight);
    static void ViewPortCanvasUpdateCallback(void* context, float deltaTime);
    static void ViewPortCanvasRenderCallback(void* context);
    static void ViewPortCanvasMouseMoveCallback(void* context, float newX, float newY);
    static void ViewPortCanvasMouseClickCallback(void* context, MouseButtonType button, MouseActionType action);
    static void ViewPortCanvasMouseWheelCallback(void* context, float amount, MouseWheelDirection direction);

private:

    bool InitGlfw3();
    bool InitGlad();
    bool InitMyOpenGLResources();
    bool InitMyGui();

    void Update();
    void Render();

    void SwapToMenu(ChessAppMenuType menuType);

    void ChessViewPortResize(int newWidth, int newHeight);
    void ChessViewPortRender();
    void ChessViewPortUpdate(float deltaTime);
    void ChessViewPortMouseMove(float newX, float newY);
    void ChessViewPortMouseClick(MouseButtonType button, MouseActionType action);
    void ChessViewPortMouseWheel(float amount, MouseWheelDirection direction);


};

#endif