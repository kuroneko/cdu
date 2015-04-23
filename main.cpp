#include <cstdio>
#include <clutter/clutter.h>
#include "ss.h"

using namespace std;

ClutterActor *    myStage;
ClutterActor *    bgImage;
int
main(int argc, char **argv)
{
	clutter_init(&argc, &argv);

  myStage = clutter_stage_new();
  clutter_stage_set_title(CLUTTER_STAGE(myStage), "MCDU");

  bgImage = clutter_texture_new_from_file("resources/B744_MCDU_1170_lit.png", NULL);
  if (NULL != bgImage) {
    gfloat xsize, ysize;
    clutter_actor_get_size(bgImage, &xsize, &ysize);

    clutter_actor_set_size(myStage, xsize, ysize);
    clutter_actor_add_child(myStage, bgImage);

  }

  clutter_actor_show(myStage);
  clutter_main();
}

