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

namespace libconfig {
	class Config;
};

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

		void prerender_glyph(int point, Codepoint glyph);
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
    	// dimensions (in text terms)
		const int         columns;
		const int         rows;

		static Codepoint codepointForChar(char charIn);

		MCDUPage(int rows, int columns);
		MCDUPage(const MCDUPage &original);
		~MCDUPage();

		void render(MCDUDisplay &display, SDL_Texture *destTexture, SDL_Texture *fgLayer) const;

		void clear();
		void clear_line(int row, int startCol=0, int endCol=-1);
		void write_at(int row, int col,
			const std::string &Message,
			enum ARINC_Color fgcolor = C_Default,
			enum CDU_Font font = Font_Large,
			enum ARINC_Color bgcolor = C_Default);
		void write_at(int row, int col, const MCDUPage &page);
		CDU_Cell * cell_for(int row, int column) const;
	protected:
		struct CDU_Cell *data;
	};

	class MCDUDisplay {
		friend class MCDUPage;
	public:
		MCDUDisplay(SDL_Renderer *renderer, int rows=14, int cols=24);
		~MCDUDisplay();

	    // dimensions (in text terms)
		const int         columns;
		const int         rows;
	    // the current visible page (static)
		MCDUPage          visiblePage;
		bool              blank_display = false;

    	// render the page using the paramters below.
		SDL_Texture *			render();

		int 	width;
		int   height;

  	// render background enabled/disables the black background.
		bool  render_background = true;

		enum ARINC_Color  default_fg = C_Green;

		MCDUFont   *largeFont;
		MCDUFont   *smallFont;
	protected:
    	// some helpers
		SDL_Color color_for_ARINCColor(enum ARINC_Color color);

		void render_cell(int row, int column, struct CDU_Cell *data, SDL_Texture *bgLayer, SDL_Texture *fgLayer);

		SDL_Renderer *cduRenderer;
	};

	struct CDUKeypress {
		Uint32  	downTime;
		Codepoint 	point;
	};

	// abstract class for building the touch/mouse responsive areas
	class ScreenBox {
	public:
		int 		x, y;
		int 		w, h;

		ScreenBox(int x, int y, int w, int h)
		: x(x), y(y), w(w), h(h)
		{
		}

		bool 		pointWithin(int px, int py) {
			if (px >= x && px <= (x+w) && py >= y && py <= (y+h)) {
				return true;
			}
			return false;
		}

		virtual void	pressed() = 0;
		virtual void 	released() = 0;
	};

	class MCDULogic {
	public:
		SDL_Texture *background = NULL;
		int   bg_offset_x = 0;
		int   bg_offset_y = 0;
		int   bg_size_w = 0;
		int   bg_size_h = 0;
		int 	display_offset_x = 0;
		int   display_offset_y = 0;
    int   long_press_threshold = 1500; // 1.5 seconds
    int     output_w;
    int     output_h;
  	bool  running = true;
  	double	renderScale = 1.0;
  	std::list<ScreenBox*>	interactionRegions;
  	ScreenBox *		mousePressed = NULL;
  	MCDUDisplay   display;
		SDL_Renderer *cduRenderer;

  	MCDULogic(SDL_Renderer *rend);
  	//~MCDULogic();
		virtual void autoscale();
  	virtual void loop();
  	virtual void key_down(Codepoint key) = 0;
  	virtual void key_up(Codepoint key) = 0;
  	virtual void self_test();

  	virtual Codepoint keysymToPoint(const struct SDL_Keysym &sym) const;
    void reset_background();
    void load_background(const std::string &filename);

	protected:
		virtual void render();
		virtual void handle_keypress(SDL_Event &eventInfo);
  	virtual void handle_mousedown(const SDL_Event &eventInfo);
  	virtual void handle_mouseup(const SDL_Event &eventInfo);
  	virtual bool handle_other(SDL_Event &eventInfo);

	};

	class MCDUScreenKey : public ScreenBox {
	public:
		MCDULogic &		cdu;
		Codepoint 		key;

		MCDUScreenKey(int x, int y, int w, int h, MCDULogic &cdu, Codepoint key) 
		: ScreenBox(x,y,w,h), cdu(cdu), key(key)
		{
		}

		virtual void pressed() {
			cdu.key_down(key);
		}
		virtual void released() {
			cdu.key_up(key);
		}
	};

	class SmartLogic : public MCDULogic {
	public:
		std::list<struct CDUKeypress> keydowntimes;

		SmartLogic(SDL_Renderer *rend);

		virtual void loop();  
		virtual void key_down(Codepoint key);
		virtual void key_up(Codepoint key);

		virtual void short_press(Codepoint key) = 0;
		virtual void long_press(Codepoint key) = 0;
		virtual void msg_remove(const std::string &msg) = 0;
		virtual void msg_show(const std::string &msg) = 0;
		virtual bool can_long_press(Codepoint key) = 0;
	};

	bool configureCDU(MCDULogic &cdu, libconfig::Config &config);
};

#endif /* _MCDU_H */