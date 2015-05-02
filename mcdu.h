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
  enum class Codepoint : int {
    NONE = 0x100,  // Not a key.
    CLEAR = 0x08,
    DEGREE = 0x1C,
    BOX = 0x1D,
    DOWN = 0x1E, 
    RIGHT = 0x1F, 
    SPACE = ' ',
    BANG ='!',
    QUOTE = '\"', 
    HASH = '#',
    PERCENT = '%',
    AMPERSAND = '&', 
    APOSTROPHE = '\'', 
    LPAREN = '(', 
    RPAREN = ')',
    ASTERISK = '*', 
    PLUS = '+', 
    COMMA = ',', 
    MINUS = '-', 
    PERIOD = '.',
    SLASH = '/',
    KP_0 = '0', KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9,
    COLON = ':', 
    SEMICOMMA = ';', 
    LT = '<', 
    EQ = '=', 
    GT = '>', 
    QMARK = '?',
    A = 'A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, 
    W, X, Y, Z,
    LBRACKET = '[',
    BACKSLASH = '\\',
    RBRACKET = ']', 
	UP = 0x5E,
	LEFT = 0x5F,
    
    // 0x60 onwards
    MENU = 0x60, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5, FUNC6, FUNC7,
    FUNC8, FUNC9, FUNC10, FUNC11, FUNC12, FUNC13, TRIANGLE, RING,
    // 0x70
    SEL1, SEL2, SEL3, SEL4, SEL5, SEL6,
    PREVPAGE, NEXTPAGE,
    SER1, SER2, SER3, SER4, SER5, SER6,
    DELETE = 0x7F
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
    SDL_Texture * glyphFor(Codepoint point);

    int max_width;
    int max_height;
  private:
    // internal array of standard glyphs.    
    SDL_Texture *   glyphs[256];

    // font data
    TTF_Font *font;

    void prerender_glyph(char point, Codepoint glyph);
    void prerender_glyph_utf8(const std::string &point, Codepoint glyph);
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
    Codepoint        glyph;
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

    static Codepoint codepointForChar(char charIn);

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
    bool              blank_display = false;

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

  struct CDUKeypress {
    Uint32  	downTime;
    Codepoint 	point;
  };

  class MCDULogic {
  public:
    SDL_Texture *background = NULL;
    int   bg_offset_x = 0;
    int   bg_offset_y = 0;
    int   bg_size_w = 0;
    int   bg_size_h = 0;
    int   long_press_threshold = 1500; // 1.5 seconds

    bool  running = true;

    MCDULogic(SDL_Window *win, SDL_Renderer *rend, int fontsize=24);
    //~MCDULogic();

    virtual void loop();
    virtual void key_down(Codepoint key) = 0;
    virtual void key_up(Codepoint key) = 0;
    virtual void self_test();

    virtual Codepoint keysymToPoint(const struct SDL_Keysym &sym) const;

    MCDUDisplay   display;
  protected:
    virtual void render();
    virtual void handle_keypress(SDL_Event &eventInfo);

    SDL_Window *cduWindow;
    SDL_Renderer *cduRenderer;
  };

  class SmartLogic : public MCDULogic {
  public:
    std::list<struct CDUKeypress> keydowntimes;

    SmartLogic(SDL_Window *win, SDL_Renderer *rend, int fontsize=24);

    virtual void loop();  
    virtual void key_down(Codepoint key);
    virtual void key_up(Codepoint key);

    virtual void short_press(Codepoint key) = 0;
    virtual void long_press(Codepoint key) = 0;
    virtual void msg_remove(const std::string &msg) = 0;
    virtual void msg_show(const std::string &msg) = 0;
    virtual bool can_long_press(Codepoint key) = 0;
  };
};

#endif /* _MCDU_H */