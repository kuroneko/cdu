#include <string>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>

#include "mcdu.h"

using namespace std;
using namespace mcdu;

MCDULogic::MCDULogic(SDL_Window *win, SDL_Renderer *rend, int fontsize)
  : display(win, rend, fontsize)
{
  display.self_test();
}

void
MCDULogic::loop()
{
  while(1) {
    display.render(display_offset_x, display_offset_y);

    SDL_Event eventInfo;

    if (!SDL_WaitEvent(&eventInfo)) {
      return;
    }
    switch (eventInfo.type) {
    case SDL_QUIT:
      return;
    case SDL_WINDOWEVENT:
      break;
    }
  }
}