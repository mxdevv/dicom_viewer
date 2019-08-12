#ifndef __PROGRAM_CPP__
#define __PROGRAM_CPP__

#include "program.h"

void Program::compile()
{
  program = glCreateProgram();
  if (vertex) glAttachShader(program, vertex->get());
  if (geometry) glAttachShader(program, geometry->get());
  if (fragment) glAttachShader(program, fragment->get());
  glLinkProgram(program);

  GLint success;
  GLchar log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, log);
    std::cout << "Linking program fail!\n" << log << std::endl;
  }
}

Program::Program()
  { }

Program::Program(Shader* vertex, Shader* geometry, Shader* fragment)
  : vertex(vertex), geometry(geometry), fragment(fragment)
{
  compile();
}

void Program::create(Shader* vertex, Shader* geometry, Shader* fragment)
{
  this->vertex = vertex;
  this->geometry = geometry;
  this->fragment = fragment;

  compile();
}

const GLuint Program::get()
{
  return program;
}

void Program::run()
{
  glUseProgram(program);
}

#endif
