/**
 * teye.c

 MIT License

Copyright (c) 2026 PaperFox56

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

#include <stdlib.h>
#include <string.h>
// #include <locale.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> // for usleep
// #include <sys/ioctl.h>  // for terminal size
#include <stdio.h>

#include <ncurses.h>
#include <teye/teye.h>

// ANSI escape codes for colors
#define ANSI_COLOR_RESET "\x1b[0m"
#define clear_screen() printf("\033[H\033[J")
#define move_cursor_top_left() printf("\033[H")
#define hide_cursor() printf("\033[?25l")
#define show_cursor() printf("\033[?25h")

#define FORGROUND "\x1b[38;2;"
#define BACKGROUND "\x1b[48;2;"

char colors[][15] = {
    "255;255;255;0m", // void
    "255;255;255m",   // White
    "175;175;175m",   // Light gray
    "105;105;105m",   // Dark gray
    "75;75;75m",      // Black
};

#define get_color_from_number(x) colors[x]

#define clamp(x, min, max) (x < min ? min : (x > max ? max : x))
#define min(a, b) (a < b ? a : b)

static TEYE_Buffer teye_instance = {0, 0, NULL};

#define pixelcount(lcd) ((lcd).width * (lcd).height)

// Initialize a buffer for drawing
TEYE_Buffer TEYE_init(ushort width, ushort height) {
  // hide_cursor();
  // setlocale(LC_ALL, "");
  // system("clear");
  // clear_screen();

  // Don't initialize the buffer twice !
  if (teye_instance.frame_buffer != NULL) {
    return teye_instance;
  }

  initscr();
  raw();                // Disable line buffering (get keys immediately)
  keypad(stdscr, TRUE); // Enable arrow keys/F-keys
  noecho();             // Don't print typed characters to the screen

  // Allocate the buffer
  teye_instance.width = width;
  teye_instance.height = height;
  teye_instance.frame_buffer = malloc(sizeof(uint8_t *) * height);

  for (int i = 0; i < height; i++) {
    teye_instance.frame_buffer[i] = malloc(sizeof(uint8_t) * width);
  }

  TEYE_clear_buffer(0);
  // if (CharBuffer_init(&char_buffer) < 0)
  //     panic("Couldn't initialize a framebuffer");

  return teye_instance;
}

void TEYE_clear_buffer(uint8_t color) {
  for (int i = 0; i < teye_instance.height; i++) {
    memset(teye_instance.frame_buffer[i], color, teye_instance.width);
  }
}

// Function to render the frame buffer, try to fit the buffer's size to the
// screen's
void TEYE_render_frame_mode_2() {
  // get the screen size
  ushort rows, cols;
  getmaxyx(stdscr, rows, cols);

  // clear_screen(); fflush(stdout); // flush printf buffer if needed

  double screen_to_buffer = (double)teye_instance.width / (double)cols;
  // calculate the renderered image's size
  rows = min(rows * 2, teye_instance.height / screen_to_buffer);

  // Loop over the frame buffer, two rows at a time (since we render 2 rows per
  // character)
  for (ushort i = 0; i < rows - 1; i += 2) {

    move(i, 0);

    for (ushort j = 0; j < cols; j++) {
      // get into the frame buffer's coordinate
      ushort x = clamp(j * screen_to_buffer, 0, teye_instance.width - 1),
             y1 = clamp(i * screen_to_buffer, 0, teye_instance.height - 1),
             y2 =
                 clamp((i + 1) * screen_to_buffer, 0, teye_instance.height - 1);

      // Combine two rows to form one character
      uint8_t upper_half = teye_instance.frame_buffer[y1][x];
      uint8_t lower_half = teye_instance.frame_buffer[y2][x];

      // Based on the character_value, pick the color
      if (upper_half == lower_half) {
        char *color = get_color_from_number(upper_half);
        printw(BACKGROUND "%s ", color);
      } else {
        char *color1 = get_color_from_number(upper_half);
        char *color2 = get_color_from_number(lower_half);
        printw(FORGROUND "%s" BACKGROUND "%s▀\n", color1, color2);
      }
    }
  }

  refresh();
  getch();

}

/* Function to render the frame buffer, represent two pixels with a single
character void TEYE_render_frame_mode_1() { move_cursor_top_left();
fflush(stdout); // flush printf buffer if needed

    // Buffer for printing
    size_t buf_size = pixelcount(teye_instance) * 35;
    char *string_buffer = malloc(buf_size);
    int offset = 0;

    // Loop over the frame buffer, two rows at a time (since we render 2 rows
per character) for (ushort i = 0; i < teye_instance.height-1; i += 2) {

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
                    get_color_from_number(upper_half),
get_color_from_number(lower_half)
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

}*/

/** Restore the terminal and clean the buffer*/
void TEYE_free() {
  for (int i = 0; i < teye_instance.height; i++) {
    free(teye_instance.frame_buffer[i]);
  }
  free(teye_instance.frame_buffer);

  teye_instance.frame_buffer = NULL;

  endwin();
}

void panic(const char *s) {

  perror(s);
  exit(1);
}
