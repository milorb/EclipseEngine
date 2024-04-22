#include "Input.hpp"

void Input::init() {
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; ++code) {
        states[SDL_Scancode(code)] = InputState_UP;
    }
}

bool Input::GetKey(std::string key) {
    SDL_Scancode keycode = string_to_scancode[key];
    if (states[keycode] == InputState_DOWN 
        || states[keycode] == InputState_PRESS) return true;
    return false;
}

bool Input::GetKeyDown(std::string key) {
    SDL_Scancode keycode = string_to_scancode[key];
    if (states[keycode] == InputState_PRESS) return true;
    return false;
}

bool Input::GetKeyUp(std::string key) {
    SDL_Scancode keycode = string_to_scancode[key];
    if (states[keycode] == InputState_LIFT) return true;
    return false;
}

glm::vec2 Input::GetMousePosition() {
    return mouse_position;
}

bool Input::GetMouseButton(int button) {
    if (mouse_button_states[button] == InputState_DOWN
        || mouse_button_states[button] == InputState_PRESS) return true;
    return false;
}

bool Input::GetMouseButtonDown(int button) {
    if (mouse_button_states[button] == InputState_PRESS) return true;
    return false;
}

bool Input::GetMouseButtonUp(int button) {
    if (mouse_button_states[button] == InputState_LIFT) return true;
    return false;
}

float Input::GetMouseScrollDelta() {
    return mouse_scroll;
}

void Input::process_event(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        pressed_this_frame
            .push_back(e.key.keysym.scancode);
        states[e.key.keysym.scancode] = InputState_PRESS;
    } else if (e.type == SDL_KEYUP) {
        lifted_this_frame
            .push_back(e.key.keysym.scancode);
        states[e.key.keysym.scancode] = InputState_LIFT;
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        clicked_this_frame
            .push_back(e.button.button);
        mouse_button_states[e.button.button] = InputState_PRESS;
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        declicked_this_frame
            .push_back(e.button.button);
        mouse_button_states[e.button.button] = InputState_LIFT;
    } else if (e.type == SDL_MOUSEMOTION) {
        mouse_position.x = e.motion.x;
        mouse_position.y = e.motion.y;
    } else if (e.type == SDL_MOUSEWHEEL) {
        mouse_scroll = e.wheel.preciseY;
    }
}

void Input::update_frame_end() {
    for (SDL_Scancode& sc : pressed_this_frame) {
        states[sc] = InputState_DOWN;
    }
    for (int i : clicked_this_frame) {
        mouse_button_states[i] = InputState_DOWN;
    }
    pressed_this_frame.clear();
    clicked_this_frame.clear();

    for (SDL_Scancode& sc: lifted_this_frame) {
        states[sc] = InputState_UP;
    }
    for (int i : declicked_this_frame) {
        mouse_button_states[i] = InputState_UP;
    }
    lifted_this_frame.clear();
    declicked_this_frame.clear();

    mouse_scroll = 0.0f;
}

