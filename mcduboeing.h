/* mcduboeing.h
 *
 * Bits specific to the Boeing-style MCDU
 */

 #include "mcdu.h"

 #ifndef _MCDUBOEING_H
 #define _MCDUBOEING_H

 namespace mcduboeing {
  class MCDUBoeing : public mcdu::MCDULogic {
  public:
    MCDUBoeing(SDL_Window *win, SDL_Renderer *rend);

    virtual bool can_long_press(mcdu::Codepoint key);
    virtual void short_press(mcdu::Codepoint key);
    virtual void long_press(mcdu::Codepoint key);
    virtual void msg_remove(const std::string &msg);
    virtual void msg_show(const std::string &msg);

    static const mcdu::Codepoint  Key_Exec = mcdu::Codepoint::FUNC1;
    static const mcdu::Codepoint  Key_NavRad = mcdu::Codepoint::FUNC2;
    static const mcdu::Codepoint  Key_InitRef = mcdu::Codepoint::FUNC3;
    static const mcdu::Codepoint  Key_Rte = mcdu::Codepoint::FUNC4;
    static const mcdu::Codepoint  Key_DepArr = mcdu::Codepoint::FUNC5;
    static const mcdu::Codepoint  Key_ATC = mcdu::Codepoint::FUNC6;
    static const mcdu::Codepoint  Key_VNav = mcdu::Codepoint::FUNC7;
    static const mcdu::Codepoint  Key_Fix = mcdu::Codepoint::FUNC8;
    static const mcdu::Codepoint  Key_Legs = mcdu::Codepoint::FUNC9;
    static const mcdu::Codepoint  Key_Hold = mcdu::Codepoint::FUNC10;
    static const mcdu::Codepoint  Key_FmcComm = mcdu::Codepoint::FUNC11;
    static const mcdu::Codepoint  Key_Prog = mcdu::Codepoint::FUNC12;
    
  protected:
    std::string scratchpad = "";
    bool        annun_msg = false;
    bool        under_test = false;
    bool        delete_selected = false;
    std::list<std::string>  messages;

    void update_scratchpad();
    void do_clear(bool longPress);
    void do_delete();
  };
};

 #endif /* #ifndef _MCDUBOEING_H */