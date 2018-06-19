#include <ShaderLoader.h>
#include <fstream>
#include <streambuf>



static ShaderLoader* s_instance = 0;


ShaderLoader& ShaderLoader::get()
{
  if (s_instance == 0) s_instance = new ShaderLoader;
  return *s_instance;
}


ShaderLoader::~ShaderLoader()
{

}


void ShaderLoader::load(std::string& id, int flags, std::string filename)
{
  // ShaderInstance& shader = ShaderLibrary::Collection[id];
  // if (shader.flags == -1)
  // {
  //   shader.flags = flags;
  //   shader.filename = filename;
  //
  //   std::ifstream t(filename);
  //   std::string str((std::istreambuf_iterator<char>(t)),
  //                std::istreambuf_iterator<char>());
  //
  //   shader.filetext = str;
  //
  //
  //   //
  //   // Compile it
  //   //
  //
  //   if (flags == 1)
  //     shader.shader	= glCreateShader(GL_VERTEX_SHADER_ARB);
  //   else
  //     shader.shader	= glCreateShader(GL_FRAGMENT_SHADER_ARB);
  //
  //   glShaderSource(shader.shader, 1, shader.filetext.c_str(), NULL);
  //   glCompileShader(shader.shader);
  //
  //   int			bCompiled = false;
  //   glGetObjectParameterivARB( shader.shader, GL_OBJECT_COMPILE_STATUS_ARB, &bCompiled );
    // if( bCompiled  == false )
    //   throw new std::exception("Shader compilation failed.");

    // // Once compiled we can bind everything together for OpenGL to use
    // m_programHandler = glCreateProgramObjectARB();
    // glAttachObjectARB(m_programHandler,m_vertexShader);
    // glAttachObjectARB(m_programHandler,m_fragmentShader);
    //
    // glLinkProgramARB(m_programHandler);
//  }
}


ShaderLoader::ShaderLoader()
{

}
