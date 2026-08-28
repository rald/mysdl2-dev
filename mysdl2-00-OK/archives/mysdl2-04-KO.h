/*
* mysdl2.h - Minimalist SDL2 GameController Wrapper (STB-style single-header library)
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
    SDL_GameController* controller;
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

// Collision Helpers
bool mysdl_inrect(int x, int y, int rx, int ry, int rw, int rh);
bool mysdl_incircle(int x, int y, int cx, int cy, int radius);

// Handlers / Input Queries
bool mysdl_key_down(MySDL* app, SDL_Scancode scancode);
bool mysdl_mouse_button_down(MySDL* app, int button);
void mysdl_get_mouse_pos(MySDL* app, int* x, int* y);
bool mysdl_gamepad_button_down(MySDL* app, SDL_GameControllerButton button);
float mysdl_gamepad_axis(MySDL* app, SDL_GameControllerAxis axis);

#ifdef __cplusplus
}
#endif

#ifdef MYSDL2_IMPLEMENTATION

#include <math.h>
#include <stdio.h>

bool mysdl_init(MySDL* app, const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }
    
    app->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!app->window) return false;
    
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!app->renderer) return false;
    
    app->is_running = true;
    app->keyboard_state = SDL_GetKeyboardState(NULL);
    app->mouse_x = 0;
    app->mouse_y = 0;
    app->mouse_buttons = 0;
    app->controller = NULL;

    // Open the first available game controller
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            app->controller = SDL_GameControllerOpen(i);
            if (app->controller) {
                printf("Successfully opened Gamepad: %s\n", SDL_GameControllerName(app->controller));
                break;
            }
        }
    }

    if (!app->controller) {
        printf("Warning: No compatible GameController found. (Check permissions or connection)\n");
    }

    return true;
}

void mysdl_quit(MySDL* app) {
    if (app->controller) SDL_GameControllerClose(app->controller);
    if (app->renderer) SDL_DestroyRenderer(app->renderer);
    if (app->window) SDL_DestroyWindow(app->window);
    SDL_Quit();
}

bool mysdl_poll(MySDL* app) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) app->is_running = false;
        
        // Auto-handle controller hotplugging
        if (e.type == SDL_CONTROLLERDEVICEADDED) {
            if (!app->controller) {
                app->controller = SDL_GameControllerOpen(e.cdevice.which);
                printf("Controller connected dynamically.\n");
            }
        }
        if (e.type == SDL_CONTROLLERDEVICEREMOVED) {
            if (app->controller && e.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(app->controller))) {
                SDL_GameControllerClose(app->controller);
                app->controller = NULL;
                printf("Controller disconnected.\n");
            }
        }
    }
    
    app->mouse_buttons = SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
    app->keyboard_state = SDL_GetKeyboardState(NULL);

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

bool mysdl_inrect(int x, int y, int rx, int ry, int rw, int rh) {
    return (x >= rx && x <= (rx + rw) && y >= ry && y <= (ry + rh));
}

bool mysdl_incircle(int x, int y, int cx, int cy, int radius) {
    int dx = x - cx;
    int dy = y - cy;
    return (dx * dx + dy * dy) <= (radius * radius);
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

bool mysdl_gamepad_button_down(MySDL* app, SDL_GameControllerButton button) {
    if (!app->controller) return false;
    return SDL_GameControllerGetButton(app->controller, button) ? true : false;
}

float mysdl_gamepad_axis(MySDL* app, SDL_GameControllerAxis axis) {
    if (!app->controller) return 0.0f;
    Sint16 val = SDL_GameControllerGetAxis(app->controller, axis);
    return (float)val / 32767.0f;
}

#endif // MYSDL2_IMPLEMENTATION
#endif // MYSDL2_H
