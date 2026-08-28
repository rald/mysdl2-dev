/*
* main.c - Showcase application utilizing SDL_GameController
*/

#define MYSDL2_IMPLEMENTATION
#include "mysdl2.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    MySDL app;

    if (!mysdl_init(&app, "MySDL2 GameController Showcase", 800, 600)) {
        printf("Failed to initialize MySDL2!\n");
        return 1;
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

        // Check SNES D-Pad / Left Stick via standardized GameController API
        bool move_up = mysdl_key_down(&app, SDL_SCANCODE_W) || mysdl_key_down(&app, SDL_SCANCODE_UP) || mysdl_gamepad_button_down(&app, SDL_CONTROLLER_BUTTON_DPAD_UP);
        bool move_down = mysdl_key_down(&app, SDL_SCANCODE_S) || mysdl_key_down(&app, SDL_SCANCODE_DOWN) || mysdl_gamepad_button_down(&app, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        bool move_left = mysdl_key_down(&app, SDL_SCANCODE_A) || mysdl_key_down(&app, SDL_SCANCODE_LEFT) || mysdl_gamepad_button_down(&app, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        bool move_right = mysdl_key_down(&app, SDL_SCANCODE_D) || mysdl_key_down(&app, SDL_SCANCODE_RIGHT) || mysdl_gamepad_button_down(&app, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

        if (move_up) player_y -= player_speed;
        if (move_down) player_y += player_speed;
        if (move_left) player_x -= player_speed;
        if (move_right) player_x += player_speed;

        // Action button (Space or SNES 'A' / 'B' button)
        bool action = mysdl_key_down(&app, SDL_SCANCODE_SPACE) || mysdl_gamepad_button_down(&app, SDL_CONTROLLER_BUTTON_A);

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

        mysdl_present(&app);
    }

    mysdl_quit(&app);
    return 0;
}
