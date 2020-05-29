#include "mcdu.h"

using namespace std;
using namespace mcdu;

MCDUPage::MCDUPage(int rows, int columns) : rows(rows), columns(columns)
{
  data = new struct CDU_Cell[rows*columns];
  clear();
}

MCDUPage::MCDUPage(const MCDUPage &original)
  : MCDUPage(original.rows, original.columns)
{
  clear();
  write_at(0, 0, original);
}

MCDUPage::~MCDUPage() 
{
  if (NULL != data) {
    free(data);
    data = NULL;
  }
}

void
MCDUPage::render(MCDUDisplay &display) const
{
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      display.render_cell(i, j, cell_for(i, j));
    }
  }
}

CDU_Cell * 
MCDUPage::cell_for(int row, int column) const
{
  return &(data[(row*columns) + column]);
}

void 
MCDUPage::clear_line(int row, int startCol, int endCol)
{
  if (endCol < 0) {
    endCol = columns + endCol;
  }
  for (int i = startCol; i <= endCol; i++) {
    CDU_Cell *thisCell = cell_for(row, i);
    thisCell->glyph = Codepoint::NONE;
    thisCell->fgcolor = ARINC_Color::C_Default;
    thisCell->bgcolor = ARINC_Color::C_Default;
  }
}

void
MCDUPage::clear()
{
  for (int i = 0; i < rows; i++) {
    clear_line(i);
  }
}

Codepoint 
MCDUPage::codepointForChar(char charIn)
{
  switch(charIn) {
  case '_':
    return Codepoint::SPACE;
  case 'o':
    return Codepoint::DEGREE;
  case 'b':
    return Codepoint::BOX;
  case 'u':
    return Codepoint::UP;
  case 'd':
    return Codepoint::DOWN;
  case 'l':
    return Codepoint::LEFT;
  case 'r':
    return Codepoint::RIGHT;
  case 't':
    return Codepoint::TRIANGLE;
  default:
    return static_cast<Codepoint>(charIn);
  }
}

void
MCDUPage::write_at(int row, int col, const std::string &message,
  enum ARINC_Color fgcolor, enum CDU_Font font, enum ARINC_Color bgcolor)
{
  if (row >= rows) {
    return;
  }
  CDU_Cell *cells = cell_for(row, col);
  for (int i = 0; i < (columns-col); i++) {
    if (i >= message.length()) {
      break;
    }
    cells[i].glyph = codepointForChar(message[i]);
    cells[i].fgcolor = fgcolor;
    cells[i].bgcolor = bgcolor;
    cells[i].font = font;
  }
}
    
void 
MCDUPage::write_at(int row, int col, const MCDUPage &src)
{
  for (int i = 0; i < src.rows; i++) {
    if (row+i >= rows) {
      break;
    }
    for (int j = 0; j < src.columns; j++) {
      if (col+j >= columns) {
        break;
      }
      CDU_Cell *dstcell = cell_for(i, j);
      CDU_Cell *srccell = src.cell_for(i, j);
      *dstcell = *srccell;
    }
  }
}
