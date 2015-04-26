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

    virtual void short_press(mcdu::CDUKey key);
    virtual void long_press(mcdu::CDUKey key);
    virtual void msg_remove(const std::string &msg);
    virtual void msg_show(const std::string &msg);
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