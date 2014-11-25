/* created by Arthur Sonzogni */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Application.hpp"
#include <iostream>
#include "utils/glError.hpp"

using namespace std;


Application* currentApplication;

Application& Application::getInstance()
{
    return *currentApplication;
}

Application::Application():
    state(stateReady),
    width(640),
    height(480),
    title("Application")
{
    currentApplication=this;

    cout<<"[Info] GLFW initialisation"<<endl;

    // initialize the GLFW library
    if (!glfwInit())
    {
        cout<<"[error] glfw init fail"<<endl;
        std::exit(EXIT_FAILURE);
    }

    // setting the opengl version
    int major = 3;
    int minor = 3;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create the window
    window = glfwCreateWindow(width,height,title.c_str(),NULL,NULL);
    if (!window) {
        cout<<"[error] window create fail"<<endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    cout<<"[Info] GLEW initialization"<<endl;
	glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    glCheckError("glewInit()",__LINE__);

    if ( err != GLEW_OK)
	{
		cout<<"[Error] Cannot initialize GLEW"<<endl;
        cout<<glewGetErrorString(err)<<endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
	}



    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION); 
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // opengl configuration
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

    // vsync
    //glfwSwapInterval(false);



}

GLFWwindow* Application::getWindow() const
{
    return window;
}

void Application::exit() {
    state = stateExit;
}


float Application::getFrameDeltaTime() const
{
    return deltaTime;
}

float Application::getTime() const
{
    return time;
}

void Application::run() {
    state = stateRun;


    //Make the window's context current
    glfwMakeContextCurrent(window);

    auto timePrevious = (float)glfwGetTime();

    while( state == stateRun ) {

        // compute new time and delta time
        deltaTime = (float)glfwGetTime() - timePrevious;
        time+=deltaTime;
        timePrevious = (float)glfwGetTime();
        
        // detech window related changes
        detectWindowDimensionChange();

        // execute the frame code
        loop();

        // Swap Front and Back buffers (double buffering)
        glfwSwapBuffers(window);

        // Pool and process events
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void Application::detectWindowDimensionChange()
{
    int w,h;
    glfwGetWindowSize(getWindow(), &w, &h);
    dimensionChange = ( w!= width or h != height) ;
    if (dimensionChange) 
    {
        width = w;
        height = h;
        glViewport(0, 0, width, height);
    }
}
void Application::loop()
{
    cout<<"[INFO] : loop"<<endl;
}

int Application::getWidth()
{
    return width;
}

int Application::getHeight()
{
    return height;
}

float Application::getWindowRatio()
{
    return float(width)/float(height);
}

bool Application::windowDimensionChange()
{
    return dimensionChange;
}
