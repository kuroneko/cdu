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
  while(1) {
    render();

    SDL_Event eventInfo;

    if (!SDL_WaitEvent(&eventInfo)) {
      return;
    }
    switch (eventInfo.type) {
    case SDL_QUIT:
      return;
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
  CDUKey    keycode = CDUKey::NONE;

  keycode = this->keysymToKey(eventInfo.key.keysym);
  if (keycode == CDUKey::NONE) {
    return;
  }

  if (eventInfo.key.type == SDL_KEYDOWN) {
    keydowntimes.push_front(CDUKeypress{downTime: eventInfo.key.timestamp, key: keycode});
  } else if (eventInfo.key.type == SDL_KEYUP) {
    for (list<struct CDUKeypress>::iterator item = keydowntimes.begin(); item != keydowntimes.end(); item++) {
      if (item->key == keycode) {
        int duration = SDL_TICKS_PASSED(eventInfo.key.timestamp, item->downTime);
        keydowntimes.erase(item);
        if (duration >= long_press_threshold) {
          this->long_press(keycode);
        } else {
          this->short_press(keycode);
        }
        return;
      }
    }
    this->short_press(keycode);
  }
}

//FIXME: this translation needs to be dynamically loadable
CDUKey
MCDULogic::keysymToKey(const struct SDL_Keysym &sym) const
{
  // first, map the SDL keysym to a CDU keysym
  if (sym.sym >= 'A' && sym.sym <= 'Z') {
    return static_cast<CDUKey>(sym.sym);
  }
  if (sym.sym >= 'a' && sym.sym <= 'z') {
    return static_cast<CDUKey>(toupper(sym.sym));
  }
  if (sym.sym >= '0' && sym.sym <= '9') {
    return static_cast<CDUKey>(sym.sym);
  }
  switch (sym.sym) {
    case SDLK_PAGEUP:
      return CDUKey::PREVPAGE;
    case SDLK_PAGEDOWN:
      return CDUKey::NEXTPAGE;
    case SDLK_UP:
      return CDUKey::UP;
    case SDLK_DOWN:
      return CDUKey::DOWN;
    case SDLK_LEFT:
      return CDUKey::LEFT;
    case SDLK_RIGHT:
      return CDUKey::RIGHT;
    case SDLK_BACKSPACE:
      return CDUKey::CLEAR;
    //FIXME:  these belong in the Boeing logic only
    case SDLK_DELETE:
      return CDUKey::DELETE;
    case SDLK_KP_ENTER:
    case SDLK_RETURN:
    case SDLK_RETURN2:
      return CDUKey::EXEC;
    default:
      break;
  }
  return CDUKey::NONE;
}

void
MCDULogic::self_test() {
  string  testLine = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-+/%()!\'#*^?:;~ ";

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
