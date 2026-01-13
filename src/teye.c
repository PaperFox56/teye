#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>  // for usleep

#include "teye.h"

// ANSI escape codes for colors
#define ANSI_COLOR_RESET   "\x1b[0m"
#define clear_screen() printf("\033[H\033[J")
#define move_cursor_top_left() printf("\033[H")
#define hide_cursor() printf("\033[?25l")
#define show_cursor() printf("\033[?25h")

#define FORGROUND   "\033[38;2;"
#define BACKGROUND  "\033[48;2;"


char colors
///
static inline const char* get_color_from_number(int x) {
    switch (x) {
        case 0: return "255;255;255m";  // White
        case 1: return "175;175;175m";  // Light gray
        case 2: return "105;105;105m";  // Dark gray
        default: return "75;75;75m";    // Black
    }
}


TEYE_Buffer teye_instance;

// Cause I'm lazy
#define pixelcount(lcd) ((lcd).width * (lcd).height)

// Function to initialize LCD
TEYE_Buffer TEYE_init(ushort width, ushort height) {
    hide_cursor();
    setlocale(LC_ALL, "");
    //system("clear");
    clear_screen();

    // Allocate the buffer
    teye_instance.width = width;
    teye_instance.height = height;
    teye_instance.frame_buffer = malloc(sizeof(uint8_t*) * height);

    for (int i = 0; i < height; i++) {
        teye_instance.frame_buffer[i] = malloc(sizeof(uint8_t) * width);
    }

    TEYE_clear_buffer();

    return teye_instance;
}

void TEYE_clear_buffer() {
    for (int i = 0; i < teye_instance.height; i++) {
        memset(teye_instance.frame_buffer[i], 0, teye_instance.width);
    }
}

// Function to render the frame with optimized approach
void TEYE_render_frame() {
    // Clear the screen once, no need to do it in each loop
    move_cursor_top_left(); fflush(stdout); // flush printf buffer if needed

    // Buffer for printing
    size_t buf_size = pixelcount(teye_instance) * 35;
    char *string_buffer = malloc(buf_size); 
    int offset = 0;

    // Loop over the frame buffer, two rows at a time (since we render 2 rows per character)
    for (ushort i = 0; i < teye_instance.height-1; i += 2) {

        for (ushort j = 0; j < teye_instance.width; j++) {
            // Combine two rows to form one character
            uint8_t upper_half = teye_instance.frame_buffer[i][j];
            uint8_t lower_half = teye_instance.frame_buffer[i + 1][j];

            // Based on the character_value, pick the color
            if (upper_half == lower_half) {
                offset += snprintf(
                    &string_buffer[offset],
                    buf_size - offset,
                        ANSI_COLOR_RESET BACKGROUND "%s ",
                        get_color_from_number(upper_half)
                );
            } else {
                offset += snprintf(
                    &string_buffer[offset],
                    buf_size - offset,
                    FORGROUND "%s" BACKGROUND "%s▀",
                    get_color_from_number(upper_half), get_color_from_number(lower_half)
                );
            }
        }        
        
        // End line with color reset and newline
        offset += snprintf(&string_buffer[offset], buf_size - offset,
                           ANSI_COLOR_RESET "\n");

    }
    write(STDOUT_FILENO, string_buffer, offset);


    // Final reset to ensure terminal colors are clean
    write(STDOUT_FILENO, ANSI_COLOR_RESET, strlen(ANSI_COLOR_RESET));

    free(string_buffer);

}

/** Restore the terminal and clean the memory*/
void TEYE_free() {
    show_cursor();

    for (int i = 0; i < teye_instance.height; i++) {
        free(teye_instance.frame_buffer[i]);
    }
    free(teye_instance.frame_buffer);
}
