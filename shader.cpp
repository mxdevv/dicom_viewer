#ifndef __SHADER_CPP__
#define __SHADER_CPP__

#include "shader.h"

void Shader::compile()
{
  shader = glCreateShader(static_cast<GLuint>(shader_type));
  glShaderSource(shader, 1, &shader_source, NULL);
  glCompileShader(shader);

  GLint success;
  GLchar log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, log);
    std::cerr << "Shader compilation error!\n" << log << std::endl;
  }
}

Shader::Shader()
  { }

Shader::Shader(const GLchar* shader_source, e_type shader_type)
  : shader_source(shader_source), shader_type(shader_type)
{
  compile();
}

void Shader::create(const GLchar* shader_source, e_type shader_type)
{
  if (shader != -1) {
    throw recreation_exception();
  }

  this->shader_source = shader_source;
  this->shader_type = shader_type;

  compile();
}

const GLuint Shader::get()
{
  if (shader_source == nullptr) {
    throw no_shader_source_exception();
  }
  return shader;
}

const Shader::e_type Shader::get_type()
{
  if (shader_source == nullptr) {
    throw no_shader_source_exception();
  }
  return shader_type;
}

#endif
