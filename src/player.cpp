#include <player.h>

#include <asyncTask.h>
#include <bitMask.h>
#include <cActor.h>
#include <clockObject.h>
#include <collisionHandlerQueue.h>
#include <collisionNode.h>
#include <collisionRay.h>
#include <deg_2_rad.h>
#include <lvector2.h>
#include <lvector3.h>
#include <nodePath.h>

#include <game.h>
#include <input_controller.h>
#include <orbit_camera.h>

static Player* player_instance = nullptr;

Player* Player::get_instance()
{
  assert(player_instance != nullptr);
  return player_instance;
}

Player* Player::init_instance()
{
  assert(player_instance == nullptr);
  player_instance = new Player();
  assert(player_instance != nullptr);
  return player_instance;
}

void Player::destroy_instance()
{
  assert(player_instance != nullptr);
  delete player_instance;
  player_instance = nullptr;
}

Player::Player()
    : actor(), camera(nullptr), enabled(false), moving(false), movement_speed(4.0f),
      movement_deadzone_factor(0.05f), collider_nodepath()
{
  LPoint3f startPos = Game::get_world().find("**/start_point").get_pos();

  CActor::AnimMap animations;
  animations["models/ralph-run.egg"].push_back("run");
  animations["models/ralph-walk.egg"].push_back("walk");

  actor.load_actor(
      Game::get_window_framework(),
      "models/ralph.egg",
      &animations,
      PartGroup::HMF_ok_wrong_root_name | PartGroup::HMF_ok_anim_extra
          | PartGroup::HMF_ok_part_extra
  );

  actor.set_scale(0.2);
  actor.set_pos(startPos);

  enable();

  // time to setup colliders
  //
  // We will detect the height of the terrain by creating a collision
  // ray and casting it downward toward the terrain. If it hits the
  // terrain, we can detect the height. If it hits anything else, we
  // rule that the move is illegal.
  collision_ray = new CollisionRay();
  assert(collision_ray != nullptr);
  collision_ray->set_origin(0, 0, 1000);
  collision_ray->set_direction(0, 0, -1);

  collision_node = new CollisionNode("playerRay");
  assert(collision_node != nullptr);
  collision_node->add_solid(collision_ray);
  collision_node->set_from_collide_mask(BitMask32::bit(0));
  collision_node->set_into_collide_mask(BitMask32::all_off());

  collider_nodepath = actor.attach_new_node(collision_node);

  collision_queue = new CollisionHandlerQueue();
  assert(collision_queue != nullptr);
  collision_traverser.add_collider(collider_nodepath, collision_queue);
}

Player::~Player() { }

CActor* Player::get_actor() { return &actor; }

bool Player::is_enabled() { return enabled; }

void Player::enable()
{
  actor.reparent_to(Game::get_render());
  enabled = true;
}

void Player::disable()
{
  actor.remove_node();
  enabled = false;
}

void Player::bind_camera(OrbitCamera* cam) { this->camera = cam; }

bool Player::is_moving() { return moving; }

float Player::get_movement_speed() { return movement_speed; }
void  Player::set_movement_speed(float new_speed) { movement_speed = new_speed; }

float Player::get_deadzone_factor() { return movement_deadzone_factor; }
void  Player::set_deadzone_factor(float new_fac)
{
  movement_deadzone_factor = new_fac;
}

void Player::move_and_collide()
{
  if (!enabled)
  {
    return;
  }

  InputController* input        = InputController::get_instance();
  double           dt           = ClockObject::get_global_clock()->get_dt();
  LPoint3f         start_pos    = actor.get_pos();
  LVector2f        movement_vec = input->get_movement_vector();

  bool      was_moving = moving;
  LVector3f forward    = camera == nullptr
                           ? LVector3f::forward()
                           : camera->get_camera().get_quat().get_forward();

  forward.set_z(0);
  forward.normalize();

  LVector3f right = forward.cross(LVector3f::unit_z());
  right.normalize();

  LVector3f movement = forward * movement_vec.get_y() + right * movement_vec.get_x();
  moving             = movement.length_squared() > get_deadzone_factor();

  if (moving)
  {
    movement.normalize();
    actor.set_pos(start_pos + movement * dt * movement_speed);
    actor.set_h(0 - rad_2_deg(atan2(0 - movement.get_x(), 0 - movement.get_y())));
  }

  // Update animation
  if (moving != was_moving)
  {
    if (moving)
    {
      actor.loop("run", true);
    }
    else
    {
      actor.stop("run");
      actor.pose("walk", 5);
    }
  }

  // Check for collisions
  collision_traverser.traverse(Game::get_render());
  collision_queue->sort_entries();
  if (collision_queue->get_num_entries() > 0
      && collision_queue->get_entry(0)->get_into_node()->get_name() == "terrain")
  {
    actor.set_z(
        collision_queue->get_entry(0)->get_surface_point(Game::get_render()).get_z()
    );
  }
  else
  {
    actor.set_pos(start_pos);
  }
}

AsyncTask::DoneStatus Player::do_task()
{
  move_and_collide();
  return DS_cont;
}
