#pragma once
#ifndef CHESSCLIENTAPP_H
#define CHESSCLIENTAPP_H

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>


#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MyOpenGL.h"

#include "Gui.h"
#include "Container.h"
#include "Button.h"
#include "Canvas.h"
#include "Label.h"
#include "TextInput.h"

#include "OrbitCamera.h"

#include "ChessGame.h"

enum class MainStateType
{
    MAINSTATE_NONE = 0,
    MAINSTATE_START = 1,
    MAINSTATE_SINGLEPLAYER = 2,
    MAINSTATE_MULTIPLAYER = 3,
    MAINSTATE_ONLINE_OPPONENT_FINDING_LOBBY = 4,
    MAINSTATE_SAME_COMPUTER_GAME_CONFIGURATION = 5,
    MAINSTATE_GAME_PLAYING = 6
};

enum class OpponentType
{
    OPPONENT_NONE = 0,
    OPPONENT_SAME_COMPUTER_PLAYER = 1,
    OPPONENT_ONLINE_PLAYER = 2,
    OPPONENT_BOT = 3
};

enum class ChessGameLightPlayerType
{
    LIGHT_PLAYER_RANDOM = 0,
    LIGHT_PLAYER_PLAYER1 = 1,
    LIGHT_PLAYER_PLAYER2 = 2
};


enum class ChessPickingState
{
    PICKSTATE_NONE_PICKED = 0,
    PICKSTATE_FIRST_POSITION_PICKED = 1,
};

struct ViewportPickResult
{
    bool wasAnythingActuallyThere = false;
    glm::vec3 pickedVirtualCoordinates = glm::vec3(0,0,0);
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

    /**
     * The Application is basically a state machine(like most programs) and it has main states and substates
     * (For example: online lobby main state can have multiple substates)
     */
    MainStateType _previousMainState = MainStateType::MAINSTATE_NONE;
    MainStateType _currentMainState = MainStateType::MAINSTATE_NONE;

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
    Label _labelPlayer1Name; TextInput _textInputPlayer1Name; Label _labelPlayer2Name; TextInput _textInputPlayer2Name; 
    
    Button _buttonStartSameComputerChessGame;

    // Finally, the Game menu, where the magic happens
    Container _containerGameMenu;
    Button _buttonGameMenuToPreviousMenu; //just simply "previous", because we dont actually know if the game started from singleplayer,
                                          // MUltiplayer online, or same computer.
    Container _container3DViewPort;
    Canvas _canvasChessViewport;


    


    // Variables related to the chess viewport rendering
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
    const float _boardHeight = 0.5f;
    glm::vec3 _boardLightColor = glm::vec3(0.8,0.8,0.8);
    glm::vec3 _boardDarkColor = glm::vec3(0.2,0.2,0.2);
    glm::mat4 _boardTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0,-_boardHeight/2,0)) * glm::scale(glm::mat4(1.0f), glm::vec3(_boardWidth,_boardHeight,_boardWidth));

    glm::vec3 _pieceLigthColor = glm::vec3(0.85,0.85,0.85);
    glm::vec3 _pieceDarkColor = glm::vec3(0.25,0.25,0.25);
    float _pieceSize = 0.7f;
    glm::mat4 _lightBaseRotationTransform = glm::mat4(1.0f);
    glm::mat4 _darkBaseRotationTransform = glm::rotate(glm::mat4(1.0f),glm::radians(180.0f),glm::vec3(0,1,0));
    Shader _pieceShader;
    std::vector<Mesh<VertexP3N3>> _pieceMeshes;

    

    // Variables related to the chess model logic and interaction logic
    ChessGame _chessGame;

    //pregame configurations
    ChessGameLightPlayerType _whoShouldBeLightPlayerDuringGame = ChessGameLightPlayerType::LIGHT_PLAYER_RANDOM;
    ChessGameLightPlayerType _lightPlayerIdentifier = ChessGameLightPlayerType::LIGHT_PLAYER_RANDOM; // actually who is light during game
    OpponentType _gameOpponentIsFrom = OpponentType::OPPONENT_NONE; // where the opponent moves are awaited from

    //picking related variables
    ChessPickingState _currentPickingState = ChessPickingState::PICKSTATE_NONE_PICKED;
    unsigned int _firstPickedIndex = 64u; // 64 is impossible to reach
    uint64_t _cachedCurrentPickingState = 0u;

    //previous move is also stored for easier readability
    unsigned int _previousMoveFirstIndex = 64u;
    unsigned int _previousMoveSecondIndex = 64u;
    uint64_t _cachedPreviousMoveBitmask = 0u;

    uint64_t _cachedLegalMovesBitmask = 0ull;
    
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

    static glm::uvec2 CreateUvec2FromUint64t(const uint64_t& bitmask);

private:

    bool InitGlfw3();
    bool InitGlad();
    bool InitMyOpenGLResources();
    bool InitMyGui();

    void Update();
    void Render();

    //These functions are responsible for swapping between the main menus, they check conditions, and properly setup the valid state
    // we are entering into. (for example if we move from bot selection -> game, we have to configure the enemy bot)
    // (or other example: if we move from online lobby to )
    void SetMainStateAs(MainStateType newMainState); // <-- simply sets the current as new, and previous as current
    void TryEnterSinglePlayerMainState();
    void TryEnterMultiPlayerSelectionMainState();
    void TryEnterStartMainState();
    void TryEnterOnlineOpponentLobbyMainState();
    void TryEnterSameComputerOpponentMainState();
    void TryEnterGamePlayingMainState();

    /**
     * This function set the state of the chessgame "model" class, and also sets the camera in a start angle
     */
    void SetupSharedChessGameState();

    /**
     * Based on the state it was called from, it sets up a state where the client is only playing itself.
     * In this state, it sets the opponent to be from the same computer, and by using the variable of whostartfirst, it can make
     * a determined state.
     */
    void SetupSameComputerGame();
    ChessGameLightPlayerType GetRandomPlayer();

    void SetupOnlineGame();

    void SetupSingleplayerGame();
    

    void ChessViewPortResize(int newWidth, int newHeight);
    void ChessViewPortRender();
    glm::vec2 GetViewPositionFromChessGameIndex(int index);
    void ChessViewPortUpdate(float deltaTime);
    void ChessViewPortMouseMove(float newX, float newY);
    void ChessViewPortMouseClick(MouseButtonType button, MouseActionType action);
    void ChessViewPortMouseWheel(float amount, MouseWheelDirection direction);

    //Picking related functions
    ViewportPickResult PickChessViewPort(int x, int y);
    void ResetPickingState();
    void TryAdvancePickingState(unsigned int pickedIndex);
    void CalculateCachedPickingState();
    void CalculateCachedPreviousMoveBitmask();
    unsigned int GetChessGameIndexFromVirtualPosition(float x, float z); // we dont care about y coord

    //The main move functions inside view
    // returns whether or not move was succesful
    bool TryMakeMove(unsigned int firstIndex, unsigned int secondIndex);
    
};

#endif