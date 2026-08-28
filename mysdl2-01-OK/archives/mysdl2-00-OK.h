/*
* mysdl2.h - Minimalist SDL2 Wrapper (STB-style single-header library)
* 
* Usage:
*   #define MYSDL2_IMPLEMENTATION
*   #include "mysdl2.h"
*/

#ifndef MYSDL2_H
#define MYSDL2_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool is_running;
    
    // Input states
    const Uint8* keyboard_state;
    int mouse_x, mouse_y;
    Uint32 mouse_buttons;
    SDL_Joystick* joysticks[4];
    Sint16 joy_axes[4][6];
    Uint8 joy_buttons[4][32];
} MySDL;

bool mysdl_init(MySDL* app, const char* title, int width, int height);
void mysdl_quit(MySDL* app);
bool mysdl_poll(MySDL* app);
void mysdl_clear(MySDL* app, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void mysdl_present(MySDL* app);

// Graphics Primitives
void mysdl_draw_pixel(MySDL* app, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void mysdl_draw_line(MySDL* app, int x0, int y0, int x1, int y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void mysdl_draw_rect(MySDL* app, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void mysdl_fill_rect(MySDL* app, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void mysdl_draw_circle(MySDL* app, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void mysdl_fill_circle(MySDL* app, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// Handlers / Input Queries
bool mysdl_key_down(MySDL* app, SDL_Scancode scancode);
bool mysdl_mouse_button_down(MySDL* app, int button);
void mysdl_get_mouse_pos(MySDL* app, int* x, int* y);
float mysdl_joystick_axis(MySDL* app, int joy_idx, int axis);
bool mysdl_joystick_button_down(MySDL* app, int joy_idx, int button);

#ifdef __cplusplus
}
#endif

#ifdef MYSDL2_IMPLEMENTATION

bool mysdl_init(MySDL* app, const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) return false;
    
    app->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!app->window) return false;
    
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!app->renderer) return false;
    
    app->is_running = true;
    app->keyboard_state = SDL_GetKeyboardState(NULL);
    app->mouse_x = 0;
    app->mouse_y = 0;
    app->mouse_buttons = 0;

    int num_joysticks = SDL_NumJoysticks();
    for (int i = 0; i < 4 && i < num_joysticks; i++) {
        app->joysticks[i] = SDL_JoystickOpen(i);
    }
    for (int i = num_joysticks; i < 4; i++) {
        app->joysticks[i] = NULL;
    }

    return true;
}

void mysdl_quit(MySDL* app) {
    for (int i = 0; i < 4; i++) {
        if (app->joysticks[i]) SDL_JoystickClose(app->joysticks[i]);
    }
    if (app->renderer) SDL_DestroyRenderer(app->renderer);
    if (app->window) SDL_DestroyWindow(app->window);
    SDL_Quit();
}

bool mysdl_poll(MySDL* app) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) app->is_running = false;
    }
    
    app->mouse_buttons = SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
    app->keyboard_state = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < 4; i++) {
        if (app->joysticks[i]) {
            for (int a = 0; a < 6; a++) {
                app->joy_axes[i][a] = SDL_JoystickGetAxis(app->joysticks[i], a);
            }
            for (int b = 0; b < 32; b++) {
                app->joy_buttons[i][b] = SDL_JoystickGetButton(app->joysticks[i], b);
            }
        }
    }

    return app->is_running;
}

void mysdl_clear(MySDL* app, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
    SDL_RenderClear(app->renderer);
}

void mysdl_present(MySDL* app) {
    SDL_RenderPresent(app->renderer);
}

void mysdl_draw_pixel(MySDL* app, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
    SDL_RenderDrawPoint(app->renderer, x, y);
}

void mysdl_draw_line(MySDL* app, int x0, int y0, int x1, int y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
    SDL_RenderDrawLine(app->renderer, x0, y0, x1, y1);
}

void mysdl_draw_rect(MySDL* app, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(app->renderer, &rect);
}

void mysdl_fill_rect(MySDL* app, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(app->renderer, &rect);
}

void mysdl_draw_circle(MySDL* app, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
    int dx = radius;
    int dy = 0;
    int err = 0;

    while (dx >= dy) {
        SDL_RenderDrawPoint(app->renderer, cx + dx, cy + dy);
        SDL_RenderDrawPoint(app->renderer, cx - dx, cy + dy);
        SDL_RenderDrawPoint(app->renderer, cx + dx, cy - dy);
        SDL_RenderDrawPoint(app->renderer, cx - dx, cy - dy);
        SDL_RenderDrawPoint(app->renderer, cx + dy, cy + dx);
        SDL_RenderDrawPoint(app->renderer, cx - dy, cy + dx);
        SDL_RenderDrawPoint(app->renderer, cx + dy, cy - dx);
        SDL_RenderDrawPoint(app->renderer, cx - dy, cy - dx);

        dy += 1;
        err += 1 + 2 * dy;
        if (2 * (err - dx) + 1 > 0) {
            dx -= 1;
            err += 1 - 2 * dx;
        }
    }
}

void mysdl_fill_circle(MySDL* app, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
    int dx = radius;
    int dy = 0;
    int err = 0;

    while (dx >= dy) {
        SDL_RenderDrawLine(app->renderer, cx - dx, cy + dy, cx + dx, cy + dy);
        SDL_RenderDrawLine(app->renderer, cx - dx, cy - dy, cx + dx, cy - dy);
        SDL_RenderDrawLine(app->renderer, cx - dy, cy + dx, cx + dy, cy + dx);
        SDL_RenderDrawLine(app->renderer, cx - dy, cy - dx, cx + dy, cy - dx);

        dy += 1;
        err += 1 + 2 * dy;
        if (2 * (err - dx) + 1 > 0) {
            dx -= 1;
            err += 1 - 2 * dx;
        }
    }
}

bool mysdl_key_down(MySDL* app, SDL_Scancode scancode) {
    return app->keyboard_state[scancode] ? true : false;
}

bool mysdl_mouse_button_down(MySDL* app, int button) {
    return (app->mouse_buttons & SDL_BUTTON(button)) != 0;
}

void mysdl_get_mouse_pos(MySDL* app, int* x, int* y) {
    if (x) *x = app->mouse_x;
    if (y) *y = app->mouse_y;
}

float mysdl_joystick_axis(MySDL* app, int joy_idx, int axis) {
    if (joy_idx < 0 || joy_idx >= 4 || !app->joysticks[joy_idx]) return 0.0f;
    Sint16 val = app->joy_axes[joy_idx][axis];
    return (float)val / 32767.0f;
}

bool mysdl_joystick_button_down(MySDL* app, int joy_idx, int button) {
    if (joy_idx < 0 || joy_idx >= 4 || !app->joysticks[joy_idx]) return false;
    return app->joy_buttons[joy_idx][button] ? true : false;
}

#endif // MYSDL2_IMPLEMENTATION
#endif // MYSDL2_H
