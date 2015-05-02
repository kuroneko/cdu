#include <string>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>
#include <cctype>

#include "mcdu.h"

using namespace std;
using namespace mcdu;

MCDULogic::MCDULogic(SDL_Window *win, SDL_Renderer *rend, int fontsize)
  : display(win, rend, fontsize)
{
  cduWindow = win;
  cduRenderer = rend;
  // 2 seconds is the actual long-action threshold for Boeing.
  long_press_threshold = 2000;
  self_test();
}

void
MCDULogic::render()
{
  SDL_SetRenderDrawColor(cduRenderer, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_NONE);
  SDL_RenderClear(cduRenderer);
  if (background != NULL) {
    SDL_Rect  dest = {
      bg_offset_x, bg_offset_y,
      bg_size_w, bg_size_h
    };
    SDL_RenderCopy(cduRenderer, background, NULL, &dest);
  }
  display.render();
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
