#include <inputDeviceManager.h>
#include <inputDeviceSet.h>
#include <pandaFramework.h>

#include <config_loader.h>
#include <input_controller.h>

#include "input_controller_macros.h"

static InputController* input_instance = nullptr;

// Control mappings.
static ConfigStringMap gamepad_map  = config_file_read("gamepad.cfg");
static ConfigStringMap keyboard_map = config_file_read("keyboard.cfg");

PandaFramework* InputController::framework;

InputController* InputController::get_instance()
{
  assert(input_instance != nullptr);
  return input_instance;
}

InputController* InputController::init_instance(PandaFramework* f)
{
  assert(input_instance == nullptr);
  InputController::framework = f;
  input_instance             = new InputController();
  assert(input_instance != nullptr);
  return input_instance;
}

void InputController::destroy_instance()
{
  assert(input_instance != nullptr);
  delete input_instance;
  input_instance = nullptr;
}

void InputController::register_gamepads(InputController* controller)
{
  controller->connect_first_device();
  EventHandler& handler = framework->get_event_handler();
  handler
      .add_hook("connect-device", InputController::handle_connect_device, controller);
  handler.add_hook(
      "disconnect-device",
      InputController::handle_disconnect_device,
      controller
  );

  for (uint ctl = static_cast<uint>(C_FOO) + 1; ctl < static_cast<uint>(C_MAX); ++ctl)
  {
    register_gamepad_button(controller, static_cast<InputControl>(ctl));
  }
}

void InputController::register_gamepad_button(
    InputController* controller,
    InputControl     control
)
{
  EventHandler&     handler     = framework->get_event_handler();
  const std::string controlName = get_control_name(control);
  const std::string key_name    = gamepad_map[controlName];
  bool*             state       = controller->gamepad_state + control;

  if (key_name.empty() || controlName.empty() || state == nullptr)
  {
    return;
  }
  handler.add_hook(
      "gamepad-" + key_name,
      InputController::handle_gamepad_button_down,
      state
  );
  handler.add_hook(
      "gamepad-" + key_name + "-up",
      InputController::handle_gamepad_button_up,
      state
  );
}

void InputController::register_key(InputController* controller, InputControl control)
{
  const std::string controlName = get_control_name(control);
  const std::string key_name    = keyboard_map[controlName];
  bool*             state       = controller->keyboard_state + control;

  if (key_name.empty())
  {
    throw std::runtime_error("Missing control in key configuration file");
  }
  framework
      ->define_key(key_name, controlName, InputController::handle_key_down, state);
  framework->define_key(
      key_name + "-up",
      controlName + "-up",
      InputController::handle_key_up,
      state
  );
}

void InputController::register_keys(InputController* controller)
{

  framework->get_window(0)->enable_keyboard();
  for (uint ctl = static_cast<uint>(C_FOO) + 1; ctl < static_cast<uint>(C_MAX); ++ctl)
  {
    register_key(controller, static_cast<InputControl>(ctl));
  }
}

void InputController::handle_key_down(const Event* event, void* data)
{
  bool* boolean = static_cast<bool*>(data);
  *boolean      = true;
  input_log_keyboard("down", data);
  input_instance->controller = false;
}

void InputController::handle_key_up(const Event* event, void* data)
{
  bool* boolean = (bool*)data;
  *boolean      = false;

  input_log_keyboard("up", data);
  input_instance->controller = false;
}

void InputController::handle_gamepad_button_down(const Event* event, void* data)
{
  bool* boolean = static_cast<bool*>(data);
  *boolean      = true;

  input_log_gamepad("down", data);
  input_instance->controller = true;
}

void InputController::handle_gamepad_button_up(const Event* event, void* data)
{
  bool* boolean = (bool*)data;
  *boolean      = false;

  input_log_gamepad("up", data);
  input_instance->controller = true;
}

void InputController::handle_connect_device(const Event* event, void* data)
{
  static_cast<InputController*>(data)->connect_device(
      DCAST(InputDevice, event->get_parameter(0).get_typed_ref_count_value())
  );
}
void InputController::handle_disconnect_device(const Event* event, void* data)
{
  static_cast<InputController*>(data)->disconnect_device(
      DCAST(InputDevice, event->get_parameter(0).get_typed_ref_count_value())
  );
}

DBG_ONLY(InputControl InputController::debug_determine_key(void* data) {
  // This is a cursed way to figure out which key was pressed
  // when all we have to go o n is a pointer to the key's state
  // bool within the larger keyboard_state array
  std::uintptr_t addr_state = (std::uintptr_t)input_instance->keyboard_state;
  std::uintptr_t addr_data  = (std::uintptr_t)data;
  std::uintptr_t diff       = addr_data - addr_state;
  if (diff >= C_MAX)
  {
    return C_FOO;
  }
  return (InputControl)diff;
})

DBG_ONLY(InputControl InputController::debug_determine_button(void* data) {
  // This is a cursed way to figure out which key was pressed
  // when all we have to go o n is a pointer to the key's state
  // bool within the larger keyboard_state array
  std::uintptr_t addr_state = (std::uintptr_t)input_instance->gamepad_state;
  std::uintptr_t addr_data  = (std::uintptr_t)data;
  std::uintptr_t diff       = addr_data - addr_state;
  if (diff >= C_MAX)
  {
    return C_FOO;
  }
  return (InputControl)diff;
})
