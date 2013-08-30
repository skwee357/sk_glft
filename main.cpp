#include <SDL/SDL.h>
#include <GL/gl.h>

#include "skfont.hpp"

using namespace sk;

void Init()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_SetVideoMode(640, 480, 32, SDL_OPENGL);

  glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
  glEnable(GL_TEXTURE_2D);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  glViewport(0, 0, 640, 480);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void Exit()
{
  SDL_Quit();
}

int main(int argc, char* argv[])
{
  Init();
  SDL_Event event;
  bool running = true;
  SkFont* font = new SkFont("Vera.ttf", 12, 0);

  while(running)
  {
    while(SDL_PollEvent(&event))
      if(event.type == SDL_QUIT)
        running = false;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f (0.0f, 0.0f, 0.0f);
    font->outText(100, 400, "Render multiply lines\nok\nUnicode:");
    glColor3f (1.0f, 0.0f, 0.0f);
    font->outText("gmm");
    glColor3f (0.0f, 1.0f, 0.0f);
    font->outText("adsa");
    glColor3f (0.0f, 0.0f, 1.0f);
    font->outText("aadssda");
    glColor3f (1.0f, 1.0f, 1.0f);
    font->outText("aadssda");
    glFlush();
    SDL_GL_SwapBuffers();
    SDL_Delay(1);
  }

  delete font;

  Exit();
  return 0;
}
