/*
* main.c - Showcase application utilizing standardized SDL_GameController configuration layout with GUID printer
*/

#define MYSDL2_IMPLEMENTATION
#include "mysdl2.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    MySDL app;

    if (!mysdl_init(&app, "MySDL2 GameController Configuration Showcase", 800, 600)) {
        printf("Failed to initialize MySDL2!\n");
        return 1;
    }

    // Print the connected joystick GUID to help configure gamecontrollerdb.txt properly
    if (SDL_NumJoysticks() > 0) {
        char guid_str[33];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(0), guid_str, sizeof(guid_str));
        printf("Connected Joystick GUID: %s\n", guid_str);
    }

    int custom_circle_x = 400;
    int custom_circle_y = 300;
    int custom_circle_r = 40;

    float player_x = 400.0f;
    float player_y = 450.0f;
    float player_speed = 4.0f;

    while (mysdl_poll(&app)) {
        if (mysdl_key_down(&app, SDL_SCANCODE_ESCAPE)) {
            break;
        }

        // Standardized GameController inputs mapped via gamecontrollerdb.txt
        bool a_pressed      = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_A);
        bool b_pressed      = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_B);
        bool x_pressed      = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_X);
        bool y_pressed      = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_Y);
        bool l_pressed      = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        bool r_pressed      = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        bool select_pressed = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_BACK);
        bool start_pressed  = mysdl_controller_button_down(&app, SDL_CONTROLLER_BUTTON_START);

        if (a_pressed) {
            printf("A button pressed!\n");
        }

        if (b_pressed) {
            printf("B button pressed!\n");
        }

        if (x_pressed) {
            printf("X button pressed!\n");
        }

        if (y_pressed) {
            printf("Y button pressed!\n");
        }

        if (r_pressed) {
            printf("R button pressed!\n");
        }

        if (l_pressed) {
            printf("L button pressed!\n");
        }

        if (select_pressed) {
            printf("Select button pressed!\n");
        }

        if (start_pressed) {
            printf("Start button pressed!\n");
        }

        int xDir = mysdl_joystick_get_direction_x(&app, 0, 8000);
        int yDir = mysdl_joystick_get_direction_y(&app, 1, 8000);
        double joystickAngle = mysdl_joystick_angle(&app, 0, 1, 8000);

        Uint8 hat  = mysdl_joystick_hat(&app, 0);
        bool up    = mysdl_key_down(&app, SDL_SCANCODE_W) || mysdl_key_down(&app, SDL_SCANCODE_UP) || yDir < 0 || (hat & SDL_HAT_UP);
        bool down  = mysdl_key_down(&app, SDL_SCANCODE_S) || mysdl_key_down(&app, SDL_SCANCODE_DOWN) || yDir > 0 || (hat & SDL_HAT_DOWN);
        bool left  = mysdl_key_down(&app, SDL_SCANCODE_A) || mysdl_key_down(&app, SDL_SCANCODE_LEFT) || xDir < 0 || (hat & SDL_HAT_LEFT);
        bool right = mysdl_key_down(&app, SDL_SCANCODE_D) || mysdl_key_down(&app, SDL_SCANCODE_RIGHT) || xDir > 0 || (hat & SDL_HAT_RIGHT);

        if (up) player_y -= player_speed;
        if (down) player_y += player_speed;
        if (left) player_x -= player_speed;
        if (right) player_x += player_speed;

        bool action = mysdl_key_down(&app, SDL_SCANCODE_SPACE) || 
                      a_pressed ||
                      b_pressed ||
                      x_pressed ||
                      y_pressed ||
                      r_pressed ||
                      l_pressed ||
                      start_pressed || 
                      select_pressed;

        int mx, my;
        mysdl_get_mouse_pos(&app, &mx, &my);
        bool mouse_clicked = mysdl_mouse_button_down(&app, SDL_BUTTON_LEFT);

        bool hovering_rect = mysdl_inrect(mx, my, 100, 100, 200, 100);
        bool hovering_circle = mysdl_incircle(mx, my, custom_circle_x, custom_circle_y, custom_circle_r);

        mysdl_clear(&app, 30, 30, 40, 255);

        for (int x = 0; x < 800; x += 50) mysdl_draw_line(&app, x, 0, x, 600, 40, 40, 50, 255);
        for (int y = 0; y < 600; y += 50) mysdl_draw_line(&app, 0, y, 800, y, 40, 40, 50, 255);

        Uint8 rect_g = hovering_rect ? 200 : 100;
        if (hovering_rect && mouse_clicked) rect_g = 255;
        mysdl_fill_rect(&app, 100, 100, 200, 100, 50, rect_g, 150, 255);
        mysdl_draw_rect(&app, 100, 100, 200, 100, 255, 255, 255, 255);

        Uint8 circ_b = hovering_circle ? 255 : 150;
        if (hovering_circle && mouse_clicked) circ_b = 255;
        mysdl_fill_circle(&app, custom_circle_x, custom_circle_y, custom_circle_r, 200, 50, circ_b, 255);
        mysdl_draw_circle(&app, custom_circle_x, custom_circle_y, custom_circle_r, 255, 255, 255, 255);

        Uint8 player_r = action ? 255 : 0;
        Uint8 player_b = action ? 0 : 255;
        mysdl_fill_rect(&app, (int)player_x, (int)player_y, 30, 30, player_r, 200, player_b, 255);
        mysdl_draw_rect(&app, (int)player_x, (int)player_y, 30, 30, 255, 255, 255, 255);

        if (xDir != 0 || yDir != 0) {
            (void)joystickAngle;
        }

        mysdl_present(&app);
    }

    mysdl_quit(&app);
    return 0;
}
