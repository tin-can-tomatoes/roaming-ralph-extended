#pragma once

#include <asyncTask.h>
#include <cActor.h>
#include <collisionHandlerQueue.h>
#include <collisionNode.h>
#include <collisionRay.h>
#include <collisionTraverser.h>
#include <windowFramework.h>

#include <orbit_camera.h>

// The Player class encapsulates all the functionality
// for the single player in our game. For now, that's
// only movement with collision detection.
//
// The Player class derives from AsyncTask so that it's
// relatively straightforward to add to the global
// task manager. The overridden "do_task" method gets called
// once a frame and in turn calls move_and_collide. To add
// additional functionality to the player is easy, simply
// another method; say for example "check_interactions" and
// then also call it from "do_task"
//
// The player class is designed as a singleton which can be
// retrieved with Player::get_instance()

class Player final : public AsyncTask
{
  public:
    // Return the singleton instance of Player
    static Player* get_instance();

    // Get the Actor / NodePath of the player
    CActor* get_actor();

    bool is_enabled();
    void enable();
    void disable();

    bool is_moving();

    float get_movement_speed();
    void  set_movement_speed(float new_speed);
    float get_deadzone_factor();
    void  set_deadzone_factor(float new_fac);

    ALLOC_DELETED_CHAIN(Player);

  private:
    static Player* init_instance();
    static void    destroy_instance();

    Player();
    ~Player();

    CActor       actor;
    OrbitCamera* camera;

    bool enabled;

    bool moving;

    float movement_speed;
    float movement_deadzone_factor;

    NodePath                         collider_nodepath;
    CollisionTraverser               collision_traverser;
    PointerTo<CollisionRay>          collision_ray;
    PointerTo<CollisionNode>         collision_node;
    PointerTo<CollisionHandlerQueue> collision_queue;

    void bind_camera(OrbitCamera* cam);

    void move_and_collide();

    virtual DoneStatus do_task() override final;

    friend class Game;
};
