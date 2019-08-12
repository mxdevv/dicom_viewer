#ifndef __SHADER_H__
#define __SHADER_H__

class Shader
{
public:
  enum class e_type {
    vertex = GL_VERTEX_SHADER, geometry = GL_GEOMETRY_SHADER,
    fragment = GL_FRAGMENT_SHADER
  };

private:
  GLuint shader = -1;
  e_type shader_type;
  const GLchar* shader_source = nullptr;

  void compile();

public:
  Shader();
  Shader(const GLchar* shader_source, e_type shader_type);

  void create(const GLchar* shader_source, e_type shader_type);

  const GLuint get();
  const e_type get_type();

  class no_shader_source_exception { };
  class recreation_exception { };
};

#endif
