#include "mcdu.h"
#include <iostream>
#include <string>

using namespace std;
using namespace mcdu;

MCDUDisplay::MCDUDisplay(SDL_Window *window, SDL_Renderer *renderer, int fontSize, int newRows, int newCols) 
{
  cduWindow = window;
  cduRenderer = renderer;  
  rows = newRows;
  columns = newCols;

  largeFont = new MCDUFont(cduRenderer);
  largeFont->loadAerowinxTTF("resources/awnxfmcl_fans05.TTF", fontSize);
  smallFont = new MCDUFont(cduRenderer);
  smallFont->loadAerowinxTTF("resources/awnxfmcs_fans05.TTF", fontSize);

  charcell_height = largeFont->max_height;
  charcell_width = largeFont->max_width;

  data = new struct CDU_Cell[rows*columns];
  clear();
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
  if (data != NULL) {
    delete data;
    data = NULL;
  }
}

void
MCDUDisplay::render(int xoffs, int yoffs) 
{
  if (render_background) {
    SDL_SetRenderDrawColor(cduRenderer, 0, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_NONE);

    SDL_Rect  bgRect = {
      xoffs, yoffs,
      charcell_width * columns,
      charcell_height * rows
    };
    SDL_RenderFillRect(cduRenderer, &bgRect);
  }

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      render_cell(xoffs, yoffs, i, j);
    }
  }
  SDL_RenderPresent(cduRenderer);
}

SDL_Color
MCDUDisplay::color_for_ARINCColor(enum ARINC_Color color)
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
    return SDL_Color{255, 255, 0, 255};
  case CDU_Green:
    return SDL_Color{0, 255, 0, 255};
  }
}

void
MCDUDisplay::render_cell(int xoffs, int yoffs, int row, int column)
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
    xoffs + (column * charcell_width), 
    yoffs + (row*charcell_height) + (charcell_height - font->max_height),
  };
  SDL_QueryTexture(glyph, NULL, NULL, &destRect.w, &destRect.h);
  // tweak the offset slightly to compensate for under and oversized glyphs.
  destRect.x += (charcell_width - destRect.w)/2;

  SDL_SetRenderDrawColor(cduRenderer, gcolor.r, gcolor.g, gcolor.b, gcolor.a);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_BLEND);
  SDL_RenderCopy(cduRenderer, glyph, NULL, &destRect);
  SDL_SetRenderDrawBlendMode(cduRenderer, SDL_BLENDMODE_MOD);
  SDL_RenderFillRect(cduRenderer, &destRect);
}

CDU_Cell *  
MCDUDisplay::cell_for(int row, int column)
{
  return &(data[(row*columns) + column]);
}

void
MCDUDisplay::clear()
{
  for (int i; i < rows*columns; i++) {
    data[i].glyph = 0;
    data[i].color = CDU_Black;
  }
}

void
MCDUDisplay::write_at(int row, int col, enum CDU_Font font, enum ARINC_Color color, const std::string &message)
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

int
MCDUDisplay::getRows()
{
  return rows;
}

int
MCDUDisplay::getColumns()
{
  return columns;
}

void 
MCDUDisplay::clear_line(int row, int startCol, int endCol)
{
  if (endCol < 0) {
    endCol = columns + endCol;
  }
  for (int i = startCol; i <= endCol; i++) {
    CDU_Cell *thisCell = cell_for(row, i);
    thisCell->glyph = 0;
    thisCell->color = CDU_Black;
  }
}