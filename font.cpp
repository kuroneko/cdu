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
  loadUnicodeTTF(fontname, basesize);
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
MCDUFont::loadUnicodeTTF(const std::string &fontname, int size)
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
  PREP_GLYPH('-');
  PREP_GLYPH('+');
  PREP_GLYPH('/');
  PREP_GLYPH('%');
  PREP_GLYPH('.');
  PREP_GLYPH('(');
  PREP_GLYPH(')');
  PREP_GLYPH('#');
  PREP_GLYPH('*');
  PREP_GLYPH('<');
  PREP_GLYPH('>');
  prerender_glyph_utf8(u8"\u00B0", G_DEGREE);
  prerender_glyph_utf8(u8"\u25A1", G_BOX);
  prerender_glyph_utf8(u8"\u2190", G_ARROW_LEFT);
  prerender_glyph_utf8(u8"\u2191", G_ARROW_UP);
  prerender_glyph_utf8(u8"\u2192", G_ARROW_RIGHT);
  prerender_glyph_utf8(u8"\u2193", G_ARROW_DOWN);
  prerender_glyph_utf8(u8"\u25B3", G_TRIANGLE);
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

  for (int i = 'A'; i <= 'Z'; i++) {
    PREP_GLYPH(i);
  }
  for (int i = '0'; i <= '9'; i++) {
    PREP_GLYPH(i);
  }

  PREP_GLYPH('-');
  PREP_GLYPH('+');
  PREP_GLYPH('/');
  PREP_GLYPH('%');
  PREP_GLYPH('.');
  PREP_GLYPH('(');
  PREP_GLYPH(')');
  PREP_GLYPH('#');
  PREP_GLYPH('*');
  PREP_GLYPH('<');
  PREP_GLYPH('>');
  prerender_glyph('g', G_DEGREE);
  prerender_glyph('!', G_BOX);
  prerender_glyph('u', G_ARROW_UP);
  prerender_glyph('d', G_ARROW_DOWN);
  prerender_glyph('l', G_ARROW_LEFT);
  prerender_glyph('r', G_ARROW_RIGHT);
  prerender_glyph('t', G_TRIANGLE);

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

void
MCDUFont::prerender_glyph_utf8(const std::string &point, char glyph)
{
  SDL_Texture *rv = NULL;

  SDL_Surface *cleanGlyph = TTF_RenderUTF8_Blended(font, point.c_str(), SDL_Color{255,255,255,255});
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