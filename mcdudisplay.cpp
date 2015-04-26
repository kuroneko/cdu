#include "mcdu.h"
#include <iostream>
#include <string>

using namespace std;
using namespace mcdu;

MCDUDisplay::MCDUDisplay(SDL_Window *window, SDL_Renderer *renderer, int fontSize, int newRows, int newCols) 
  : rows(newRows), columns(newCols),
    visiblePage(rows, columns)
{
  cduWindow = window;
  cduRenderer = renderer;  

  largeFont = new MCDUFont(cduRenderer);
  largeFont->loadAerowinxTTF("resources/awnxfmcl_fans05.TTF", fontSize);
  smallFont = new MCDUFont(cduRenderer);
  smallFont->loadAerowinxTTF("resources/awnxfmcs_fans05.TTF", fontSize);

  charcell_height = largeFont->max_height;
  charcell_width = largeFont->max_width;
}

MCDUDisplay::~MCDUDisplay() 
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
MCDUDisplay::render() 
{
  if (render_background) {
    SDL_SetRenderDrawColor(cduRenderer, 0, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_NONE);

    SDL_Rect  bgRect = {
      offset_x, offset_y,
      charcell_width * columns,
      charcell_height * rows
    };
    SDL_RenderFillRect(cduRenderer, &bgRect);
  }
  visiblePage.render(*this);
  //FIXME: not sure we should be responsible for getting SDL to flush.
  SDL_RenderPresent(cduRenderer);
}

SDL_Color
MCDUDisplay::color_for_ARINCColor(enum ARINC_Color color)
{
  switch (color) {
  case C_Black:
    return SDL_Color{0, 0, 0, 255};
  case C_Magenta:
    return SDL_Color{255, 0, 255, 255};
  case C_Cyan:
    return SDL_Color{0, 255, 255, 255};
  case C_Amber:
    return SDL_Color{255, 128, 0, 255};
  case C_Red:
    return SDL_Color{255, 0, 0, 255};
  case C_White:
    return SDL_Color{255, 255, 255, 255};
  case C_Yellow:
    return SDL_Color{255, 255, 0, 255};
  case C_Green:
    return SDL_Color{0, 255, 0, 255};
  }
}

void
MCDUDisplay::render_cell(int row, int column, struct CDU_Cell *data)
{
  if (data == NULL) {
    return;
  }
  // first of all, render the cell background.
  if (data->bgcolor != C_Default) {
    SDL_Rect  bgRect = { 
      offset_x + (column * charcell_width), 
      offset_y + (row*charcell_height),
      charcell_width,
      charcell_height
    };
    SDL_Color bgcolor = color_for_ARINCColor(data->bgcolor);

    SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(cduRenderer, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
    SDL_RenderFillRect(cduRenderer, &bgRect);
  }

  if (data->glyph == 0) {
    return;
  }
  MCDUFont *font;
  switch (data->font) {
    case Font_Large:
      font = largeFont;
      break;
    case Font_Small:
      font = smallFont;
      break;
  }
  if (font == NULL) {
    return;
  }

  SDL_Texture *glyph = font->glyphFor(data->glyph);
  enum ARINC_Color fgcolorNum = data->fgcolor;
  if (fgcolorNum == C_Default) {
    fgcolorNum = default_fg;
  }
  SDL_Color fgcolor = color_for_ARINCColor(fgcolorNum);
  /* position the box for the glyph */
  SDL_Rect  destRect = { 
    offset_x + (column * charcell_width), 
    offset_y + (row*charcell_height) + (charcell_height - font->max_height),
  };
  SDL_QueryTexture(glyph, NULL, NULL, &destRect.w, &destRect.h);
  // tweak the offset slightly to compensate for under and oversized glyphs.
  destRect.x += (charcell_width - destRect.w)/2;

  SDL_SetRenderDrawColor(cduRenderer, fgcolor.r, fgcolor.g, fgcolor.b, fgcolor.a);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_BLEND);
  SDL_RenderCopy(cduRenderer, glyph, NULL, &destRect);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_MOD);
  SDL_RenderFillRect(cduRenderer, &destRect);
}

