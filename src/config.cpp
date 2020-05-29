/* config.cpp
 *
 * Genericised Configuration 
*/
#include "mcdu.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>
#include <sol/sol.hpp>

using namespace std;
using namespace mcdu;

static MCDUFont *
loadCDUFont(MCDULogic &cdu, const sol::table &config) {
    int fontSize = config["size"].get_or(24);
    std::string fontName = config["file"].get_or<string>("");
    std::string mapping = config["mapping"].get_or<string>("hoppie");
    if (fontName == "") {
        return nullptr;
    }
    MCDUFont *rv = new MCDUFont(cdu.cduRenderer);
    rv->antialias = config["antialias"].get_or(false);
    bool loaded = false;
    if (mapping == "hoppie") {
        loaded = rv->loadAerowinxTTF(fontName, fontSize);
    } else {
        loaded = rv->loadUnicodeTTF(fontName, fontSize);
    }
    if (!loaded) {
        delete rv;
        rv = nullptr;
    }
    return rv;
}

bool
mcdu::configureCDU(MCDULogic &cdu, const sol::table &config) {
    int fontSize = 24;

    // first up, load up the CDU fonts.
    sol::table allFonts = config["display"]["font"].get<sol::table>();

    sol::optional<sol::table> smallFont = allFonts["small"].get<sol::table>();
    sol::optional<sol::table> largeFont = allFonts["large"].get<sol::table>();

    if (smallFont.has_value()) {
        cdu.display.smallFont = loadCDUFont(cdu, smallFont.value());
        if (cdu.display.smallFont == nullptr) {
            SDL_Log("Couldn't load small font");
        }
    }
    if (largeFont.has_value()) {
        cdu.display.largeFont = loadCDUFont(cdu, largeFont.value());
        if (cdu.display.largeFont == nullptr) {
            SDL_Log("Couldn't load large font");
        }
    }

    sol::optional<string> bgImage = config["background"]["image"];
    if (bgImage.has_value()) {
        cdu.load_background(bgImage.value());
    }
    cdu.display.offset_x = config["display"]["position"]["x"].get_or(-1);
    cdu.display.offset_y = config["display"]["position"]["y"].get_or(-1);
    cdu.display.width = config["display"]["position"]["w"].get_or(-1);
    cdu.display.height = config["display"]["position"]["h"].get_or(-1);
    cdu.display.render_background = config["display"]["fill_background"].get_or(false);

    return true;
}
  

