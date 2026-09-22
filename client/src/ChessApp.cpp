#include "ChessApp.h"

#include <iostream>

ChessApp::ChessApp()
{
    bool glfw3InitResult = InitGlfw3();
    if(glfw3InitResult == false)
    {
        std::cout<<"[Error]: Something went wrong while initializing GLFW3. Shutting down..\n";
        _isRunning = false;
        return;
    }

    bool gladInitResult = InitGlad();
    if(gladInitResult == false)
    {
        std::cout<<"[Error]: Something went wrong while initializing GLAD. Shutting down..\n";
        _isRunning = false;
        return;
    }

    bool myGuiInitResult = InitMyGui();
    if(myGuiInitResult == false)
    {
        std::cout<<"[Error]: Something went wrong while initializing GUI. Shutting down..\n";
        _isRunning = false;
        return;
    }
}

ChessApp::~ChessApp()
{
}

void ChessApp::Run()
{
    while(_isRunning)
    {
        Update();
        Render();
    }
}

bool ChessApp::InitGlfw3()
{
    if (!glfwInit())
	{
		std::cout << "[GLFW Error]: Initialization of GLFW failed!" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_windowSize = this->_windowMinSize;
	_window = glfwCreateWindow(_windowSize.x, _windowSize.y, _appTitle.c_str(), NULL, NULL);
	if (!_window)
	{
		std::cout << "[GLFW Error]: Window creation failed!" << std::endl;
		glfwTerminate();
		return false;
	}
	

	glfwSetWindowSizeLimits(_window,_windowMinSize.x, _windowMinSize.y, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);

	//Sets the current App context as the window user pointer
	glfwSetWindowUserPointer(_window, this);

	glfwSetInputMode(_window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Callbacks
    
	glfwSetWindowSizeCallback(_window, ChessApp::WindowSizeCallback);
	glfwSetCursorPosCallback(_window, ChessApp::CursorPosCallback);
	glfwSetKeyCallback(_window, ChessApp::KeyCallback);
	glfwSetMouseButtonCallback(_window, ChessApp::MouseButtonCallback);
	glfwSetScrollCallback(_window, ChessApp::ScrollCallback);
	glfwSetWindowMaximizeCallback(_window,ChessApp::WindowMaximizationCallback);
	/*
	
	glfwSetWindowIconifyCallback(m_window, App::WindowIconifiedCallback);
	*/


    return true;
}

bool ChessApp::InitGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout<<"[GLAD ERROR]: Failed to load GLAD libraries!\n";
        return false;
    }

    glViewport(0, 0, _windowSize.x, _windowSize.y);
	glEnable(GL_CULL_FACE);

    return true;
}

bool ChessApp::InitMyGui()
{
    _gui.Init(this->_windowSize.x,this->_windowSize.y);

	/**
	 * The Start Menu's GUI
	 */
    _buttonStartSinglePlayer.SetMargin(MarginType::MARGIN_LEFT,0);
    _buttonStartSinglePlayer.SetMargin(MarginType::MARGIN_RIGHT,0);
    _buttonStartSinglePlayer.SetMargin(MarginType::MARGIN_BOTTOM,150);
    _buttonStartSinglePlayer.SetSize(SizeType::SIZE_HEIGHT,100.0f);
    _buttonStartSinglePlayer.SetBaseColor(0.4,0.4,0.4);
    _buttonStartSinglePlayer.SetHoverColor(0.5,0.5,0.5);
    _buttonStartSinglePlayer.SetText("Singleplayer");
    _buttonStartSinglePlayer.SetTextColor(1,1,1);
	_buttonStartSinglePlayer.SetCallBackContext(this);
	_buttonStartSinglePlayer.SetCallback(SwapToSinglePlayerMenuCallback);

    _containerStartButtons.AddControl(&_buttonStartSinglePlayer);

    _buttonStartMultiPlayer.SetMargin(MarginType::MARGIN_LEFT,0);
    _buttonStartMultiPlayer.SetMargin(MarginType::MARGIN_RIGHT,0);
    _buttonStartMultiPlayer.SetMargin(MarginType::MARGIN_BOTTOM,0);
    _buttonStartMultiPlayer.SetSize(SizeType::SIZE_HEIGHT,100.0f);
    _buttonStartMultiPlayer.SetBaseColor(0.4,0.4,0.4);
    _buttonStartMultiPlayer.SetHoverColor(0.5,0.5,0.5);
    _buttonStartMultiPlayer.SetText("Multiplayer");
    _buttonStartMultiPlayer.SetTextColor(1,1,1);
	_buttonStartMultiPlayer.SetCallBackContext(this);
	_buttonStartMultiPlayer.SetCallback(SwapToMultiplayerMenuCallback);

    _containerStartButtons.AddControl(&_buttonStartMultiPlayer);

    _containerStartButtons.SetMargin(MarginType::MARGIN_BOTTOM,200.0f);
    _containerStartButtons.SetSize(SizeType::SIZE_WIDTH,600.0f);
    _containerStartButtons.SetSize(SizeType::SIZE_HEIGHT,400.0f);
    _containerStartButtons.SetIsVisible(false);

    _containerStartMenu.AddControl(&_containerStartButtons);

    _containerStartMenu.SetMargin(MarginType::MARGIN_LEFT,0.0f);
    _containerStartMenu.SetMargin(MarginType::MARGIN_BOTTOM,0.0f);
    _containerStartMenu.SetMargin(MarginType::MARGIN_RIGHT,0.0f);
    _containerStartMenu.SetMargin(MarginType::MARGIN_TOP,0.0f);
    _containerStartMenu.SetBaseColor(0.2,0.2,0.2);
    _containerStartMenu.SetHoverColor(0.3,0.3,0.3);

    _gui.AddControl(&_containerStartMenu);
	_containerStartMenu.SetIsActive(true);

	/**
	 * THe Singleplayer Menu's GUI
	 */
	_buttonSingleplayerMenuToStartMenu.SetMargin(MarginType::MARGIN_TOP,10.0f);
	_buttonSingleplayerMenuToStartMenu.SetMargin(MarginType::MARGIN_LEFT,10.0f);
	_buttonSingleplayerMenuToStartMenu.SetSize(SizeType::SIZE_WIDTH, 50.0f);
	_buttonSingleplayerMenuToStartMenu.SetSize(SizeType::SIZE_HEIGHT, 50.0f);
	_buttonSingleplayerMenuToStartMenu.SetBaseColor(0.4,0.4,0.4);
	_buttonSingleplayerMenuToStartMenu.SetHoverColor(0.5,0.5,0.5);
	_buttonSingleplayerMenuToStartMenu.SetCallBackContext(this);
	_buttonSingleplayerMenuToStartMenu.SetCallback(SwapToStartMenuCallback);

	_containerSinglePlayerMenu.AddControl(&_buttonSingleplayerMenuToStartMenu);

	_containerSinglePlayerMenu.SetMargin(MarginType::MARGIN_LEFT,0.0f);
    _containerSinglePlayerMenu.SetMargin(MarginType::MARGIN_BOTTOM,0.0f);
    _containerSinglePlayerMenu.SetMargin(MarginType::MARGIN_RIGHT,0.0f);
    _containerSinglePlayerMenu.SetMargin(MarginType::MARGIN_TOP,0.0f);
    _containerSinglePlayerMenu.SetBaseColor(0.2,0.2,0.2);
    _containerSinglePlayerMenu.SetHoverColor(0.3,0.3,0.3);

    _gui.AddControl(&_containerSinglePlayerMenu);
	//_containerSinglePlayerMenu.SetIsActive(false);


	/**
	 * The Multiplayer Menu's GUI
	 */
	_buttonMultiplayerMenuToStartMenu.SetMargin(MarginType::MARGIN_TOP,10.0f);
	_buttonMultiplayerMenuToStartMenu.SetMargin(MarginType::MARGIN_LEFT,10.0f);
	_buttonMultiplayerMenuToStartMenu.SetSize(SizeType::SIZE_WIDTH, 50.0f);
	_buttonMultiplayerMenuToStartMenu.SetSize(SizeType::SIZE_HEIGHT, 50.0f);
	_buttonMultiplayerMenuToStartMenu.SetBaseColor(0.4,0.4,0.4);
	_buttonMultiplayerMenuToStartMenu.SetHoverColor(0.5,0.5,0.5);
	_buttonMultiplayerMenuToStartMenu.SetCallBackContext(this);
	_buttonMultiplayerMenuToStartMenu.SetCallback(SwapToStartMenuCallback);

	_containerMultiplayerMenu.AddControl(&_buttonMultiplayerMenuToStartMenu);

	_buttonStartGameSameComputer.SetMargin(MarginType::MARGIN_BOTTOM,0.0f);
	_buttonStartGameSameComputer.SetMargin(MarginType::MARGIN_LEFT,0.0f);
	_buttonStartGameSameComputer.SetMargin(MarginType::MARGIN_RIGHT,0.0f);
	_buttonStartGameSameComputer.SetSize(SizeType::SIZE_HEIGHT, 100.0f);
	_buttonStartGameSameComputer.SetBaseColor(0.4,0.4,0.4);
	_buttonStartGameSameComputer.SetHoverColor(0.5,0.5,0.5);
	_buttonStartGameSameComputer.SetText("Same Computer");
	_buttonStartGameSameComputer.SetTextColor(1,1,1);
	_buttonStartGameSameComputer.SetCallBackContext(this);
	_buttonStartGameSameComputer.SetCallback(SwapToSameComputerMenuCallback);

	_containerMultiplayerGameOptions.AddControl(&_buttonStartGameSameComputer);

	_buttonEnterOnlineLobby.SetMargin(MarginType::MARGIN_BOTTOM,150.0f);
	_buttonEnterOnlineLobby.SetMargin(MarginType::MARGIN_LEFT,0.0f);
	_buttonEnterOnlineLobby.SetMargin(MarginType::MARGIN_RIGHT,0.0f);
	_buttonEnterOnlineLobby.SetSize(SizeType::SIZE_HEIGHT, 100.0f);
	_buttonEnterOnlineLobby.SetBaseColor(0.4,0.4,0.4);
	_buttonEnterOnlineLobby.SetHoverColor(0.5,0.5,0.5);
	_buttonEnterOnlineLobby.SetText("Online Lobby");
	_buttonEnterOnlineLobby.SetTextColor(1,1,1);
	_buttonEnterOnlineLobby.SetCallBackContext(this);
	_buttonEnterOnlineLobby.SetCallback(SwapToOnlineLobbyMenuCallback);

	_containerMultiplayerGameOptions.AddControl(&_buttonEnterOnlineLobby);
	

	_containerMultiplayerGameOptions.SetMargin(MarginType::MARGIN_BOTTOM,200.0f);
	_containerMultiplayerGameOptions.SetSize(SizeType::SIZE_WIDTH,600.0f);
	_containerMultiplayerGameOptions.SetSize(SizeType::SIZE_HEIGHT,400.0f);
	_containerMultiplayerGameOptions.SetIsVisible(false);
	
	_containerMultiplayerMenu.AddControl(&_containerMultiplayerGameOptions);

	_containerMultiplayerMenu.SetMargin(MarginType::MARGIN_LEFT,0.0f);
    _containerMultiplayerMenu.SetMargin(MarginType::MARGIN_BOTTOM,0.0f);
    _containerMultiplayerMenu.SetMargin(MarginType::MARGIN_RIGHT,0.0f);
    _containerMultiplayerMenu.SetMargin(MarginType::MARGIN_TOP,0.0f);
    _containerMultiplayerMenu.SetBaseColor(0.2,0.2,0.2);
    _containerMultiplayerMenu.SetHoverColor(0.3,0.3,0.3);


    _gui.AddControl(&_containerMultiplayerMenu);
	//_containerMultiplayerMenu.SetIsActive(false);

	/**
	 * The gui components for the online lobby menu
	 */
	_buttonOnlineLobbyMenuToMultiplayerMenu.SetMargin(MarginType::MARGIN_TOP,10.0f);
	_buttonOnlineLobbyMenuToMultiplayerMenu.SetMargin(MarginType::MARGIN_LEFT,10.0f);
	_buttonOnlineLobbyMenuToMultiplayerMenu.SetSize(SizeType::SIZE_WIDTH, 50.0f);
	_buttonOnlineLobbyMenuToMultiplayerMenu.SetSize(SizeType::SIZE_HEIGHT, 50.0f);
	_buttonOnlineLobbyMenuToMultiplayerMenu.SetBaseColor(0.4,0.4,0.4);
	_buttonOnlineLobbyMenuToMultiplayerMenu.SetHoverColor(0.5,0.5,0.5);
	_buttonOnlineLobbyMenuToMultiplayerMenu.SetCallBackContext(this);
	_buttonEnterOnlineLobby.SetCallback(SwapToMultiplayerMenuCallback);

	_containerOnlineLobbyMenu.AddControl(&_buttonOnlineLobbyMenuToMultiplayerMenu);

	_containerOnlineLobbyMenu.SetMargin(MarginType::MARGIN_BOTTOM,0);
	_containerOnlineLobbyMenu.SetMargin(MarginType::MARGIN_LEFT,0);
	_containerOnlineLobbyMenu.SetMargin(MarginType::MARGIN_RIGHT,0);
	_containerOnlineLobbyMenu.SetMargin(MarginType::MARGIN_TOP,0);
	_containerOnlineLobbyMenu.SetBaseColor(0.2,0.2,0.2);
	_containerOnlineLobbyMenu.SetHoverColor(0.3,0.3,0.3);

	_gui.AddControl(&_containerOnlineLobbyMenu);
	//_containerOnlineLobbyMenu.SetIsActive(false);

	/**
	 * The gui elements for the same computer multiplayermenu
	 */

	_buttonSameComputerMenuToMultiplayerMenu.SetMargin(MarginType::MARGIN_TOP,10.0f);
	_buttonSameComputerMenuToMultiplayerMenu.SetMargin(MarginType::MARGIN_LEFT,10.0f);
	_buttonSameComputerMenuToMultiplayerMenu.SetSize(SizeType::SIZE_WIDTH, 50.0f);
	_buttonSameComputerMenuToMultiplayerMenu.SetSize(SizeType::SIZE_HEIGHT, 50.0f);
	_buttonSameComputerMenuToMultiplayerMenu.SetBaseColor(0.4,0.4,0.4);
	_buttonSameComputerMenuToMultiplayerMenu.SetHoverColor(0.5,0.5,0.5);
	_buttonSameComputerMenuToMultiplayerMenu.SetCallBackContext(this);
	_buttonSameComputerMenuToMultiplayerMenu.SetCallback(SwapToMultiplayerMenuCallback);

	_containerSameComputerMenu.AddControl(&_buttonSameComputerMenuToMultiplayerMenu);

	_containerSameComputerMenu.SetMargin(MarginType::MARGIN_BOTTOM,0);
	_containerSameComputerMenu.SetMargin(MarginType::MARGIN_LEFT,0);
	_containerSameComputerMenu.SetMargin(MarginType::MARGIN_RIGHT,0);
	_containerSameComputerMenu.SetMargin(MarginType::MARGIN_TOP,0);
	_containerSameComputerMenu.SetBaseColor(0.2,0.2,0.2);
	_containerSameComputerMenu.SetHoverColor(0.3,0.3,0.3);

	_gui.AddControl(&_containerSameComputerMenu);
	//_containerSameComputerMenu.SetIsActive(false);

    return true;
}

void ChessApp::Update()
{
    float currentFrameTime = glfwGetTime();
	_deltaTime = currentFrameTime - _lastFrameTime;
	_lastFrameTime = currentFrameTime;

	_timeSinceStartup = currentFrameTime;

    _gui.Update(_deltaTime);

    glfwPollEvents();
}

void ChessApp::Render()
{
    glClearColor(0.0,1.0f,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _gui.Render();

    glfwSwapBuffers(_window);
}

void ChessApp::SwapToMenu(ChessAppMenuType menuType)
{
	if(this->_currentMenu == menuType) return;
	
	/*
	this->_containerStartMenu.SetIsActive(false);
	this->_containerSinglePlayerMenu.SetIsActive(false);
	this->_containerMultiplayerMenu.SetIsActive(false);
	this->_containerOnlineLobbyMenu.SetIsActive(false);
	this->_containerSameComputerMenu.SetIsActive(false);
	this->_containerGameMenu.SetIsActive(false);
	*/

	this->_previousMenu = this->_currentMenu;
	this->_currentMenu = menuType;

	switch(menuType)
	{
		case ChessAppMenuType::MENU_START:
			this->_containerStartMenu.SetIsActive(true);
		break;

		case ChessAppMenuType::MENU_SINGLEPLAYER:
			this->_containerSinglePlayerMenu.SetIsActive(true);
		break;

		case ChessAppMenuType::MENU_MULTIPLAYER:
			this->_containerMultiplayerMenu.SetIsActive(true);
		break;

		case ChessAppMenuType::MENU_ONLINE_LOBBY:
			this->_containerOnlineLobbyMenu.SetIsActive(true);
		break;

		case ChessAppMenuType::MENU_SAME_COMPUTER:
			this->_containerSameComputerMenu.SetIsActive(true);
		break;

		case ChessAppMenuType::MENU_GAME:
			this->_containerGameMenu.SetIsActive(true);
		break;
	}
}

void ChessApp::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	ChessApp* app = (ChessApp*)glfwGetWindowUserPointer(window);
	app->_windowSize.x = width;
	app->_windowSize.y = height;

	if (width != 0 && height != 0)
	{
		glViewport(0, 0, width, height);
		app->_gui.Resize(width,height);

		app->Render();
	}
}


void ChessApp::WindowMaximizationCallback(GLFWwindow *window, int maximized)
{
	ChessApp* app = (ChessApp*)glfwGetWindowUserPointer(window);

	int w,h;
	glfwGetWindowSize(window,&w,&h);
	

	app->_windowSize.x = w;
	app->_windowSize.y = h;

	
	if (maximized)
    {
        // The window was maximized

    }
    else
    {
        // The window was rest

    }

		glViewport(0, 0, w, h);
		app->_gui.Resize(w,h);

	//std::cout<<"Window Resized Callback!\n";
}


void ChessApp::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	ChessApp* app = (ChessApp*)glfwGetWindowUserPointer(window);

	app->_gui.MouseMove(xpos,ypos);

	/*
	if (app->is_free_cam)
	{
		if (app->is_mouse_first_pos)
		{
			app->lastMouseX = xpos;
			app->lastMouseY = ypos;
			app->is_mouse_first_pos = false;
			//std::cout << "First mouse pos: (x: " << xpos << " y: " << ypos << " )\n";
		}
		else
		{
			float dx = xpos - app->lastMouseX;
			float dy = app->lastMouseY - ypos;

			//std::cout << "Delta mouse pos: (x: " << dx << " y: " << dy << " )\n";

			app->lastMouseX = xpos;
			app->lastMouseY = ypos;

			//app->camera.Rotate(dx, dy);

		}
	}
	
	*/
	


}

void ChessApp::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ChessApp* app = (ChessApp*)glfwGetWindowUserPointer(window);

	/*
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		if (app->is_free_cam == false)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			app->is_free_cam = true;
			app->is_mouse_first_pos = true;

			double x, y;
			glfwGetCursorPos(window, &x, &y);

			app->lastMouseX = x;
			app->lastMouseY = y;

			
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			app->is_free_cam = false;
		}


	}
		*/

		/*
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		//app->virtualWorld.SwitchRenderMode();


	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		//app->virtualWorld.debugDepth += 1;
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		//app->virtualWorld.debugDepth -= 1;
	}

	if(action != GLFW_RELEASE)
	{
		//std::cout<<"Key is being pressed: " << key << "\n";
	
		
	}*/

	app->_gui.KeyInput(key,action,mods);

	
	
}

void ChessApp::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	ChessApp* app = (ChessApp*)glfwGetWindowUserPointer(window);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//std::cout<<"Left mouse button clicked\n";
		app->_gui.MouseClick(MouseButtonType::MOUSE_BUTTON_LEFT,MouseActionType::MOUSE_ACTION_PRESS);
		
		//std::cout << "Picking at: (x :"<< x<< " y: " << y<< ")\n";
		//app->virtualWorld.Pick((int)x, (int)y);
	}
	else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		app->_gui.MouseClick(MouseButtonType::MOUSE_BUTTON_LEFT,MouseActionType::MOUSE_ACTION_RELEASE);	
	}
	else if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//app->m_GUI.MouseClick(1,0);
	}
}

void ChessApp::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	ChessApp* app = (ChessApp*)glfwGetWindowUserPointer(window);

	app->_gui.MouseWheel(yoffset,yoffset>0 ? MouseWheelDirection::MOUSEWHEEL_FORWARD : MouseWheelDirection::MOUSEWHEEL_BACKWARD);
}

void ChessApp::SwapToStartMenuCallback(void *context)
{
	ChessApp* app = (ChessApp*)context;
	app->SwapToMenu(ChessAppMenuType::MENU_START);
}

void ChessApp::SwapToSinglePlayerMenuCallback(void *context)
{
	ChessApp* app = (ChessApp*)context;
	app->SwapToMenu(ChessAppMenuType::MENU_SINGLEPLAYER);
}

void ChessApp::SwapToMultiplayerMenuCallback(void *context)
{
	ChessApp* app = (ChessApp*)context;
	app->SwapToMenu(ChessAppMenuType::MENU_MULTIPLAYER);
}

void ChessApp::SwapToOnlineLobbyMenuCallback(void *context)
{
	ChessApp* app = (ChessApp*)context;
	app->SwapToMenu(ChessAppMenuType::MENU_ONLINE_LOBBY);
}

void ChessApp::SwapToSameComputerMenuCallback(void *context)
{
	ChessApp* app = (ChessApp*)context;
	app->SwapToMenu(ChessAppMenuType::MENU_SAME_COMPUTER);
}

void ChessApp::SwapToGameMenuCallback(void *context)
{
	ChessApp* app = (ChessApp*)context;
	
	app->SwapToMenu(ChessAppMenuType::MENU_GAME);
}

void ChessApp::SwapToPreviousMenuCallback(void *context)
{
	ChessApp* app = (ChessApp*)context;
	if(app->_previousMenu != ChessAppMenuType::MENU_NONE)
	{
		app->SwapToMenu(app->_previousMenu);
	}
}
