#pragma once

enum InputControl : unsigned int
{
  C_FOO = 0,
  C_LEFT,
  C_RIGHT,
  C_FORWARD,
  C_BACKWARD,
  C_JUMP,
  C_INTERACT,
  C_ATTACK,
  C_SNEAK,
  C_CAM_LEFT,
  C_CAM_RIGHT,
  C_CAM_UP,
  C_CAM_DOWN,
  C_MAX
};

extern const char* const InputControlNames[C_MAX];
const char* const        get_control_name(InputControl ctl);
InputControl             parse_control_name(const char* const);
