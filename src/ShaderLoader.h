#include <string>
#include <map>
//#include <GLES2/gl2.h>
//#include <GLFW/glfw3.h>
#include "graphics.h"


class ShaderLoader
{

public:
  static ShaderLoader& get();
  ~ShaderLoader();

  void load(std::string& id, int flags, std::string filename);

protected:

  ShaderLoader();


};


class ShaderInstance
{
public:
  int flags = -1;
  std::string filename;
  std::string filetext;
  GLuint shader;
  GLuint program;
};


class ShaderLibrary
{
public:
  static std::map<std::string, ShaderInstance> Collection;
};
