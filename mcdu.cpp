#include "mcdu.h"
#include <iostream>
#include <string>

using namespace std;
using namespace mcdu;

MCDU::MCDU(SDL_Window *window, SDL_Renderer *renderer) 
{
  cduWindow = window;
  cduRenderer = renderer;
  largeFontHeight = 24;

  largeFont = new MCDUFont(cduRenderer);
  largeFont->loadAerowinxTTF("resources/awnxfmcl_fans05.TTF", largeFontHeight);
  smallFont = new MCDUFont(cduRenderer);
  smallFont->loadUnicodeTTF("resources/awnxfmcs_fans05.TTF", largeFontHeight);  
}

MCDU::~MCDU() 
{
  if (NULL != largeFont) {
    delete largeFont;
    largeFont = NULL;
  }
  if (NULL != smallFont) {
    delete smallFont;
    smallFont = NULL;
  }
}

void
MCDU::render() 
{
  SDL_SetRenderDrawColor(cduRenderer, 0, 0, 0, 255);
  SDL_RenderClear(cduRenderer);

  render_line("MCDU TEST 0.1           ", 0, true);
  render_line("123456789012345678901234", 1, false);
  render_line("123456789012345678901234", 2, true);
  render_line("ABCDEFGHIJKLMNOPQRSTUVWX", 3, false);
  render_line("YZ1234567890-+/*%#^?:;!~", 4, true);

  SDL_RenderPresent(cduRenderer);
}

void
MCDU::render_line(const std::string &text, int row, bool large)
{
  SDL_Color   textColor = {0, 255, 0, 255};

  MCDUFont *font = NULL;
  if (large) {
    font = largeFont;
  } else {
    font = smallFont;
  }

  // do font offset math.
  int width = largeFont->max_width;
  int yoffset = (largeFont->max_height - font->max_height);

  // fixme; cols
  for (int i = 0; i < 24; i++) {
    if (i >= text.length()) {
      break;
    }
    SDL_Texture *glyph = font->glyphFor(text[i]);
    if (NULL != glyph) {      
      SDL_Rect  destRect = { 
        (i * width), 
        yoffset + (row*largeFontHeight),
      };
      SDL_QueryTexture(glyph, NULL, NULL, &destRect.w, &destRect.h);
      // tween the offset slightly.
      int xcenter = (width - destRect.w)/2;
      destRect.x += xcenter;
      SDL_RenderCopy(cduRenderer, glyph, NULL, &destRect);
    }
  }
}
