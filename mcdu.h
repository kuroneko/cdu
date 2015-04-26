/* mcdu.h
*/

#ifndef _MCDU_H
#define _MCDU_H

#include <string>
#include <list>
#include <unordered_map>
#include <cstdio>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

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
    C_Black = 0,
    C_Magenta,
    C_Cyan,
    C_Amber,
    C_Red,
    C_White,
    C_Yellow,
    C_Green,
    C_Default  // render using the fg using the default color, and don't render any background.
  };

  enum CDU_Font {
    Font_Large = 0,
    Font_Small
  };

  struct CDU_Cell {
    char            glyph;
    enum ARINC_Color fgcolor;
    enum ARINC_Color bgcolor;
    enum CDU_Font    font;
  };

  class MCDUDisplay;
  class MCDUPage {
  public:
    MCDUPage(int rows, int columns);
    MCDUPage(const MCDUPage &original);
    ~MCDUPage();

    // dimensions (in text terms)
    const int         columns;
    const int         rows;

    void clear();
    void clear_line(int row, int startCol=0, int endCol=-1);
    void write_at(int row, int col,
      const std::string &Message,
      enum ARINC_Color fgcolor = C_Default,
      enum CDU_Font font = Font_Large,
      enum ARINC_Color bgcolor = C_Default);
    void write_at(int row, int col, const MCDUPage &page);
    void render(MCDUDisplay &display) const;
  protected:
    struct CDU_Cell *data;
    CDU_Cell * cell_for(int row, int column) const;
  };

  class MCDUDisplay {
    friend class MCDUPage;
  public:
    MCDUDisplay(SDL_Window *window, SDL_Renderer *renderer, int fontSize=24, int rows=14, int cols=24);
    ~MCDUDisplay();

    // dimensions (in text terms)
    const int         columns;
    const int         rows;
    // the current visible page (static)
    MCDUPage          visiblePage;

    // render the page using the paramters below.
    void render();

    // these two variables control the output size of the CDU panel.
    // note that the CDU panel is not clipped to this region!
    int   charcell_width;
    int   charcell_height;

    int   offset_x;
    int   offset_y;

    // render background enabled/disables the black background.
    bool  render_background = true;

    enum ARINC_Color  default_fg = C_Green;
  protected:
    // some helpers
    SDL_Color color_for_ARINCColor(enum ARINC_Color color);
    void render_cell(int row, int column, struct CDU_Cell *cell_data);


    SDL_Window *cduWindow;
    SDL_Renderer *cduRenderer;
    MCDUFont   *largeFont;
    MCDUFont   *smallFont;
  };

  enum class CDUKey : int {
    NONE = 0,  // Not a key.
    KP_0 = '0', KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9,
    // All CDUs have these...
    A = 'A', B, C, D, E, F, G, H, I, J, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    PERIOD = '.',
    PLUSMINUS = '-',
    SPACE = ' ',
    CLEAR = 256,
    MENU,
    // Most CDUs have there
    LKL1, LKL2, LKL3, LKL4, LKL5, LKL6,
    LKR1, LKR2, LKR3, LKR4, LKR5, LKR6,
    // these are reasponably universal.
    PREVPAGE, // Boeing + Universal (PREV)
    NEXTPAGE, // Boeing + Universal (NEXT)
    INIT_REF, // Boeing + Airbus (INIT)
    PROG,   // Boeing + Airbus
    NAVRAD, // Boeing + Airbus (RAD/NAV) + Universal (TUNE)
    DELETE, // Boeing
    EXEC,   // Boeing
    RTE,    // Boeing + Airbus (F-PLN) + Universal (FPL)
    DEPARR, // Boeing
    ATC,    // Boeing + Airbus (ATC COMM)
    VNAV,   // Boeing + Universal
    FIX,    // Boeing
    LEGS,   // Boeing
    HOLD,   // Boeing
    FMCCOMM, // Boeing
    OVFY,   // Airbus
    DIR,    // Airbus + Universal (DTO)
    PERF,   // Airbus + Universal (PERF)
    DATA,   // Airbus + Universal
    FUELPRED, // Airbus + Universal (FUEL)
    SECFPLN, // Airbus
    AIRPORT, // Airbus
    UP,     // Airbus
    DOWN,   // Airbus
    LEFT,   // Airbus
    RIGHT,  // Airbus
    NAV,    // Universal
    LIST,   // Universal
    MSG,    // Universal
  };

  struct CDUKeypress {
    Uint32  downTime;
    CDUKey  key;
  };

  class MCDULogic {
  public:
    SDL_Texture *background = NULL;
    int   bg_offset_x = 0;
    int   bg_offset_y = 0;
    int   bg_size_w = 0;
    int   bg_size_h = 0;
    int   long_press_threshold = 3000; // 3 seconds


    MCDULogic(SDL_Window *win, SDL_Renderer *rend, int fontsize=24);
    //~MCDULogic();

    virtual void loop();
    virtual void self_test();
    virtual void short_press(CDUKey key) = 0;
    virtual void long_press(CDUKey key) = 0;
    virtual void msg_remove(const std::string &msg) = 0;
    virtual void msg_show(const std::string &msg) = 0;

    virtual CDUKey keysymToKey(const struct SDL_Keysym &sym) const;

    MCDUDisplay   display;
  protected:
    std::list<struct CDUKeypress> keydowntimes;
    virtual void render();
    virtual void handle_keypress(SDL_Event &eventInfo);

    SDL_Window *cduWindow;
    SDL_Renderer *cduRenderer;
  };

  class MCDUBoeing : public MCDULogic {
  public:
    MCDUBoeing(SDL_Window *win, SDL_Renderer *rend);

    virtual void short_press(CDUKey key);
    virtual void long_press(CDUKey key);
    virtual void msg_remove(const std::string &msg);
    virtual void msg_show(const std::string &msg);
  protected:
    std::string scratchpad;
    bool        annun_msg;
    bool        under_test;
    bool        delete_selected;
    std::list<std::string>  messages;

    void update_scratchpad();
    void do_clear(bool longPress);
    void do_delete();
  };
};

#endif /* _MCDU_H */