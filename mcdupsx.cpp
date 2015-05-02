#include "mcdupsx.h"
#include "mcdu.h"
#include "psx.h"
#include <iostream>

using namespace std;
using namespace mcdu;
using namespace mcdupsx;
using namespace psx;

MCDUPSX::MCDUPSX(SDL_Window *win, SDL_Renderer *rend, enum Position position,
  const std::string &hostname, int port)
  : MCDULogic(win, rend, 28), position(position), connection(hostname, port)
{
    SDL_Surface *bgSurf = IMG_Load("resources/B744_CDU.png");
    SDL_Texture *bgtexture = SDL_CreateTextureFromSurface(rend, bgSurf);
    SDL_QueryTexture(bgtexture, NULL, NULL, &bg_size_w, &bg_size_h);
    background = bgtexture;
    display.offset_x = 89;
    display.offset_y = 60;
    display.charcell_height = 24;
    SDL_FreeSurface(bgSurf);

    connection.startListener();
}

void
MCDUPSX::send_key(PSXKey key)
{
  WirePair    wiremsg;

  switch (position) {
    case Position::Left:
      wiremsg.setKey(Qh::KeybCduL);
      break;
    case Position::Centre:
      wiremsg.setKey(Qh::KeybCduC);
      break;
    case Position::Right:
      wiremsg.setKey(Qh::KeybCduR);
      break;
    default:
      return;
  }
  wiremsg.value = to_string(static_cast<int>(key));

  // pump the wiremsg out of the network
  connection.send(wiremsg);
}


#define K_EQUIV(x) case Codepoint::x: send_key(PSXKey::x); break
#define K_MAP(x,y) case Codepoint::x: send_key(PSXKey::y); break
void
MCDUPSX::key_down(Codepoint key)
{
  switch (key) {
    K_EQUIV(A);
    K_EQUIV(B);
    K_EQUIV(C);
    K_EQUIV(D);
    K_EQUIV(E);
    K_EQUIV(F);
    K_EQUIV(G);
    K_EQUIV(H);
    K_EQUIV(I);
    K_EQUIV(J);
    K_EQUIV(K);
    K_EQUIV(L);
    K_EQUIV(M);
    K_EQUIV(N);
    K_EQUIV(O);
    K_EQUIV(P);
    K_EQUIV(Q);
    K_EQUIV(R);
    K_EQUIV(S);
    K_EQUIV(T);
    K_EQUIV(U);
    K_EQUIV(V);
    K_EQUIV(W);
    K_EQUIV(X);
    K_EQUIV(Y);
    K_EQUIV(Z);
    K_EQUIV(SPACE);
    K_EQUIV(DELETE);
    K_EQUIV(SLASH);
    K_EQUIV(CLEAR);
    K_EQUIV(KP_0);
    K_EQUIV(KP_1);
    K_EQUIV(KP_2);
    K_EQUIV(KP_3);
    K_EQUIV(KP_4);
    K_EQUIV(KP_5);
    K_EQUIV(KP_6);
    K_EQUIV(KP_7);
    K_EQUIV(KP_8);
    K_EQUIV(KP_9);
    K_MAP(FUNC1, EXEC);
    K_EQUIV(SEL1);
    K_EQUIV(SEL2);
    K_EQUIV(SEL3);
    K_EQUIV(SEL4);
    K_EQUIV(SEL5);
    K_EQUIV(SEL6);
    K_EQUIV(MENU);
    K_MAP(FUNC2, NAV_RAD);
    K_EQUIV(PREVPAGE);
    K_EQUIV(NEXTPAGE);
    K_EQUIV(SER1);
    K_EQUIV(SER2);
    K_EQUIV(SER3);
    K_EQUIV(SER4);
    K_EQUIV(SER5);
    K_EQUIV(SER6);
    K_MAP(FUNC3,INIT_REF);
    K_MAP(FUNC4,RTE);
    K_MAP(FUNC5,DEP_ARR);
    K_MAP(FUNC6,ATC);
    K_MAP(FUNC7,VNAV);
    K_MAP(FUNC8,FIX);
    K_MAP(FUNC9,LEGS);
    K_MAP(FUNC10,HOLD);
    K_MAP(FUNC11,FMC_COMM);
    K_MAP(FUNC12,PROG);
    K_EQUIV(PERIOD);
    K_EQUIV(MINUS);
  }
}

void
MCDUPSX::key_up(Codepoint key)
{
  switch(key) {
  case Codepoint::FUNC6:
  case Codepoint::CLEAR:
    WirePair msg;
    msg.setKey(Qh::KeybCduL);
    msg.value = to_string(-1);

    connection.send(msg);
    break;
  }
}

CDU_Font
MCDUPSX::default_font_size(int row) 
{
  if (row<13) {
    if (row%2) {
      return Font_Small;
    }
  }
  return Font_Large;
}

void 
MCDUPSX::update_line(int row, const std::string &psx_value)
{
  for (int i = 0; i < display.columns; i++) {
    CDU_Cell *  charcell = display.visiblePage.cell_for(row, i);
    if (i < psx_value.length()) {
      charcell->glyph = MCDUPage::codepointForChar(psx_value[i]);
    } else {
      charcell->glyph = Codepoint::NONE;
    }
    charcell->font = default_font_size(row);
    if (psx_value.length() > display.columns) {
      char fontSpec; 
      if (i + display.columns < psx_value.length()) {
        fontSpec = psx_value[i+display.columns];
      } else {
        fontSpec = psx_value[psx_value.length()-1];
      }
      switch (fontSpec) {
      case '+':
        charcell->font = Font_Large;
        break;
      case '-':
        charcell->font = Font_Small;
        break;
      }
    }
    charcell->bgcolor = C_Default;
    charcell->fgcolor = C_Default;
  }
}

void
MCDUPSX::loop()
{
  running = true;
  while(running) {
    render();

    do {
      SDL_Event eventInfo;
      if (!SDL_WaitEvent(&eventInfo)) {
        cout << SDL_GetError() << endl;
        return;
      }
      switch (eventInfo.type) {
      case SDL_QUIT:
        running = false;
        continue;
      case SDL_WINDOWEVENT:
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        handle_keypress(eventInfo);
        break;
      default:
        if (eventInfo.type == connection.simEventType) {
          handle_message(eventInfo);
        }
        break;
      }
    } while(SDL_PollEvent(NULL));
  }
}

void
MCDUPSX::handle_message(SDL_Event &eventInfo)
{
  WirePair *msg = static_cast<WirePair*>(eventInfo.user.data1);

  if (msg->key.length() <= 2) {
    return;
  }
  if (msg->key[0] != 'Q') {
    return;
  }
  switch (msg->key[1]) {
  case 's':
    std::string paramId = msg->key.substr(2);
    Qs sCode = static_cast<Qs>(stoi(paramId));
    Qs firstCode = Qs::LcduTitle;
    switch(position) {
    case Position::Left:
      firstCode = Qs::LcduTitle;
      break;
    case Position::Centre:
      firstCode = Qs::CcduTitle;
      break;
    case Position::Right:
      firstCode = Qs::RcduTitle;
      break;
    }
    if (static_cast<int>(sCode) >= static_cast<int>(firstCode) 
      && static_cast<int>(sCode) < (static_cast<int>(firstCode) + 14)) {
      int row = static_cast<int>(sCode) - static_cast<int>(firstCode);
      update_line(row, msg->value);
    }
    break;
  }
}
