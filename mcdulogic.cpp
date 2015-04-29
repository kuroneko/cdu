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

    if (!SDL_PollEvent(NULL) && long_press_threshold != 0 && keydowntimes.size() > 0) {
      // the oldest key pressed will ALWAYS be at the back of the list.
      Uint32  firstKey = keydowntimes.back().downTime;
      Uint32  curTime = SDL_GetTicks();

      if (curTime-firstKey >= long_press_threshold) {
        // if the longpress key pressed has exceeded its timeout, invoke the longpress and
        // remove it from the pending keys
        this->long_press(keydowntimes.back().point);
        keydowntimes.pop_back();
        continue;
      } else {
        SDL_ClearError();
        if (!SDL_WaitEventTimeout(&eventInfo, long_press_threshold-(curTime-firstKey))) {
          // timeout.  Re-enter so the early attempt to catch timeouts can fire.
          continue;
        }
      }
    } else {
      if (!SDL_WaitEvent(&eventInfo)) {
        cout << SDL_GetError() << endl;
        return;
      }
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
  Codepoint    keycode = Codepoint::NONE;

  keycode = this->keysymToPoint(eventInfo.key.keysym);
  if (keycode == Codepoint::NONE) {
    return;
  }
  Uint32    timestamp = SDL_GetTicks();

  if (eventInfo.key.type == SDL_KEYDOWN) {
    if (this->can_long_press(keycode)) {
      keydowntimes.push_front(CDUKeypress{downTime: timestamp, point: keycode});
    } else {
      this->short_press(keycode);
    }
  } else if (eventInfo.key.type == SDL_KEYUP) {
    // we only care for longpress keys
    if (this->can_long_press(keycode)) {
      for (list<struct CDUKeypress>::iterator item = keydowntimes.begin(); item != keydowntimes.end(); item++) {
        if (item->point == keycode) {
          int duration = SDL_TICKS_PASSED(timestamp, item->downTime);
          keydowntimes.erase(item);
          if (SDL_TICKS_PASSED(timestamp, item->downTime + long_press_threshold)) {
            this->long_press(keycode);
          } else {
            this->short_press(keycode);
          }
          return;
        }
      }
      // couldn't find the keydown for a long-press key - send it as a short-press anyway
      this->short_press(keycode);
    }
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
    case SDLK_SPACE:
      return Codepoint::SPACE;
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

extern "C" {
  static int
  bootstrap_listener(void *objptr)
  {
    MCDULogic *obj = static_cast<MCDULogic *>(objptr);
    return obj->listener();
  }
}

void
MCDULogic::start_listener() {
  running = true;
  SDL_CreateThread(&bootstrap_listener, "MCDURecv", this);
}

int
MCDULogic::listener() {
  while (running) {
    
  }
}


