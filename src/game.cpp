#include <game.h>

#include <config.h>

#include <ambientLight.h>
#include <asyncTaskManager.h>
#include <directionalLight.h>
#include <nodePath.h>
#include <pandaFramework.h>
#include <pointerTo.h>
#include <windowFramework.h>

#include <input_control.h>
#include <input_controller.h>
#include <orbit_camera.h>
#include <player.h>

static PandaFramework   framework;
static WindowFramework* window;

static NodePath models;
static NodePath world;
static NodePath camera;
static NodePath render;
static NodePath render2d;
static NodePath pixel2d;

static AsyncTaskManager* task_mgr;

WindowFramework* Game::get_window_framework() { return window; }
PandaFramework*  Game::get_panda_framework() { return &framework; }
NodePath         Game::get_models() { return models; }
NodePath         Game::get_world() { return world; }
NodePath         Game::get_camera() { return camera; }
NodePath         Game::get_render() { return render; }
NodePath         Game::get_render2d() { return render2d; }
NodePath         Game::get_pixel2d() { return pixel2d; }

int Game::run(int argc, char** argv)
{
  framework.open_framework(argc, argv);
  framework.set_window_title(GAME_TITLE_STR);
  window   = framework.open_window();
  models   = framework.get_models();
  camera   = window->get_camera_group();
  render   = window->get_render();
  render2d = window->get_render_2d();
  pixel2d  = window->get_pixel_2d();
  window->set_background_type(WindowFramework::BT_black);

  task_mgr = AsyncTaskManager::get_global_ptr();

  world = window->load_model(models, "models/world.egg");
  world.reparent_to(render);
  world.set_pos(0, 0, 0);

  // Set up all the static instances now that framework is ready
  InputController::init_instance(&framework);
  Player* player = Player::init_instance();

  PointerTo<OrbitCamera> camera_controller =
      new OrbitCamera(window, "cameraRay", camera, player->get_actor());

  player->bind_camera(camera_controller.p());

  // Create some lighting
  PointerTo<AmbientLight> ambientLightPtr = new AmbientLight("ambientLight");
  assert(ambientLightPtr != nullptr);
  ambientLightPtr->set_color(LColorf(.3, .3, .3, 1));
  render.set_light(render.attach_new_node(ambientLightPtr));

  PointerTo<DirectionalLight> directionalLightPtr =
      new DirectionalLight("directionalLightPtr");
  assert(directionalLightPtr != nullptr);
  directionalLightPtr->set_direction(LVecBase3f(-5, -5, -5));
  directionalLightPtr->set_color(LColorf(1, 1, 1, 1));
  directionalLightPtr->set_specular_color(LColorf(1, 1, 1, 1));
  render.set_light(render.attach_new_node(directionalLightPtr));

  // Since class Player is derived from AsyncTask,
  // we just gotta add the player instance to the TaskManager
  task_mgr->add(Player::get_instance());

  // Same goes for the camera controller
  task_mgr->add(camera_controller);

  framework.main_loop();
  framework.close_framework();
  return 0;
}
