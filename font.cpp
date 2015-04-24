/* font.cpp
 *
 * Font and Glyph Processing for MCDU
*/

#include "mcdu.h"

using namespace std;
using namespace mcdu;

MCDUFont::MCDUFont(SDL_Renderer *newRenderer)
{
  renderer = newRenderer;
  font = NULL;
  max_width = 0;
  max_height = 0;
  // empty the glyph table
  for (int i = 0; i < 256; i++) {
    glyphs[i] = NULL;
  }
}

MCDUFont::MCDUFont(SDL_Renderer *newRenderer, const std::string &fontname, int basesize) : MCDUFont(newRenderer)
{
  loadAerowinxTTF(fontname, basesize);
}

MCDUFont::~MCDUFont()
{
  // release any loaded glyphs
  for (int i = 0; i < 256; i++) {
    if (glyphs[i] != NULL) {
      SDL_DestroyTexture(glyphs[i]);
      glyphs[i] = NULL;
    }
  }
  close_font();
}

bool
MCDUFont::loadAerowinxTTF(const std::string &fontname, int size)
{
  if (!open_font(fontname, size)) {
    return false;
  }
  // now, build the glyph table
  for (int i = 0; i < 256; i++) {
    glyphs[i] = NULL;
  }

#define PREP_GLYPH(x) prerender_glyph(x, x)
  for (int i = 'A'; i <= 'Z'; i++) {
    PREP_GLYPH(i);
  }
  for (int i = '0'; i <= '9'; i++) {
    PREP_GLYPH(i);
  }
  PREP_GLYPH('.');
  PREP_GLYPH('(');
  PREP_GLYPH(')');
  PREP_GLYPH(G_DEGREE);
  PREP_GLYPH(G_BOX);
  PREP_GLYPH(G_ARROW_UP);
  PREP_GLYPH(G_ARROW_DOWN);
  PREP_GLYPH(G_ARROW_LEFT);
  PREP_GLYPH(G_ARROW_RIGHT);
  PREP_GLYPH(G_TRIANGLE);

  return true;
}

bool 
MCDUFont::open_font(const std::string &fontname, int size)
{
  close_font();
  font = TTF_OpenFont(fontname.c_str(), size);
  if (NULL == font) {
    return false;
  }
  return true; 
}

void
MCDUFont::close_font()
{
  if (NULL != font) {
    TTF_CloseFont(font);
    font = NULL;
  }
}

void
MCDUFont::prerender_glyph(char point, char glyph)
{
  SDL_Texture *rv = NULL;
  char    sbuf[2] = {point, '\0'};

  SDL_Surface *cleanGlyph = TTF_RenderText_Blended(font, sbuf, SDL_Color{255,255,255,255});
  if (cleanGlyph->w > max_width) {
    max_width = cleanGlyph->w;
  }
  if (cleanGlyph->h > max_height) {
    max_height = cleanGlyph->h;
  }
  glyphs[glyph] = SDL_CreateTextureFromSurface(renderer, cleanGlyph);
  SDL_FreeSurface(cleanGlyph);
}

SDL_Texture *
MCDUFont::glyphFor(int point)
{
  return glyphs[point];
}