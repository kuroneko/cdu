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
#include <libconfig.h++>

#include "mcdu.h"
#include "mcdupsx.h"

using namespace std;
using namespace mcdupsx;
using namespace libconfig;

#ifdef __cplusplus
extern "C"
#endif
int
main(int argc, char **argv)
{
  libconfig::Config cduConfiguration;
  cduConfiguration.setIncludeDir(".");

  try {
    cduConfiguration.readFile("mcdupsx.conf");
  } catch (ParseException e) {
    cerr << "Couldn't parse config: " << e.getFile() << ":" << e.getLine() << ": " << e.getError() << endl;
    exit(1);
  }

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
  TTF_Init();
  IMG_Init(IMG_INIT_PNG);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  // CDU logical configuration
  Position position;
  if (cduConfiguration.exists("mcdu.position")) {
    std::string positionName = cduConfiguration.lookup("mcdu.position");
    if (positionName == "left") {
      position = Position::Left;
    } else if (positionName == "centre" || positionName == "center") {
      position = Position::Centre;
    } else if (positionName == "right") {
      position = Position::Right;
    }
  }

  // network parameters
  string connHostname = "";
  int connPort = 10747;
  if (!cduConfiguration.lookupValue("psx.hostname", connHostname)) {
    cerr << "Configuration must specify the host to connect to!" << endl;
    exit(1);
  }
  cduConfiguration.lookupValue("psx.port", connPort);

  // screen configuration
  bool fullScreen = false;
  int xres = 640;
  int yres = 480;
  bool grabFocus = false;
  
  cduConfiguration.lookupValue("mcdu.window.fullscreen", fullScreen);
  cduConfiguration.lookupValue("mcdu.window.x", xres);
  cduConfiguration.lookupValue("mcdu.window.y", yres);
  cduConfiguration.lookupValue("mcdu.window.grabfocus", grabFocus);

  SDL_Window *win = NULL;
  SDL_Renderer *rend = NULL;
  Uint32  flags = SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL;
  if (fullScreen) {
    flags |= SDL_WINDOW_FULLSCREEN;
  }
  if (grabFocus) {
    flags |= SDL_WINDOW_INPUT_GRABBED;
  }
  if (SDL_CreateWindowAndRenderer(xres, yres, flags, &win, &rend)) {
    cout << "ERROR: " << SDL_GetError() << endl;
    exit(1);
  }

  SDL_RendererInfo ri;
  if (SDL_GetRendererInfo(rend, &ri)) {
    cerr << "Failed to get SDL Renderer Info: " << SDL_GetError() << endl;
    return 1;
  }
  cout << "Renderer: " << ri.name << endl;

  int fontSize = 24;
  cduConfiguration.lookupValue("mcdu.display.font.size", fontSize);
  cout << "Font Size: " << fontSize << endl;

  // not a loop - just an excuse to ensure mcdu gets destroyed before we shutdown SDL. (we get panics otherwise)
  do {
    MCDUPSX mcdu(rend, position, connHostname, connPort, fontSize);

    if (cduConfiguration.exists("mcdu.background.image")) {
      mcdu.load_background(cduConfiguration.lookup("mcdu.background.image"));
    }
    cduConfiguration.lookupValue("mcdu.display.offset.x", mcdu.display.offset_x);
    cduConfiguration.lookupValue("mcdu.display.offset.y", mcdu.display.offset_y);
    cduConfiguration.lookupValue("mcdu.display.charcell.width", mcdu.display.charcell_width);
    cduConfiguration.lookupValue("mcdu.display.charcell.height", mcdu.display.charcell_height);
    cduConfiguration.lookupValue("mcdu.display.fill_background", mcdu.display.render_background);
      
    mcdu.loop();
  } while (0);

  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  return 0;
}
  

