#include <orbit_camera.h>

#include <algorithm>

#include <asyncTask.h>
#include <cmath>
#include <collisionHandlerQueue.h>
#include <deg_2_rad.h>
#include <lvector3.h>
#include <nodePath.h>

#include <input_control.h>
#include <input_controller.h>

OrbitCamera::OrbitCamera(
    WindowFramework*  window_framework,
    const char* const ray_name,
    NodePath          camera_node,
    NodePath          target_node
)
    : cam_ray_name(ray_name), window(window_framework), camera(camera_node),
      target(target_node), camera_angle(0.0f, 45.0f), camera_distance(7.0f),
      camera_sensitivity(160.0f, 100.0f), enabled(true), min_angle_y(-10),
      max_angle_y(50), smooth_fac(0.1f), min_floor_distance(0.25f),
      target_z_offset(1.2f), input(InputController::get_instance())
{
  // time to setup colliders

  // We create a collision ray pointing downward and attach it to the camera node
  // Later, in move_and_collide, we will detect the z value (height) where the ray
  // meets the terrain to ensure the camera does not go below that point

  collision_ray = new CollisionRay();
  assert(collision_ray != nullptr);
  collision_ray->set_origin(0, 0, 1000);
  collision_ray->set_direction(0, 0, -1);
  collision_node = new CollisionNode(cam_ray_name);
  assert(collision_node != nullptr);
  collision_node->add_solid(collision_ray);
  collision_node->set_from_collide_mask(BitMask32::bit(0));
  collision_node->set_into_collide_mask(BitMask32::all_off());

  collider_nodepath = camera.attach_new_node(collision_node);

  collision_queue = new CollisionHandlerQueue();
  assert(collision_queue != nullptr);
  collision_traverser.add_collider(collider_nodepath, collision_queue);
}

OrbitCamera::OrbitCamera(
    WindowFramework*  window_framework,
    const char* const ray_name,
    NodePath*         camera_node,
    NodePath          target_node
)
    : OrbitCamera(window_framework, ray_name, *camera_node, target_node)
{
  ; // this is just a convenience
}

OrbitCamera::OrbitCamera(
    WindowFramework*  window_framework,
    const char* const ray_name,
    NodePath          camera_node,
    NodePath*         target_node
)
    : OrbitCamera(window_framework, ray_name, camera_node, *target_node)
{
  ; // this is just a convenience
}

OrbitCamera::OrbitCamera(
    WindowFramework*  window_framework,
    const char* const ray_name,
    NodePath*         camera_node,
    NodePath*         target_node
)
    : OrbitCamera(window_framework, ray_name, *camera_node, *target_node)
{
  ; // this is just a convenience
}

bool OrbitCamera::is_enabled() { return enabled; }
void OrbitCamera::enable() { enabled = true; }
void OrbitCamera::disable() { enabled = false; }
void OrbitCamera::toggle_enabled() { enabled = !enabled; }

NodePath OrbitCamera::get_camera() { return NodePath(camera); }
void     OrbitCamera::set_camera(NodePath new_camera)
{
  collider_nodepath.remove_node();
  camera = new_camera;
  if (!camera.is_empty())
  {
    collider_nodepath.reparent_to(camera);
  }
}

NodePath OrbitCamera::get_target() { return NodePath(target); }
void     OrbitCamera::set_target(NodePath new_target) { target = new_target; }

LVector2f OrbitCamera::get_angle() { return camera_angle; }
void      OrbitCamera::set_angle(LVector2f new_angle) { camera_angle = new_angle; }

float OrbitCamera::get_distance() { return camera_distance; }
void OrbitCamera::get_distance(float new_distance) { camera_distance = new_distance; }

LVector2f OrbitCamera::get_sensitivity() { return camera_sensitivity; }
void      OrbitCamera::set_sensitivity(LVector2f new_sensitivity)
{
  camera_sensitivity = new_sensitivity;
}

float OrbitCamera::get_min_angle_y() { return min_angle_y; }
void  OrbitCamera::set_min_angle_y(float new_min) { min_angle_y = new_min; }

float OrbitCamera::get_max_angle_y() { return max_angle_y; }
void  OrbitCamera::set_max_angle_y(float new_max) { max_angle_y = new_max; }

float OrbitCamera::get_smooth_fac() { return smooth_fac; }
void  OrbitCamera::set_smooth_fac(float new_fac) { smooth_fac = new_fac; }

float OrbitCamera::get_min_floor_distance() { return min_floor_distance; }
void  OrbitCamera::set_min_floor_distance(float new_fac)
{
  min_floor_distance = new_fac;
};

float OrbitCamera::get_target_z_offset() { return target_z_offset; }
void  OrbitCamera::set_target_z_offset(float new_offset)
{
  target_z_offset = new_offset;
}

AsyncTask::DoneStatus OrbitCamera::do_task()
{
  if (!enabled || camera.is_empty() || target.is_empty())
  {
    return AsyncTask::DS_cont;
  }

  LVector3f target_pos  = target.get_pos();
  LVector3f start_pos   = camera.get_pos();
  double    delta_time  = ClockObject::get_global_clock()->get_dt();
  LVector2f delta_angle = input->get_camera_vector() * delta_time;

  // First we update the angle property based on the input
  delta_angle.componentwise_mult(camera_sensitivity);
  camera_angle += delta_angle;

  // Clamp vertical angle to avoid issues
  camera_angle.set_y(std::clamp(camera_angle.get_y(), min_angle_y, max_angle_y));

  // Next we calculate the ideal offset for the camera based on
  // the angle and distance.
  // This is done in radians so we gotta convert first.
  float heading_radians = deg_2_rad(camera_angle.get_x());
  float pitch_radians   = deg_2_rad(camera_angle.get_y());

  LVector3f intended_pos(
      camera_distance * cos(heading_radians) * cos(pitch_radians),
      camera_distance * sin(heading_radians) * cos(pitch_radians),
      camera_distance * sin(pitch_radians)
  );

  // Apply that offset to the target's position
  intended_pos += target_pos;

  // Now interpolate the intended camera position a little
  // to give it some smoothness
  intended_pos -= start_pos;
  intended_pos *= std::min<float>(1.0f, smooth_fac * intended_pos.length_squared());
  intended_pos += start_pos;

  // Set the position of the camera node so we can check the ground height
  camera.set_pos(intended_pos);

  // Collide with the ground
  collision_traverser.traverse(window->get_render());
  collision_queue->sort_entries();
  if (collision_queue->get_num_entries() > 0
      && collision_queue->get_entry(0)->get_into_node()->get_name() == "terrain")
  {
    // Now we determine the floor height, and add an offset so we're not too close
    // If the cam is below that point, all we gotta do is raise it
    float floorz =
        collision_queue->get_entry(0)->get_surface_point(window->get_render()).get_z()
        + min_floor_distance;
    if (camera.get_z() < floorz)
    {
      camera.set_z(floorz);
    }
  }

  // TODO: detect for objects between camera & target and handle that scenerio

  // Finally, face the player
  camera.look_at(LPoint3f(
      target_pos.get_x(),
      target_pos.get_y(),
      (target_pos.get_z() + target_z_offset)
  ));

  return AsyncTask::DS_cont;
}
