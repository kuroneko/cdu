#include "mcduboeing.h"
#include <iostream>

using namespace std;
using namespace mcdu;
using namespace mcduboeing;

MCDUBoeing::MCDUBoeing(SDL_Window *win, SDL_Renderer *rend)
  : MCDULogic(win, rend, 27)
{
    SDL_Surface *bgSurf = IMG_Load("resources/B744_CDU.png");
    SDL_Texture *bgtexture = SDL_CreateTextureFromSurface(rend, bgSurf);
    SDL_QueryTexture(bgtexture, NULL, NULL, &bg_size_w, &bg_size_h);
    background = bgtexture;
    display.offset_x = 89;
    display.offset_y = 62;
    display.charcell_height = 24;
    SDL_FreeSurface(bgSurf);
}

void
MCDUBoeing::short_press(CDUKey key)
{
  switch (key) {
    case CDUKey::A:
    case CDUKey::B:
    case CDUKey::C:
    case CDUKey::D:
    case CDUKey::E:
    case CDUKey::F:
    case CDUKey::G:
    case CDUKey::H:
    case CDUKey::I:
    case CDUKey::J:
    case CDUKey::K:
    case CDUKey::L:
    case CDUKey::M:
    case CDUKey::N:
    case CDUKey::O:
    case CDUKey::P:
    case CDUKey::Q:
    case CDUKey::R:
    case CDUKey::S:
    case CDUKey::T:
    case CDUKey::U:
    case CDUKey::V:
    case CDUKey::W:
    case CDUKey::X:
    case CDUKey::Y:
    case CDUKey::Z:
    case CDUKey::KP_0:
    case CDUKey::KP_1:
    case CDUKey::KP_2:
    case CDUKey::KP_3:
    case CDUKey::KP_4:
    case CDUKey::KP_5:
    case CDUKey::KP_6:
    case CDUKey::KP_7:
    case CDUKey::KP_8:
    case CDUKey::KP_9:
    case CDUKey::PERIOD:
    case CDUKey::SPACE:
      if (delete_selected) {
        break;
      }
      if (!messages.empty()) {
        break;
      }
      if (scratchpad.length() >= display.columns) {
        return;
      }
      scratchpad += static_cast<char>(key);
      update_scratchpad();
      break;
    case CDUKey::PLUSMINUS:
      if (scratchpad[scratchpad.length()-1] == '-') {
        scratchpad[scratchpad.length()-1] == '+' ;
      } else if (scratchpad[scratchpad.length()-1] == '+' ) {
        scratchpad[scratchpad.length()-1] == '-' ;
      } else {
        if (scratchpad.length() < display.columns) {
          scratchpad += '-';
        }
      }
      update_scratchpad();
      break;
    case CDUKey::CLEAR:
      do_clear(false);
      break;
    case CDUKey::DELETE:
      do_delete();
      break;
    default:
      cout << "Short key press registered for: " << static_cast<int>(key) << endl;
      msg_show("NOT IMPLEMENTED");
      break;
  }
}

void
MCDUBoeing::long_press(CDUKey key)
{
  switch (key) {
    case CDUKey::CLEAR:
      do_clear(true);
      break;
    default:
      short_press(key);
      break;
  }
}

void
MCDUBoeing::msg_remove(const std::string &msg)
{
  under_test = false;
  messages.remove(msg);
  update_scratchpad();
}

void
MCDUBoeing::msg_show(const std::string &msg)
{
  under_test = false;
  messages.remove(msg);
  messages.emplace_front(msg);
  update_scratchpad();
}

void
MCDUBoeing::update_scratchpad()
{
  int scratchpadRow = display.rows - 1; 
  if(messages.empty()) {
    if (!under_test) {
      display.visiblePage.clear_line(scratchpadRow);
      display.visiblePage.write_at(scratchpadRow, 0, scratchpad);  
    }
    annun_msg = false;
  } else {
    display.visiblePage.clear_line(scratchpadRow);
    display.visiblePage.write_at(scratchpadRow, 0, messages.front());
    annun_msg = true;
  }
}

void
MCDUBoeing::do_delete()
{
  // do nothing is there's a pending message
  if (!messages.empty()) {
    return;
  }
  // do nothing unless the scratchpad is empty.
  if (scratchpad != "") {
    return;
  }
  delete_selected = true;
  scratchpad = "DELETE";
  update_scratchpad();
}

void
MCDUBoeing::do_clear(bool longPress)
{
  if (messages.empty()) {
    // no messages?  clear delete if set, otherwise start removing character by character from the scratchpad.
    if (delete_selected) {
      delete_selected = false;
      scratchpad = "";
    } else if (scratchpad.length() > 0) {
      if (longPress) {
        scratchpad = "";
      } else {
        scratchpad.resize(scratchpad.length()-1);
      }
    }
  } else {
    // displaying a message?  show the next one.
    messages.pop_front();
  }
  update_scratchpad();
}

