
#include <string>
//#include <GLFW/glfw3.h>
#include "graphics.h"

class RenderSystem
{

public:
  RenderSystem();
  ~RenderSystem();

  void setCommonUniforms(); //shader, view, modelToWorldTransform)

  void drawObjModel(); //model, modelToWorldTransform, view):

  void setupObjModelShader();

  void draw();
  void draw1Pass();
  void draw2Pass();

  void RenderQuad();


protected:

  static std::string s_commonFragmentShaderCode;
  //objModelShader;

  //GLhandleARB 		m_shader; // Deferred rendering shader
  long m_shader;
  GLuint programObject;

  long m_shader2;
  GLuint m_programObject2;


  // G-buffer stuff
  unsigned int m_gBuffer = 0; // FBO
  unsigned int m_depBuffer = 0;
  unsigned int m_norBuffer = 0;
  unsigned int m_colBuffer = 0;
  unsigned int m_posBuffer = 0;


};
