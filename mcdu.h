/* mcdu.h
*/

#ifndef _MCDU_H
#define _MCDU_H

#include <string>
#include <cstdio>
#include <SDL.h>
#include <SDL_ttf.h>

#include "ss.h"

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
    // handle to the renderer (required for initilisation/loading of fonts)
    SDL_Renderer *  renderer;
    // construct an empty Font
    MCDUFont(SDL_Renderer *renderer = NULL);

    // constructs a MCDU font for the given font
    MCDUFont(SDL_Renderer *renderer, const std::string &fontname, int basesize);
    virtual ~MCDUFont();

    bool  loadAerowinxTTF(const std::string &fontname, int size);
    bool  loadUnicodeTTF(const std::string &fontname, int size);

    // returns a GPU_Image for the requested codepoint
    SDL_Texture * glyphFor(int point);

    int max_width;
    int max_height;
  private:
    // internal array of standard glyphs.    
    SDL_Texture *   glyphs[256];

    // font data
    TTF_Font *font;

    void prerender_glyph(char point, char glyph);
    void prerender_glyph_utf8(const std::string &point, char glyph);
    void close_font();
    bool open_font(const std::string &fontname, int size);
  };

  class MCDU {
  public:
    MCDU(SDL_Window *window, SDL_Renderer *renderer);
    ~MCDU();

    void render();

  private:
    int largeFontHeight;

    void render_line(const std::string &text, int row, bool large=true);

    SDL_Window *cduWindow;
    SDL_Renderer *cduRenderer;
    MCDUFont   *largeFont;
    MCDUFont   *smallFont;
    // dimensions (in text terms)
    int         columns;
    int         rows;
  };
};

#endif /* _MCDU_H */