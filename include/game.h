#pragma once

#include <nodePath.h>
#include <pandaFramework.h>
#include <windowFramework.h>

class Game
{
  public:
    Game() = delete;

    static WindowFramework* get_window_framework();
    static PandaFramework*  get_panda_framework();

    static NodePath get_models();
    static NodePath get_world();
    static NodePath get_camera();
    static NodePath get_render();
    static NodePath get_render2d();
    static NodePath get_pixel2d();

    static int run(int argc, char** argv);
};
