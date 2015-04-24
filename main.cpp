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

  SDL_Window *win = NULL;
  SDL_Renderer *rend = NULL;

  if (SDL_CreateWindowAndRenderer(640, 800, SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_OPENGL, &win, &rend)) {
    cout << "ERROR: " << SDL_GetError() << endl;
    exit(1);
  }

  MCDUDisplay *cdu = new MCDUDisplay(win, rend);

  cdu->write_at(0,0, Font_Large, CDU_White, "SIMSTACK MCDU");
  cdu->write_at(13,0, Font_Large, CDU_Red, "PLEASE WAIT");

  cdu->render(0,0);
  SDL_Delay(5000);
  SDL_Quit();

  return 0;
}
  

