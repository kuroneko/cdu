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
  background = NULL;
  under_test = 0;
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
  display.render(display_offset_x, display_offset_y);
}

void
MCDULogic::loop()
{
  while(1) {
    update_scratchpad();
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
  if ((eventInfo.key.keysym.sym >= 'A' && eventInfo.key.keysym.sym <= 'Z')
    || (eventInfo.key.keysym.sym >= 'a' && eventInfo.key.keysym.sym <= 'z')
    || (eventInfo.key.keysym.sym >= '0' && eventInfo.key.keysym.sym <= '9')
    || (eventInfo.key.keysym.sym >= SDLK_KP_1 && eventInfo.key.keysym.sym <= SDLK_KP_PERIOD)
    || eventInfo.key.keysym.sym == SDLK_SPACE || eventInfo.key.keysym.sym == SDLK_KP_SPACE) {
    do_keyboard(eventInfo);
  }
  switch (eventInfo.key.keysym.sym) {
    case SDLK_BACKSPACE:
      do_clear(eventInfo);
      break;
    case SDLK_DELETE:
      if (eventInfo.type == SDL_KEYDOWN) {
        do_delete();
      }
      break;
    case SDLK_KP_ENTER:
    case SDLK_RETURN:
    case SDLK_RETURN2:
      if (eventInfo.type == SDL_KEYDOWN) {
        do_exec();
      }
      break;
    default:
      break;
  }

}

void
MCDULogic::do_exec()
{

}

void
MCDULogic::do_delete()
{
  if (scratchpad.length() == 0) {
    scratchpad = "DELETE";
    delete_selected = true;
  }
}

void
MCDULogic::do_clear(const SDL_Event &eventInfo)
{
  //FIXME:  Handle held-CLR.
  // ignore keyups for now
  if (eventInfo.type == SDL_KEYUP) {
    return;
  }
  if (messages.empty()) {
    // no messages?  clear delete if set, otherwise start removing character by character from the scratchpad.
    if (delete_selected) {
      scratchpad = "";
      delete_selected = false;
    } else if (scratchpad.length() > 0) {
      scratchpad.resize(scratchpad.length()-1);
    }
  } else {
    // displaying a message?  show the next one.
    messages.pop_front();
  }
  update_scratchpad();
}

void
MCDULogic::update_scratchpad()
{
  int scratchpadRow = display.getRows()-1; 
  if(messages.empty()) {
    if (!under_test) {
      display.clear_line(scratchpadRow);
      display.write_at(scratchpadRow, 0, Font_Large, CDU_White, scratchpad);  
    }
    annun_msg = false;
  } else {
    display.clear_line(scratchpadRow);
    display.write_at(scratchpadRow, 0, Font_Small, CDU_White, messages.front());
    annun_msg = true;
  }
}

void
MCDULogic::msg_remove(const std::string &msg)
{
  under_test = false;
  messages.remove(msg);
  update_scratchpad();
}

void
MCDULogic::msg_show(const std::string &msg)
{
  under_test = false;
  messages.remove(msg);
  messages.emplace_front(msg);
  update_scratchpad();
}

void
MCDULogic::self_test() {
  const int rows = display.getRows();
  const int columns = display.getColumns();

  under_test = true;

  string  testLine = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-+/%()!\'#*^?:;~ ";

  enum ARINC_Color color[] = {
    CDU_White, CDU_Red, CDU_Green, CDU_Cyan, CDU_Magenta, CDU_Amber, CDU_Yellow
  };

  int colOffs = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; ) {
      int subLength = testLine.length() - colOffs;
      if (subLength > (columns-j)) {
        subLength = (columns-j);
      }
      display.write_at(i, j, (i%2)?Font_Small:Font_Large, color[i%7], testLine.substr(colOffs, subLength));
      j += subLength;
      colOffs = (colOffs + subLength) % testLine.length();
    }
  }
}

void
MCDULogic::do_keyboard(const SDL_Event &eventInfo)
{
  if (eventInfo.type == SDL_KEYUP) {
    return;
  }
  if (!messages.empty()) {
    return;
  }
  int glyph = eventInfo.key.keysym.sym;
  if (glyph >= 'a' && glyph <= 'z') {
    toupper(glyph);
    scratchpad += (char)toupper(glyph);
  } else if (glyph >= 'A' && glyph <= 'Z') {
    scratchpad += (char)glyph;
  } else if (glyph >= '0' && glyph <= '9') {
    scratchpad += (char)glyph;
  } else {
    switch(glyph) {
    case SDLK_KP_1:
      scratchpad += '1';
      break;
    case SDLK_KP_2:
      scratchpad += '2';
      break;
    case SDLK_KP_3:
      scratchpad += '3';
      break;
    case SDLK_KP_4:
      scratchpad += '4';
      break;
    case SDLK_KP_5:
      scratchpad += '5';
      break;
    case SDLK_KP_6:
      scratchpad += '6';
      break;
    case SDLK_KP_7:
      scratchpad += '7';
      break;
    case SDLK_KP_8:
      scratchpad += '8';
      break;
    case SDLK_KP_9:
      scratchpad += '9';
      break;
    case SDLK_KP_0:
      scratchpad += '0';
      break;
    case SDLK_KP_PERIOD:
      scratchpad += '.';
      break;
    case SDLK_SPACE:
    case SDLK_KP_SPACE:
      scratchpad += ' ';
      break;
    }
  }
  update_scratchpad();
}

