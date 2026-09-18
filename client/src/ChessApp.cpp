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
    /*
	glfwSetWindowSizeCallback(m_window, App::WindowSizeCallback);
	glfwSetCursorPosCallback(m_window, App::CursorPosCallback);
	glfwSetKeyCallback(m_window, App::KeyCallback);
	glfwSetMouseButtonCallback(m_window, App::MouseButtonCallback);
	glfwSetScrollCallback(m_window, App::ScrollCallback);
	glfwSetWindowMaximizeCallback(m_window,App::WindowMaximizationCallback);
    */
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
    //stuff
    return true;
}

void ChessApp::Update()
{
    float currentFrameTime = glfwGetTime();
	_deltaTime = currentFrameTime - _lastFrameTime;
	_lastFrameTime = currentFrameTime;

	_timeSinceStartup = currentFrameTime;

    glfwPollEvents();
}

void ChessApp::Render()
{
    glClearColor(0.0,1.0f,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(_window);
}
