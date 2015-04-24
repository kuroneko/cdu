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

  enum ARINC_Color : int {
    CDU_Black = 0,
    CDU_Magenta,
    CDU_Cyan,
    CDU_Amber,
    CDU_Red,
    CDU_White,
    CDU_Yellow,
    CDU_Green
  };

  enum CDU_Font {
    Font_Large = 0,
    Font_Small
  };

  struct CDU_Cell {
    char            glyph;
    enum ARINC_Color color;
    enum CDU_Font    font;
  };

  class MCDUDisplay {
  public:
    MCDUDisplay(SDL_Window *window, SDL_Renderer *renderer, int fontSize=24, int rows=14, int cols=24);
    ~MCDUDisplay();

    void render(int xoffs, int yoffs);
    void clear();

    void write_at(int row, int col, enum CDU_Font font, enum ARINC_Color color, const std::string &Message);

    void self_test();

    // these two variables control the output size of the CDU panel.
    // note that the CDU panel is not clipped to this region!
    int   charcell_width;
    int   charcell_height;

  private:
    void render_cell(int xoffs, int yoffs, int row, int column);

    CDU_Cell *  cell_for(int row, int column);

    SDL_Window *cduWindow;
    SDL_Renderer *cduRenderer;
    MCDUFont   *largeFont;
    MCDUFont   *smallFont;
    // dimensions (in text terms)
    int         columns;
    int         rows;
    struct CDU_Cell *data;

    // some helpers
    SDL_Color color_for_ARINCColor(enum ARINC_Color color);
  };

  class MCDULogic {
  public:
    int   display_offset_x = 0;
    int   display_offset_y = 0;
    
    MCDULogic(SDL_Window *win, SDL_Renderer *rend, int fontsize=24);
    //~MCDULogic();

    void loop();

    MCDUDisplay   display;
  };


};

#endif /* _MCDU_H */