/* mcdupsx.h
 *
 * Bits specific to a PSX compatible CDU
*/

#include "mcdu.h"
#include "psx.h"
#ifndef _MCDUPSX_H
#define _MCDUPSX_H

namespace mcdupsx {
    enum class PSXKey : int {
        KP_0 = 0, KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        SPACE, DELETE, SLASH, CLEAR,
        EXEC,
        SEL1, SEL2, SEL3, SEL4, SEL5, SEL6,
        MENU, NAV_RAD, PREVPAGE, NEXTPAGE,
        SER1, SER2, SER3, SER4, SER5, SER6,
        INIT_REF, RTE, DEP_ARR, ATC, VNAV, 
        FIX, LEGS, HOLD, FMC_COMM, PROG,
        PERIOD, MINUS,
        NONE = -1
    };

    enum class Position : int {
        Left, Centre, Right
    };

    class MCDUPSX : public mcdu::MCDULogic {
    public:
        const enum Position position;

        MCDUPSX(SDL_Window *win, SDL_Renderer *rend, enum Position=Position::Left, 
            const std::string &hostname="localhost", int port=10747);
        void send_key(PSXKey key);
        virtual void key_down(mcdu::Codepoint key);
        virtual void key_up(mcdu::Codepoint key);
        virtual void loop();

    protected:
        psx::SimConnectionSdl   connection;
        std::string scratchpad = "";
        bool        annun_msg = false;
        bool        under_test = false;
        bool        delete_selected = false;
        std::list<std::string>  messages;

        void update_line(int row, const std::string &psx_value);
        mcdu::CDU_Font default_font_size(int row);
        void handle_message(SDL_Event &eventInfo);
    };
};

#endif /* #ifndef _MCDUPSX_H */