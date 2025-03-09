
#pragma once

#include <config.h>

#include <input_control.h>
#include <input_controller.h>

#ifdef GAME_DEBUG_BUILD
#  ifdef DEBUG_LOG_KEYBOARD
#    define input_log_keyboard(dir, data_ptr)                                        \
                                                                                     \
      InputControl _foo___c = InputController::debug_determine_key(data_ptr);        \
      std::cout << "Keyboard key " << dir << ": " << _foo___c << " - "               \
                << get_control_name(_foo___c) << std::endl;

#  else
#    define input_log_keyboard(dir, data_ptr)
#  endif
#  ifdef DEBUG_LOG_GAMEPAD
#    define input_log_gamepad(dir, data_ptr)                                         \
                                                                                     \
      InputControl _foo___c = InputController::debug_determine_button(data_ptr);     \
      std::cout << "Gamepad control " << dir << ": " << _foo___c << " - "            \
                << get_control_name(_foo___c) << std::endl;

#  else
#    define input_log_gamepad(dir, data_ptr)
#  endif

#else
#  define input_log_keyboard(dir, data_ptr)
#  define input_log_gamepad(dir, data_ptr)
#endif
