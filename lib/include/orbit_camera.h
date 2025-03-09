#pragma once

#include <asyncTask.h>
#include <collisionHandlerQueue.h>
#include <collisionRay.h>
#include <collisionTraverser.h>
#include <lvector2.h>
#include <nodePath.h>
#include <windowFramework.h>

#include <input_controller.h>

class OrbitCamera final : public AsyncTask
{
  public:
    OrbitCamera(
        WindowFramework*  window_framework,
        const char* const ray_name,
        NodePath          camera_node,
        NodePath          target_node
    );
    OrbitCamera(
        WindowFramework*  window_framework,
        const char* const ray_name,
        NodePath*         camera_node,
        NodePath          target_node
    );
    OrbitCamera(
        WindowFramework*  window_framework,
        const char* const ray_name,
        NodePath          camera_node,
        NodePath*         target_node
    );
    OrbitCamera(
        WindowFramework*  window_framework,
        const char* const ray_name,
        NodePath*         camera_node,
        NodePath*         target_node
    );

    bool is_enabled();
    void enable();
    void disable();
    void toggle_enabled();

    NodePath get_camera();
    void     set_camera(NodePath new_camera);

    NodePath get_target();
    void     set_target(NodePath new_target);

    LVector2f get_angle();
    void      set_angle(LVector2f new_angle);

    float get_distance();
    void  get_distance(float new_distance);

    LVector2f get_sensitivity();
    void      set_sensitivity(LVector2f new_sensitivity);

    float get_min_angle_y();
    void  set_min_angle_y(float new_min);

    float get_max_angle_y();
    void  set_max_angle_y(float new_max);

    float get_smooth_fac();
    void  set_smooth_fac(float new_fac);

    float get_min_floor_distance();
    void  set_min_floor_distance(float new_fac);

    float get_target_z_offset();
    void  set_target_z_offset(float new_offset);

    ALLOC_DELETED_CHAIN(OrbitCamera);

  private:
    virtual DoneStatus do_task() override final;
    const char* const  cam_ray_name;
    InputController*   input;
    WindowFramework*   window;

    bool enabled;

    NodePath camera;
    NodePath target;

    LVector2f camera_angle;
    float     camera_distance;
    LVector2f camera_sensitivity;

    float min_angle_y;
    float max_angle_y;

    float smooth_fac;

    float min_floor_distance;
    float target_z_offset;

    NodePath                         collider_nodepath;
    CollisionTraverser               collision_traverser;
    PointerTo<CollisionRay>          collision_ray;
    PointerTo<CollisionNode>         collision_node;
    PointerTo<CollisionHandlerQueue> collision_queue;
};
