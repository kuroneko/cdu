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
#include <libconfig.h++>

using namespace std;
using namespace mcdu;
using namespace libconfig;

static MCDUFont *
loadCDUFont(MCDULogic &cdu, Config &config, const std::string &fontsuffix)
{
  int fontSize = 24;
  std::string fontName = "";
  std::string mapping = "hoppie";

  config.lookupValue("mcdu.display.font." + fontsuffix + ".size", fontSize);
  config.lookupValue("mcdu.display.font." + fontsuffix + ".file", fontName);
  config.lookupValue("mcdu.display.font." + fontsuffix + ".mapping", mapping);

  if (fontName == "") {
    return NULL;
  }

  MCDUFont *rv = new MCDUFont(cdu.cduRenderer);
  config.lookupValue("mcdu.display.font." + fontsuffix + ".antialias", rv->antialias);
  bool loaded = false;
  if (mapping == "hoppie") {
    loaded = rv->loadAerowinxTTF(fontName, fontSize);
  } else {
    loaded = rv->loadUnicodeTTF(fontName, fontSize);
  }
  if (!loaded) {
    delete rv;
    rv = NULL;
  }
  return rv;
}

bool
mcdu::configureCDU(MCDULogic &cdu, Config &config)
{
  int fontSize = 24;

  // first up, load up the CDU fonts.

  cdu.display.smallFont = loadCDUFont(cdu, config, "small");
  cdu.display.largeFont = loadCDUFont(cdu, config, "large");

  if (config.exists("mcdu.background.image")) {
    cdu.load_background(config.lookup("mcdu.background.image"));
  }
  config.lookupValue("mcdu.display.position.x", cdu.display.offset_x);
  config.lookupValue("mcdu.display.position.y", cdu.display.offset_y);
  config.lookupValue("mcdu.display.position.w", cdu.display.width);
  config.lookupValue("mcdu.display.position.h", cdu.display.height);
  config.lookupValue("mcdu.display.fill_background", cdu.display.render_background);
}
  

