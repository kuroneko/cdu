/* font.cpp
 *
 * Font and Glyph Processing for MCDU
*/

#include "mcdu.h"

using namespace std;
using namespace mcdu;

MCDUFont::MCDUFont(const std::string &fontname, int basesize)
{
  TTF_Font *theFont = TTF_OpenFont(fontname.c_str(), basesize);
  if (NULL == theFont) {
    //FIXME: throw exception to indicate failure
    return;
  }
  // now, build the glyph table
  for (int i = 0; i < 256; i++) {
    glyphs[i] = NULL;
  }

#define PREP_GLYPH(x) glyphs[x] = prerender_glyph(theFont, x)
  for (int i = 'A'; i < 'Z'; i++) {
    PREP_GLYPH(i);
  }
  for (int i = '0'; i < '9'; i++) {
    PREP_GLYPH(i);
  }
  PREP_GLYPH(G_DEGREE);
  PREP_GLYPH(G_BOX);
  PREP_GLYPH(G_ARROW_UP);
  PREP_GLYPH(G_ARROW_DOWN);
  PREP_GLYPH(G_ARROW_LEFT);
  PREP_GLYPH(G_ARROW_RIGHT);
  PREP_GLYPH(G_TRIANGLE);

  TTF_CloseFont(theFont);
}

MCDUFont::~MCDUFont()
{
  // release any loaded glyphs
  for (int i = 0; i < 256; i++) {
    if (glyphs[i] != NULL) {
      GPU_FreeImage(glyphs[i]);
      glyphs[i] = NULL;
    }
  }
}

GPU_Image *
MCDUFont::prerender_glyph(TTF_Font *font, char point)
{
  GPU_Image *rv = NULL;
  char    sbuf[2] = {point, '\0'};

  SDL_Surface *cleanGlyph = TTF_RenderText_Blended(font, sbuf, SDL_Color{255,255,255,255});
  rv = GPU_CopyImageFromSurface(cleanGlyph);
  SDL_FreeSurface(cleanGlyph);

  return rv;
}

const GPU_Image *
MCDUFont::glyphFor(int point)
{
  return glyphs[point];
}