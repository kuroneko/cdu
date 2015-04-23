#include <string>
#include <cstdio>
#include <SDL.h>
#include <SDL_gpu.h>
#include <SDL_ttf.h>
#include "ss.h"

using namespace std;


class MCDU {
public:
  MCDU(GPU_Target *window) {
    cduWindow = window;
    largeFontHeight = 24;


    largeFont = TTF_OpenFont("resources/Inconsolata.otf", largeFontHeight);
    TTF_SetFontHinting(largeFont, TTF_HINTING_MONO);
    smallFont = TTF_OpenFont("resources/Inconsolata.otf", largeFontHeight);
    TTF_SetFontHinting(smallFont, TTF_HINTING_MONO);
  }

  ~MCDU() {
    TTF_CloseFont(largeFont);
    TTF_CloseFont(smallFont);
  }

  void render() {
    GPU_Clear(cduWindow);
    render_line("MCDU TEST 0.1           ", 0, true);
    render_line("123456789012345678901234", 1, false);
    render_line("123456789012345678901234", 2, true);
    render_line("ABCDEFGHIJKLMNOPQRSTUVWX", 3, false);
    render_line("YZ1234567890!@#$%^&*()[]", 4, true);
    GPU_FlushBlitBuffer();
    GPU_Flip(cduWindow);
  }

private:
  int largeFontHeight;

  void render_line(const std::string &text, int row, bool large=true) {
    SDL_Color   textColor = {0, 255, 0, 255};
    TTF_Font *font = NULL;

    if (large) {
      font = largeFont;
    } else {
      font = smallFont;
    }

    SDL_Surface *textSurf = TTF_RenderText_Blended(font, text.c_str(), textColor);
    GPU_Image *textImage = GPU_CopyImageFromSurface(textSurf);
    GPU_Rect  srcRect = { 0, 0, textImage->w, textImage->h};

    GPU_BlitScale(textImage, &srcRect, cduWindow, (textImage->w/2), (textImage->h/2)+(large?0:(largeFontHeight/3)) + (row*largeFontHeight), 1.0, (large?1.0:0.6));

    GPU_FreeImage(textImage);
    SDL_FreeSurface(textSurf);
  }

  GPU_Target *cduWindow;
  TTF_Font   *largeFont;
  TTF_Font   *smallFont;  
};

#ifdef __cplusplus
extern "C"
#endif
int
main(int argc, char **argv)
{
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  GPU_Target *cduWindow = NULL;
  cduWindow = GPU_Init(640, 800, SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);

  MCDU *cdu = new MCDU(cduWindow);

  while(true) {
    cdu->render();
  }

}

