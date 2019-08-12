#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "shader.h"

class Program
{
  GLuint program = -1;
  Shader* vertex = nullptr, * geometry = nullptr, * fragment = nullptr;

  void compile();

public:
  Program();
  Program(Shader* vertex, Shader* geometry = nullptr,
      Shader* fragment = nullptr);

  void create(Shader* vertex = nullptr, Shader* geometry = nullptr,
      Shader* fragment = nullptr);
  
  const GLuint get();
  void run();
};

#endif
