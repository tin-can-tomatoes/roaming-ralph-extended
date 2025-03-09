#include <input_control.h>

#include <cassert>
#include <cctype>
#include <cstring>

const char* const InputControlNames[C_MAX] {
  "Foo!",   "Left",  "Right",   "Forward",  "Backward", "Jump",   "Interact",
  "Attack", "Sneak", "CamLeft", "CamRight", "CamUp",    "CamDown"
};

const char* const get_control_name(InputControl ctl)
{
  assert(ctl < C_MAX);
  assert(ctl > C_FOO);
  return InputControlNames[ctl];
}

InputControl parse_control_name(const char* const control)
{
  for (unsigned int i = 0; i < C_MAX; ++i)
  {
    if (strcmp(control, InputControlNames[i]) == 0)
    {
      return static_cast<InputControl>(i);
    }
  }
  return C_FOO;
}
