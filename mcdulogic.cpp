#include <string>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>
#include <cctype>

#include "mcdu.h"

using namespace std;
using namespace mcdu;

MCDULogic::MCDULogic(SDL_Renderer *rend, int fontsize)
  : display(rend, fontsize)
{
  cduRenderer = rend;
  SDL_GetRendererOutputSize(cduRenderer, &output_w, &output_h);
  // 2 seconds is the actual long-action threshold for Boeing.
  long_press_threshold = 2000;
  self_test();
}

void
MCDULogic::render()
{
  int rend_w, rend_h;

  if (background != NULL) {
    rend_w = bg_size_w;
    rend_h = bg_size_h;
  } else {
    rend_w = display.charcell_width * display.columns;
    rend_h = display.charcell_height * display.rows;
  }
  SDL_Texture *outTexture = SDL_CreateTexture(cduRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rend_w, rend_h);
  SDL_SetRenderTarget(cduRenderer, outTexture);
  SDL_SetRenderDrawColor(cduRenderer, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_NONE);
  SDL_RenderClear(cduRenderer);
  if (background != NULL) {
    SDL_Rect  dest = {
      0, 0,
      bg_size_w, bg_size_h
    };
    SDL_RenderCopy(cduRenderer, background, NULL, &dest);
  }
  display.render();

  // now, reset rendering output back to the window/surface and scale the render to fit
  // our actual screen
  SDL_SetRenderTarget(cduRenderer, NULL);
  SDL_SetRenderDrawColor(cduRenderer, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_NONE);
  SDL_RenderClear(cduRenderer);

  // vscale
  double vFitScale = (double)output_h / (double)rend_h;
  double hFitScale = (double)output_w / (double)rend_w;

  double realScale = vFitScale;

  if ((rend_w * vFitScale) > output_w) {
    realScale = hFitScale;
  }

  SDL_Rect screenRect = {
    x: bg_offset_x,
    y: bg_offset_y,
    w: (int) ((double)bg_size_w * realScale),
    h: (int) ((double)bg_size_h * realScale),
  };
  SDL_RenderCopy(cduRenderer, outTexture, NULL, &screenRect);
  SDL_DestroyTexture(outTexture);
}

void
MCDULogic::loop()
{
  running = true;
  while(running) {
    render();

    SDL_Event eventInfo;
    if (!SDL_WaitEvent(&eventInfo)) {
      cout << SDL_GetError() << endl;
      return;
    }
    switch (eventInfo.type) {
    case SDL_QUIT:
      running = false;
      continue;
    case SDL_WINDOWEVENT:
      if (eventInfo.window.event == SDL_WINDOWEVENT_RESIZED) {
        output_w = eventInfo.window.data1;
        output_h = eventInfo.window.data2;
      }
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      handle_keypress(eventInfo);
      break;
    }
  }
}

void
MCDULogic::handle_keypress(SDL_Event &eventInfo)
{
  // ignore keyrepeats completely
  if (eventInfo.key.repeat) {
    return;
  }
  Codepoint    keycode = Codepoint::NONE;

  keycode = this->keysymToPoint(eventInfo.key.keysym);
  if (keycode == Codepoint::NONE) {
    return;
  }
  if (eventInfo.key.type == SDL_KEYDOWN) {
    this->key_down(keycode);
  } else if (eventInfo.key.type == SDL_KEYUP) {
    this->key_up(keycode);
  }
}

//FIXME: this translation needs to be dynamically loadable
Codepoint
MCDULogic::keysymToPoint(const struct SDL_Keysym &sym) const
{
  // first, map the SDL keysym to a CDU keysym
  if (sym.sym >= 'A' && sym.sym <= 'Z') {
    return static_cast<Codepoint>(sym.sym);
  }
  if (sym.sym >= 'a' && sym.sym <= 'z') {
    return static_cast<Codepoint>(toupper(sym.sym));
  }
  if (sym.sym >= '0' && sym.sym <= '9') {
    return static_cast<Codepoint>(sym.sym);
  }
  switch (sym.sym) {
    case SDLK_PERIOD:
      return Codepoint::PERIOD;
    case SDLK_SPACE:
      return Codepoint::SPACE;
    case SDLK_MINUS:
      return Codepoint::MINUS;
    case SDLK_SLASH:
      return Codepoint::SLASH;
    case SDLK_PAGEUP:
      return Codepoint::PREVPAGE;
    case SDLK_PAGEDOWN:
      return Codepoint::NEXTPAGE;
    case SDLK_UP:
      return Codepoint::UP;
    case SDLK_DOWN:
      return Codepoint::DOWN;
    case SDLK_LEFT:
      return Codepoint::LEFT;
    case SDLK_RIGHT:
      return Codepoint::RIGHT;
    case SDLK_BACKSPACE:
      return Codepoint::CLEAR;
    //FIXME:  these belong in the Boeing logic only
    case SDLK_DELETE:
      return Codepoint::DELETE;
    default:
      break;
  }
  return Codepoint::NONE;
}

void
MCDULogic::self_test() {
  string  testLine = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-+/%()bo#*udlrt ";

  enum ARINC_Color color[] = {
    C_White, C_Red, C_Green, C_Cyan, C_Magenta, C_Amber, C_Yellow
  };

  int colOffs = 0;
  for (int i = 0; i < display.rows; i++) {
    for (int j = 0; j < display.columns; ) {
      int subLength = testLine.length() - colOffs;
      if (subLength > (display.columns-j)) {
        subLength = (display.columns-j);
      }
      display.visiblePage.write_at(i, j, testLine.substr(colOffs, subLength), color[i%7], (i%2)?Font_Small:Font_Large);
      j += subLength;
      colOffs = (colOffs + subLength) % testLine.length();
    }
  }
}
