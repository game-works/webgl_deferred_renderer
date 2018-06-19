#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "graphics.h"


#include <RenderSystem.h>


GLFWwindow* g_window;
RenderSystem* g_rs = 0;


void draw()
{
  //
  // Drawing
  //

  // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
  glClearColor(0, 0.1, 0, 1);
  glClear( GL_COLOR_BUFFER_BIT );

  // Draw
  g_rs->draw();

  // Swap buffers
  // glfwSwapBuffers(g_window);
  // glfwPollEvents();
}


void input()
{

  //
  // Input
  //

  // Check if the ESC key was pressed or the window was closed
  // while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
  //      glfwWindowShouldClose(window) == 0 );

}


void loop()
{
  input();
  draw();
}


int init()
{
  if( !glfwInit() )
  {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      return 1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  // Open a window and create its OpenGL context
  g_window = glfwCreateWindow( 1280, 720, "Mega Racer", NULL, NULL);
  if( g_window == NULL )
  {
      fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(g_window); // Initialize GLEW
  //glewExperimental=true; // Needed in core profile
  // if (glewInit() != GLEW_OK)
  // {
  //     fprintf(stderr, "Failed to initialize GLEW\n");
  //     return -1;
  // }

  RenderSystem* rs = new RenderSystem();
  g_rs = rs;

  return 0;
}


void quit()
{
  glfwTerminate();
}


extern "C" int main(int argc, char** argv)
{
  if (init() != 0) return 1;

  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
  #endif

  quit();

  return 0;
}
