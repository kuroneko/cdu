#include "mcdu.h"
#include <iostream>
#include <string>

using namespace std;
using namespace mcdu;

MCDU::MCDU(SDL_Window *window, SDL_Renderer *renderer, int newRows, int newCols) 
{
  cduWindow = window;
  cduRenderer = renderer;
  largeFontHeight = 24;
  rows = newRows;
  columns = newCols;

  largeFont = new MCDUFont(cduRenderer);
  largeFont->loadAerowinxTTF("resources/awnxfmcl_fans05.TTF", largeFontHeight);
  smallFont = new MCDUFont(cduRenderer);
  smallFont->loadUnicodeTTF("resources/awnxfmcs_fans05.TTF", largeFontHeight);  

  data = new struct CDU_Cell[rows*columns];
  clear();
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
  if (data != NULL) {
    delete data;
    data = NULL;
  }
}

void
MCDU::render(int xoffs, int yoffs) 
{
  SDL_SetRenderDrawColor(cduRenderer, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_NONE);

  SDL_Rect  bgRect = {
    xoffs, yoffs,
    largeFont->max_width * columns,
    largeFontHeight * rows
  };
  SDL_RenderFillRect(cduRenderer, &bgRect);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      render_cell(xoffs, yoffs, i, j);
    }
  }
  SDL_RenderPresent(cduRenderer);
}

SDL_Color
MCDU::color_for_ARINCColor(enum ARINC_Color color)
{
  switch (color) {
  case CDU_Black:
    return SDL_Color{0, 0, 0, 255};
  case CDU_Magenta:
    return SDL_Color{255, 0, 255, 255};
  case CDU_Cyan:
    return SDL_Color{0, 255, 255, 255};
  case CDU_Amber:
    return SDL_Color{255, 128, 0, 255};
  case CDU_Red:
    return SDL_Color{255, 0, 0, 255};
  case CDU_White:
    return SDL_Color{255, 255, 255, 255};
  case CDU_Yellow:
    return SDL_Color{255, 255, 255, 255};
  case CDU_Green:
    return SDL_Color{0, 255, 0, 255};
  }
}

void
MCDU::render_cell(int xoffs, int yoffs, int row, int column)
{
  int   dataOffs = (row*columns)+column;

  if (data[dataOffs].glyph == 0) {
    return;
  }
  MCDUFont *font;
  switch (data[dataOffs].font) {
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

  SDL_Texture *glyph = font->glyphFor(data[dataOffs].glyph);
  SDL_Color gcolor = color_for_ARINCColor(data[dataOffs].color);
  /* position the box for the glyph */
  SDL_Rect  destRect = { 
    xoffs + (column * largeFont->max_width), 
    yoffs + (row*largeFontHeight) + (largeFont->max_height - font->max_height),
  };
  SDL_QueryTexture(glyph, NULL, NULL, &destRect.w, &destRect.h);
  // tweak the offset slightly to compensate for under and oversized glyphs.
  destRect.x += (largeFont->max_width - destRect.w)/2;

  SDL_SetRenderDrawColor(cduRenderer, gcolor.r, gcolor.g, gcolor.b, gcolor.a);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_BLEND);
  SDL_RenderCopy(cduRenderer, glyph, NULL, &destRect);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_MOD);
  SDL_RenderFillRect(cduRenderer, &destRect);
}

CDU_Cell *  
MCDU::cell_for(int row, int column)
{
  return &(data[(row*columns) + column]);
}

void
MCDU::clear()
{
  for (int i; i < rows*columns; i++) {
    data[i].glyph = 0;
    data[i].color = CDU_Black;
  }
}

void
MCDU::write_at(int row, int col, enum CDU_Font font, enum ARINC_Color color, const std::string &message)
{
  if (row >= rows) {
    return;
  }
  CDU_Cell *cells = cell_for(row, col);
  for (int i = 0; i < (columns-col); i++) {
    if (i >= message.length()) {
      break;
    }
    cells[i].glyph = message[i];
    cells[i].color = color;
    cells[i].font = font;
  }
}
