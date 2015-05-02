/* psxmain.cpp
 *
 * Standalone MCDU for Precision Simulator X
 *
 * Copyright (C) 2015, Christopher Collins
 */
#include <string>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>

#include "mcdu.h"
#include "mcdupsx.h"

using namespace std;
using namespace mcdupsx;

#ifdef __cplusplus
extern "C"
#endif
int
main(int argc, char **argv)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
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
    MCDUPSX mcdu(win, rend);
    
    mcdu.loop();
  } while (0);

  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  return 0;
}
  

