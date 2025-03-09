#include <input_controller.h>

#include <cassert>
#include <cstring>

#include <buttonThrower.h>
#include <inputDevice.h>
#include <inputDeviceManager.h>
#include <inputDeviceNode.h>
#include <inputDeviceSet.h>
#include <lvector2.h>
#include <pandaFramework.h>
#include <windowFramework.h>

#include <config_loader.h>
#include <input_control.h>

InputController::InputController()
    : controller(false), keyboard_state { 0 }, gamepad_state { 0 },
      gamepad_invert_left_x(false), gamepad_invert_left_y(false),
      gamepad_invert_right_x(true), gamepad_invert_right_y(true)

{
  gamepad_device = nullptr;
  register_gamepads(this);
  register_keys(this);
}

InputController::~InputController() { }

bool InputController::is_gamepad_connected() { return gamepad_device == nullptr; }

bool InputController::is_pressed(InputControl ctl)
{
  assert(ctl < C_MAX);
  return gamepad_state[ctl] || keyboard_state[ctl];
}

bool InputController::is_gamepad_pressed(InputControl ctl)
{
  assert(ctl < C_MAX);
  return gamepad_state[ctl];
}

bool InputController::is_keyboard_pressed(InputControl ctl)
{
  assert(ctl < C_MAX);
  return keyboard_state[ctl];
}

LVector2f InputController::get_movement_vector()
{
  if (controller && gamepad_device != nullptr)
  {
    InputDevice::AxisState x = gamepad_device->find_axis(InputDevice::Axis::left_x);
    InputDevice::AxisState y = gamepad_device->find_axis(InputDevice::Axis::left_y);

    LVector2f vec(
        gamepad_invert_left_x ? -x.value : x.value,
        gamepad_invert_left_y ? -y.value : y.value
    );

    if (vec.length_squared() > 0.1)
    {
      return vec;
    }
  }
  else
  {
    float x = keyboard_state[C_RIGHT] ? 1.0f : 0.0f;
    float y = keyboard_state[C_FORWARD] ? 1.0f : 0.0f;
    if (keyboard_state[C_LEFT])
    {
      x -= 1.0f;
    }
    if (keyboard_state[C_BACKWARD])
    {
      y -= 1.0f;
    }
    LVector2f movement(x, y);
    movement.normalize();
    return movement;
  }
  return LVector2f::zero();
}
LVector2f InputController::get_camera_vector() const
{
  if (controller && gamepad_device != nullptr)
  {
    InputDevice::AxisState x = gamepad_device->find_axis(InputDevice::Axis::right_x);
    InputDevice::AxisState y = gamepad_device->find_axis(InputDevice::Axis::right_y);

    LVector2f vec(
        gamepad_invert_right_x ? -x.value : x.value,
        gamepad_invert_right_y ? -y.value : y.value
    );
    if (vec.length_squared() > 0.1)
    {
      return vec;
    }
  }
  else
  {
    float x = keyboard_state[C_CAM_RIGHT] ? 1.0f : 0.0f;
    float y = keyboard_state[C_CAM_UP] ? 1.0f : 0.0f;
    if (keyboard_state[C_CAM_LEFT])
    {
      x -= 1.0f;
    }
    if (keyboard_state[C_CAM_DOWN])
    {
      y -= 1.0f;
    }
    LVector2f movement(x, y);
    movement.normalize();
    return movement;
  }
  return LVector2f::zero();
}

void InputController::connect_first_device()
{
  InputDeviceManager* mgr     = InputDeviceManager::get_global_ptr();
  InputDeviceSet      devices = mgr->get_devices(InputDevice::DeviceClass::gamepad);

  if (devices.size())
  {
    connect_device(devices[0]);
  }
}

void InputController::connect_device(InputDevice* device)
{
  if (device->get_device_class() != InputDevice::DeviceClass::gamepad
      || gamepad_device != nullptr)
  {
    std::cout << "Ignoring device " << device->get_name() << std::endl;
    return;
  }
  std::cout << "Connecting device " << device->get_name() << std::endl;

  gamepad_device = device;
  InputDeviceNode* device_node =
      new InputDeviceNode(device, "InputController_gamepad");
  gamepad_device_nodepath =
      InputController::framework->get_data_root().attach_new_node(device_node);
  ButtonThrower* bt = new ButtonThrower("InputController_gamepad");
  gamepad_device_nodepath.attach_new_node(bt);
  bt->set_prefix("gamepad-");
}

void InputController::disconnect_device(InputDevice* device)
{
  if (device != gamepad_device)
  {
    std::cout << "Disconnected: inactive device " << device->get_name() << std::endl;
    return;
  }

  std::cout << "Disconnecting device " << device->get_name() << std::endl;
  gamepad_device_nodepath.remove_node();
  gamepad_device_nodepath.clear();
  gamepad_device = nullptr;

  connect_first_device();
}
