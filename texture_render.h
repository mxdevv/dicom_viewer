#ifndef __TEXTURE_RENDER_H__
#define __TEXTURE_RENDER_H__

class Texture_render
{
  GLuint VBO, VAO, EBO;

  GLfloat vertices[36] = {
      // Positions         // Colors          // Texture Coords
       1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, // Top Right
       1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Bottom Right
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, // Bottom Left
      -1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f  // Top Left
  };
  GLuint indices[6] = {
    0, 1, 3,
    1, 2, 3
  };
public:
  Texture_render() = default;

  void init(); // отложенная инициализация
  void draw(); // без проверки на инициализацию
};

#endif
