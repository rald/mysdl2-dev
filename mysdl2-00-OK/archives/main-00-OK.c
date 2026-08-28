/*
* main.c - Showcase application for mysdl2.h
* 
* Compile instructions (Linux/GCC):
*   gcc main.c -o main -lSDL2 -lm
* Compile instructions (Windows/MinGW):
*   gcc main.c -o main.exe -lmingw32 -lSDL2main -lSDL2 -lm
*/

#define MYSDL2_IMPLEMENTATION
#include "mysdl2.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    MySDL app;

    if (!mysdl_init(&app, "MySDL2 Showcase", 800, 600)) {
        printf("Failed to initialize MySDL2!\n");
        return 1;
    }

    printf("Initialization successful!\n");
    printf("Controls:\n");
    printf(" - ESC: Quit\n");
    printf(" - Space: Draw a random filled circle\n");
    printf(" - Mouse Click: Check hit testing on shapes\n");
    printf(" - Joystick: Move axes or press buttons to test gamepad integration\n");

    int custom_circle_x = 400;
    int custom_circle_y = 300;
    int custom_circle_r = 40;

    while (mysdl_poll(&app)) {
        // Handle quick exit via ESC
        if (mysdl_key_down(&app, SDL_SCANCODE_ESCAPE)) {
            break;
        }

        // Get current mouse state
        int mx, my;
        mysdl_get_mouse_pos(&app, &mx, &my);
        bool mouse_clicked = mysdl_mouse_button_down(&app, SDL_BUTTON_LEFT);

        // Check hit testing with our shapes
        bool hovering_rect = mysdl_inrect(mx, my, 100, 100, 200, 100);
        bool hovering_circle = mysdl_incircle(mx, my, custom_circle_x, custom_circle_y, custom_circle_r);

        // Clear screen to dark slate gray
        mysdl_clear(&app, 30, 30, 40, 255);

        // --- 1. Draw Graphics Primitives ---

        // Grid lines
        for (int x = 0; x < 800; x += 50) {
            mysdl_draw_line(&app, x, 0, x, 600, 40, 40, 50, 255);
        }
        for (int y = 0; y < 600; y += 50) {
            mysdl_draw_line(&app, 0, y, 800, y, 40, 40, 50, 255);
        }

        // Rectangles (Filled changes color on hover/click)
        Uint8 rect_g = hovering_rect ? 200 : 100;
        if (hovering_rect && mouse_clicked) rect_g = 255;
        mysdl_fill_rect(&app, 100, 100, 200, 100, 50, rect_g, 150, 255);
        mysdl_draw_rect(&app, 100, 100, 200, 100, 255, 255, 255, 255);

        // Circles (Filled changes color on hover/click)
        Uint8 circ_b = hovering_circle ? 255 : 150;
        if (hovering_circle && mouse_clicked) circ_b = 255;
        mysdl_fill_circle(&app, custom_circle_x, custom_circle_y, custom_circle_r, 200, 50, circ_b, 255);
        mysdl_draw_circle(&app, custom_circle_x, custom_circle_y, custom_circle_r, 255, 255, 255, 255);

        // Single pixel accents
        mysdl_draw_pixel(&app, mx, my, 255, 255, 0, 255);
        mysdl_draw_pixel(&app, mx + 1, my, 255, 255, 0, 255);
        mysdl_draw_pixel(&app, mx, my + 1, 255, 255, 0, 255);
        mysdl_draw_pixel(&app, mx + 1, my + 1, 255, 255, 0, 255);

        // --- 2. Joystick Integration Visualizer ---
        // Display state for Joystick 0 if available
        float joy_x = mysdl_joystick_axis(&app, 0, 0); // Left stick X
        float joy_y = mysdl_joystick_axis(&app, 0, 1); // Left stick Y
        int pad_base_x = 600;
        int pad_base_y = 100;

        // Draw joystick bounding box
        mysdl_draw_rect(&app, pad_base_x - 50, pad_base_y - 50, 100, 100, 100, 100, 100, 255);
        
        // Draw joystick coordinate point based on analog input
        int stick_draw_x = pad_base_x + (int)(joy_x * 40.0f);
        int stick_draw_y = pad_base_y + (int)(joy_y * 40.0f);
        mysdl_fill_circle(&app, stick_draw_x, stick_draw_y, 8, 255, 165, 0, 255);

        // If joystick button 0 is pressed, flash a visual indicator
        if (mysdl_joystick_button_down(&app, 0, 0)) {
            mysdl_fill_rect(&app, pad_base_x - 20, pad_base_y + 60, 40, 20, 0, 255, 0, 255);
        }

        // Present frame buffer to screen
        mysdl_present(&app);
    }

    mysdl_quit(&app);
    return 0;
}
