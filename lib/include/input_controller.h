#pragma once

#include <config.h>

#include <inputDevice.h>
#include <lvector2.h>
#include <pandaFramework.h>
#include <windowFramework.h>

#include <input_control.h>

// The InputController is a singleton class that abstracts most
// of the input management away from the rest of the game.
// It handles key and gamepad button mapping and holds the state
// of the controls.
class InputController
{
  public:
    // Get the singleton instance of InputController
    static InputController* get_instance();
    static InputController* init_instance(PandaFramework* f);
    static void             destroy_instance();

    bool is_gamepad_connected();

    bool is_pressed(InputControl ctl);
    bool is_keyboard_pressed(InputControl ctl);
    bool is_gamepad_pressed(InputControl ctl);

    LVector2f get_movement_vector();
    LVector2f get_camera_vector() const;

  private:
    InputController();
    ~InputController();

    void connect_first_device();
    void connect_device(InputDevice* device);
    void disconnect_device(InputDevice* device);

    static PandaFramework* framework;

    static void register_gamepads(InputController* controller);
    static void
    register_gamepad_button(InputController* controller, InputControl control);
    static void register_key(InputController* controller, InputControl control);
    static void register_keys(InputController* controller);

    static void handle_key_down(const Event* event, void* data);
    static void handle_key_up(const Event* event, void* data);

    static void handle_connect_device(const Event* event, void* data);
    static void handle_disconnect_device(const Event* event, void* data);

    static void handle_gamepad_button_down(const Event* event, void* data);
    static void handle_gamepad_button_up(const Event* event, void* data);

    DBG_ONLY(
        // A utility function to determine which control was
        // pressed on the keyboard from inside one of the
        // handlers where all we have is a pointer to a bool
        // in the keyboard state array. Then we can log it easier
        static InputControl debug_determine_key(void* data);

        // Same as debug_determine_key but for the gamepad state
        static InputControl debug_determine_button(void* data);
    )

    InputDevice* gamepad_device;
    NodePath     gamepad_device_nodepath;

    bool gamepad_invert_left_x;
    bool gamepad_invert_left_y;
    bool gamepad_invert_right_x;
    bool gamepad_invert_right_y;

    // was the last input received from the controller?
    bool controller;

    // State of the controls
    bool keyboard_state[C_MAX];
    bool gamepad_state[C_MAX];
};
