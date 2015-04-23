/* mcdu.h
*/

#ifndef _MCDU_H
#define _MCDU_H

#include <string>
#include <cstdio>
#include <SDL.h>
#include <SDL_gpu.h>
#include <SDL_ttf.h>

namespace mcdu {

  enum CDUGlyph {
    G_DEGREE = '\'',
    G_BOX = '!',
    G_ARROW_UP = '^',
    G_ARROW_DOWN = '?',
    G_ARROW_LEFT = ':',
    G_ARROW_RIGHT = ';',
    G_TRIANGLE = '~',
  };

  class MCDUFont {
  public:
    // constructs a MCDU font for the given font
    MCDUFont(const std::string &fontname, int basesize);
    virtual ~MCDUFont();

    // returns a GPU_Image for the requested codepoint
    const GPU_Image * glyphFor(int point);
  private:
    // internal array of standard glyphs.    
    GPU_Image *   glyphs[256];
    static GPU_Image *prerender_glyph(TTF_Font *font, char point);
  };


}

#endif /* _MCDU_H */