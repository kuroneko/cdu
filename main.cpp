#include <string>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>

#include "mcdu.h"

using namespace std;
using namespace mcdu;

#ifdef __cplusplus
extern "C"
#endif
int
main(int argc, char **argv)
{
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  IMG_Init(IMG_INIT_PNG);

  SDL_Window *win = NULL;
  SDL_Renderer *rend = NULL;

  if (SDL_CreateWindowAndRenderer(640, 800, SDL_WINDOW_ALLOW_HIGHDPI|
    SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL, &win, &rend)) {
    cout << "ERROR: " << SDL_GetError() << endl;
    exit(1);
  }

  do {
    MCDULogic mcdu(win, rend, 27);

    SDL_Surface *background = IMG_Load("resources/B744_CDU.png");
    SDL_Texture *bgtexture = SDL_CreateTextureFromSurface(rend, background);
    SDL_QueryTexture(bgtexture, NULL, NULL, &mcdu.bg_size_w, &mcdu.bg_size_h);
    mcdu.background = bgtexture;
    mcdu.display_offset_x = 89;
    mcdu.display_offset_y = 62;
    mcdu.display.charcell_height = 24;
    SDL_FreeSurface(background);
    
    mcdu.loop();
  } while (0);

  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  return 0;
}
  

