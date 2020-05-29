/* psxmain.cpp
 *
 * Standalone MCDU for Precision Simulator X
 *
 * Copyright (C) 2015, Christopher Collins
 */
#include <string>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>
#include <sol/sol.hpp>

#include "mcdu.h"
#include "mcdupsx.h"

using namespace std;
using namespace mcdupsx;

sol::state *luaState = nullptr;

int
main(int argc, char **argv) {
    luaState = new sol::state;

    luaState->open_libraries(sol::lib::base,
                             sol::lib::package,
                             sol::lib::math,
                             sol::lib::coroutine,
                             sol::lib::os,
                             sol::lib::ffi,
                             sol::lib::string,
                             sol::lib::table,
                             sol::lib::utf8,
                             sol::lib::bit32,
                             sol::lib::count,
                             sol::lib::io,
                             sol::lib::jit);

    sol::optional<string> packagePath = (*luaState)["package"]["path"].get<string>();
    if (packagePath.has_value() && !packagePath.value().empty()) {
        luaState->globals()["package"]["path"] = "./resources/?.lua;" + packagePath.value();
    } else {
        luaState->globals()["package"]["path"] = "./resources/?.lua";
    }

    sol::object cduConfigObj = luaState->script_file("mcdupsx.lua");
    sol::table cduConfigTable = cduConfigObj.as<sol::table>();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    // CDU logical configuration
    Position position = Position::Left;
    string positionName = cduConfigTable["position"].get_or<string>("left");
    if (positionName == "left") {
        position = Position::Left;
    } else if (positionName == "centre" || positionName == "center") {
        position = Position::Centre;
    } else if (positionName == "right") {
        position = Position::Right;
    }

    // network parameters
    string connHostname = cduConfigTable["psx"]["hostname"].get_or<string>("");
    int connPort = cduConfigTable["psx"]["port"].get_or(10747);
    if (connHostname.empty()) {
        cerr << "Configuration must specify the host to connect to!" << endl;
        exit(1);
    }

    // screen configuration
    bool fullScreen = cduConfigTable["window"]["fullscreen"].get_or(false);
    int xres = cduConfigTable["window"]["x"].get_or(640);
    int yres = cduConfigTable["window"]["y"].get_or(480);
    bool grabFocus = cduConfigTable["window"]["grabfocus"].get_or(false);

    SDL_Window *win = nullptr;
    SDL_Renderer *rend = nullptr;
    Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    if (fullScreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (grabFocus) {
        flags |= SDL_WINDOW_INPUT_GRABBED;
    }
    if (SDL_CreateWindowAndRenderer(xres, yres, flags, &win, &rend)) {
        cout << "ERROR: " << SDL_GetError() << endl;
        exit(1);
    }

    SDL_RendererInfo ri;
    if (SDL_GetRendererInfo(rend, &ri)) {
        cerr << "Failed to get SDL Renderer Info: " << SDL_GetError() << endl;
        return 1;
    }
    cout << "Renderer: " << ri.name << endl;

    // not a loop - just an excuse to ensure mcdu gets destroyed before we shutdown SDL. (we get panics otherwise)
    do {
        MCDUPSX mcdu(rend, position, connHostname, connPort);
        auto mcduConfig = cduConfigTable["mcdu"].get<sol::table>();
        // load the common parameters
        if (!configureCDU(mcdu, mcduConfig)) {
            cerr << "Failed to load configuration" << endl;
            break;
        }
        mcdu.loop();
    } while (0);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
  

