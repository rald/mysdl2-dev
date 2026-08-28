/*
* main.c - Showcase application for mysdl2.h
*/

#define MYSDL2_IMPLEMENTATION
#include "mysdl2.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    MySDL app;

    if (!mysdl_init(&app, "MySDL2 Showcase - SNES Pad Ready", 800, 600)) {
        printf("Failed to initialize MySDL2!\n");
        return 1;
    }

    printf("Initialization successful!\n");
    printf("Controls:\n");
    printf(" - ESC: Quit\n");
    printf(" - Arrow Keys / WASD or SNES D-Pad: Move player box\n");
    printf(" - Spacebar or SNES Button: Change box color\n");

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

        // --- Keyboard & SNES Inputs Combined ---
        float joy_x = mysdl_joystick_axis(&app, 0, 0); // Horizontal axis
        float joy_y = mysdl_joystick_axis(&app, 0, 1); // Vertical axis
        Uint8 hat_val = mysdl_joystick_hat(&app, 0, 0); // Hat switch (if D-Pad uses hat)

        // Movement from Keyboard or Joystick/D-Pad Axes
        if (mysdl_key_down(&app, SDL_SCANCODE_W) || mysdl_key_down(&app, SDL_SCANCODE_UP) || joy_y < -0.5f || (hat_val & SDL_HAT_UP)) {
            player_y -= player_speed;
        }
        if (mysdl_key_down(&app, SDL_SCANCODE_S) || mysdl_key_down(&app, SDL_SCANCODE_DOWN) || joy_y > 0.5f || (hat_val & SDL_HAT_DOWN)) {
            player_y += player_speed;
        }
        if (mysdl_key_down(&app, SDL_SCANCODE_A) || mysdl_key_down(&app, SDL_SCANCODE_LEFT) || joy_x < -0.5f || (hat_val & SDL_HAT_LEFT)) {
            player_x -= player_speed;
        }
        if (mysdl_key_down(&app, SDL_SCANCODE_D) || mysdl_key_down(&app, SDL_SCANCODE_RIGHT) || joy_x > 0.5f || (hat_val & SDL_HAT_RIGHT)) {
            player_x += player_speed;
        }

        // Check action buttons (Spacebar or Joystick button 0 / B button usually)
        bool action_triggered = mysdl_key_down(&app, SDL_SCANCODE_SPACE) || mysdl_joystick_button_down(&app, 0, 0);

        int mx, my;
        mysdl_get_mouse_pos(&app, &mx, &my);
        bool mouse_clicked = mysdl_mouse_button_down(&app, SDL_BUTTON_LEFT);

        bool hovering_rect = mysdl_inrect(mx, my, 100, 100, 200, 100);
        bool hovering_circle = mysdl_incircle(mx, my, custom_circle_x, custom_circle_y, custom_circle_r);

        mysdl_clear(&app, 30, 30, 40, 255);

        // Grid lines
        for (int x = 0; x < 800; x += 50) mysdl_draw_line(&app, x, 0, x, 600, 40, 40, 50, 255);
        for (int y = 0; y < 600; y += 50) mysdl_draw_line(&app, 0, y, 800, y, 40, 40, 50, 255);

        // Rectangles & Circles
        Uint8 rect_g = hovering_rect ? 200 : 100;
        if (hovering_rect && mouse_clicked) rect_g = 255;
        mysdl_fill_rect(&app, 100, 100, 200, 100, 50, rect_g, 150, 255);
        mysdl_draw_rect(&app, 100, 100, 200, 100, 255, 255, 255, 255);

        Uint8 circ_b = hovering_circle ? 255 : 150;
        if (hovering_circle && mouse_clicked) circ_b = 255;
        mysdl_fill_circle(&app, custom_circle_x, custom_circle_y, custom_circle_r, 200, 50, circ_b, 255);
        mysdl_draw_circle(&app, custom_circle_x, custom_circle_y, custom_circle_r, 255, 255, 255, 255);

        // Player Box
        Uint8 player_r = action_triggered ? 255 : 0;
        Uint8 player_b = action_triggered ? 0 : 255;
        mysdl_fill_rect(&app, (int)player_x, (int)player_y, 30, 30, player_r, 200, player_b, 255);
        mysdl_draw_rect(&app, (int)player_x, (int)player_y, 30, 30, 255, 255, 255, 255);

        // Mouse pointer indicator
        mysdl_draw_pixel(&app, mx, my, 255, 255, 0, 255);
        mysdl_draw_pixel(&app, mx + 1, my, 255, 255, 0, 255);
        mysdl_draw_pixel(&app, mx, my + 1, 255, 255, 0, 255);
        mysdl_draw_pixel(&app, mx + 1, my + 1, 255, 255, 0, 255);

        // Joystick visualizer display box
        int pad_base_x = 600;
        int pad_base_y = 100;
        mysdl_draw_rect(&app, pad_base_x - 50, pad_base_y - 50, 100, 100, 100, 100, 100, 255);
        int stick_draw_x = pad_base_x + (int)(joy_x * 40.0f);
        int stick_draw_y = pad_base_y + (int)(joy_y * 40.0f);
        mysdl_fill_circle(&app, stick_draw_x, stick_draw_y, 8, 255, 165, 0, 255);

        if (mysdl_joystick_button_down(&app, 0, 0)) {
            mysdl_fill_rect(&app, pad_base_x - 20, pad_base_y + 60, 40, 20, 0, 255, 0, 255);
        }

        mysdl_present(&app);
    }

    mysdl_quit(&app);
    return 0;
}
