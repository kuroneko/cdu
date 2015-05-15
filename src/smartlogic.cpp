#include "mcdu.h"
#include <iostream>

using namespace std;
using namespace mcdu;

SmartLogic::SmartLogic(SDL_Renderer *rend)
  : MCDULogic(rend)
{
}

void
SmartLogic::loop()
{
  running = true;
  while(running) {
    render();

    do {
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
        running = false;
        return;
      case SDL_WINDOWEVENT:
        if (eventInfo.window.event == SDL_WINDOWEVENT_RESIZED) {
          output_w = eventInfo.window.data1;
          output_h = eventInfo.window.data2;
          autoscale();
        }
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        handle_keypress(eventInfo);
        break;
      case SDL_MOUSEBUTTONDOWN:
        handle_mousedown(eventInfo);
        break;
      case SDL_MOUSEBUTTONUP:
        handle_mouseup(eventInfo);
        break;        
      }
    } while (SDL_PollEvent(NULL));
  }
}


void
SmartLogic::key_down(Codepoint keycode)
{
  Uint32    timestamp = SDL_GetTicks();

  if (this->can_long_press(keycode)) {
    keydowntimes.push_front(CDUKeypress{timestamp, keycode});
  } else {
    this->short_press(keycode);
  }
}

void
SmartLogic::key_up(Codepoint keycode)
{
  Uint32    timestamp = SDL_GetTicks();

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