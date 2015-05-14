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
#define PREP_GLYPH(x) prerender_glyph(x, static_cast<Codepoint>(x))
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
  prerender_glyph(0x00B0, Codepoint::DEGREE);
  prerender_glyph(0x25A1, Codepoint::BOX);
  prerender_glyph(0x2190, Codepoint::LEFT);
  prerender_glyph(0x2191, Codepoint::UP);
  prerender_glyph(0x2192, Codepoint::RIGHT);
  prerender_glyph(0x2193, Codepoint::DOWN);
  prerender_glyph(0x25B3, Codepoint::TRIANGLE);
  
  return true;
}

/* This function specifically knows how to glyph-map the TTF font from Hoppie's CDU 3.0 
*/
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
  prerender_glyph('b', Codepoint::DEGREE);
  prerender_glyph('c', Codepoint::BOX);
  prerender_glyph('^', Codepoint::UP);
  prerender_glyph('d', Codepoint::DOWN);
  prerender_glyph('_', Codepoint::LEFT);
  prerender_glyph('e', Codepoint::RIGHT);
  prerender_glyph('n', Codepoint::TRIANGLE);
  prerender_glyph('o', Codepoint::RING);

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
  //FIXME: let this be set in config
  TTF_SetFontHinting(font, TTF_HINTING_NONE);
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
MCDUFont::prerender_glyph(int point, Codepoint glyph)
{
  SDL_Texture *rv = NULL;

  SDL_Surface *cleanGlyph = NULL;
	  
  if (antialias) {
    cleanGlyph = TTF_RenderGlyph_Blended(font, point, SDL_Color{255,255,255,255});
  } else {
    cleanGlyph = TTF_RenderGlyph_Solid(font, point, SDL_Color{255,255,255,255});
  }

  if (cleanGlyph->w > max_width) {
    max_width = cleanGlyph->w;
  }
  if (cleanGlyph->h > max_height) {
    max_height = cleanGlyph->h;
  }
  glyphs[static_cast<unsigned char>(glyph)] = SDL_CreateTextureFromSurface(renderer, cleanGlyph);
  SDL_FreeSurface(cleanGlyph);
}

SDL_Texture *
MCDUFont::glyphFor(Codepoint point)
{
  if (static_cast<int>(point) > 0xFF) {
    return NULL;
  }
  return glyphs[static_cast<unsigned char>(point)];
}
