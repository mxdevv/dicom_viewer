#ifndef __SHADER_H__
#define __SHADER_H__

class Shader
{
  GLuint shader = -1;
  GLuint shader_type = -1;
  const GLchar* shader_source = nullptr;

  void compile();

public:
  Shader();
  Shader(const GLchar* shader_source, GLuint shader_type);

  void create(const GLchar* shader_source, GLuint shader_type);

  const GLuint get();
  const GLuint get_type();

  class no_shader_source_exception { };
  class recreation_exception { };
};

#endif
