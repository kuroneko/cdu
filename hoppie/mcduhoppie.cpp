#include "mcduhoppie.h"
#include <iostream>

using namespace std;
using namespace mcdu;
using namespace mcduhoppie;

MCDUHoppie::MCDUHoppie(SDL_Renderer *rend)
  : SmartLogic(rend)
{
}

void
MCDUHoppie::short_press(Codepoint key)
{
  switch (key) {
    case Codepoint::A:
    case Codepoint::B:
    case Codepoint::C:
    case Codepoint::D:
    case Codepoint::E:
    case Codepoint::F:
    case Codepoint::G:
    case Codepoint::H:
    case Codepoint::I:
    case Codepoint::J:
    case Codepoint::K:
    case Codepoint::L:
    case Codepoint::M:
    case Codepoint::N:
    case Codepoint::O:
    case Codepoint::P:
    case Codepoint::Q:
    case Codepoint::R:
    case Codepoint::S:
    case Codepoint::T:
    case Codepoint::U:
    case Codepoint::V:
    case Codepoint::W:
    case Codepoint::X:
    case Codepoint::Y:
    case Codepoint::Z:
    case Codepoint::KP_0:
    case Codepoint::KP_1:
    case Codepoint::KP_2:
    case Codepoint::KP_3:
    case Codepoint::KP_4:
    case Codepoint::KP_5:
    case Codepoint::KP_6:
    case Codepoint::KP_7:
    case Codepoint::KP_8:
    case Codepoint::KP_9:
    case Codepoint::PERIOD:
    case Codepoint::SPACE:
    case Codepoint::SLASH:
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
    case Codepoint::MINUS:
      if (scratchpad[scratchpad.length()-1] == '-') {
        scratchpad[scratchpad.length()-1] = '+' ;
      } else if (scratchpad[scratchpad.length()-1] == '+' ) {
        scratchpad[scratchpad.length()-1] = '-' ;
      } else {
        if (scratchpad.length() < display.columns) {
          scratchpad += '-';
        }
      }
      update_scratchpad();
      break;
    case Codepoint::CLEAR:
      do_clear(false);
      break;
    case Codepoint::DELETE:
      do_delete();
      break;
    default:
      cout << "Short key press registered for: " << static_cast<int>(key) << endl;
      msg_show("NOT IMPLEMENTED");
      break;
  }
}

void
MCDUHoppie::long_press(Codepoint key)
{
  switch (key) {
    case Codepoint::CLEAR:
      do_clear(true);
      break;
    default:
      short_press(key);
      break;
  }
}
    

bool
MCDUHoppie::can_long_press(Codepoint key)
{
  switch(key) {
  case Codepoint::CLEAR:
  case MCDUHoppie::Key_ATC:
    return true;
  default:
    return false;
  }
}



void
MCDUHoppie::msg_remove(const std::string &msg)
{
  under_test = false;
  messages.remove(msg);
  update_scratchpad();
}

void
MCDUHoppie::msg_show(const std::string &msg)
{
  under_test = false;
  messages.remove(msg);
  messages.emplace_front(msg);
  update_scratchpad();
}

void
MCDUHoppie::update_scratchpad()
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
MCDUHoppie::do_delete()
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
MCDUHoppie::do_clear(bool longPress)
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

