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

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h> // for terminal size
#include <unistd.h>    // for usleep

#include "char_buffer.h"
#include <teye/teye.h>

// ANSI escape codes for colors
#define ANSI_COLOR_RESET "\x1b[0m"
#define clear_screen() printf("\033[J")
#define move_cursor_top_left() printf("\033[H")
#define hide_cursor() printf("\033[?25l")
#define show_cursor() printf("\033[?25h")

#define FORGROUND "\033[38;5;"
#define BACKGROUND "\033[48;5;"

#define pixelcount(buffer) (buffer.width * buffer.height)

char colors[][100] = {
    "0m",   // void
    "231m", // White
    "188m", // Light gray
    "102m", // Dark gray
    "16m",  // Black
    "87m",
};

#define get_color_from_number(x) colors[x]

#define clamp(x, min, max) (x < min ? min : (x > max ? max : x))
#define min(a, b) (a < b ? a : b)

static TEYE_Buffer teye_instance;

// This buffer is swaped with the main one after each frame.
static uint8_t *back_frame_buffer;

// Stores the frame's bytes before sending to the terminal
static struct CharBuffer char_buffer;

// Function to initialize LCD
TEYE_Buffer TEYE_init(ushort width, ushort height) {
  hide_cursor();
  setlocale(LC_ALL, "");

  printf("\033[?1049h"); // Switch to alternate buffer

  // Allocate the buffer
  teye_instance.width = width;
  teye_instance.height = height;
  teye_instance.frame_buffer =
      malloc(sizeof(uint8_t *) * pixelcount(teye_instance));
  back_frame_buffer = malloc(sizeof(uint8_t *) * pixelcount(teye_instance));
  memset(back_frame_buffer, 0, pixelcount(teye_instance));

  TEYE_clear_buffer(0);
  if (CharBuffer_init(&char_buffer) < 0)
    panic("Couldn't initialize a character buffer");

  return teye_instance;
}

void TEYE_clear_buffer(uint8_t color) {
  memset(teye_instance.frame_buffer, color, pixelcount(teye_instance));
}

TEYE_Buffer TEYE_render_frame_mode_2() {

  // As we use double-buffering, we need to make sure that the characters are
  // not overwritten by the terminal changing size
  static ushort prev_rows = 0;
  static ushort prev_cols = 0;

  // get the screen size
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  ushort rows = w.ws_row, cols = w.ws_col;

  int size_changed = (rows == prev_rows && cols == prev_cols) ? 0 : 1;

  if (size_changed) {
    // the back buffer is now misleading, clean it
    memset(back_frame_buffer, 0, pixelcount(teye_instance));
  }

  prev_cols = cols;
  prev_rows = rows;
  

  move_cursor_top_left();
  fflush(stdout); // flush printf buffer if needed

  double screen_to_buffer = (double)teye_instance.width / (double)cols;
  // calculate the renderered image's size
  rows = min(rows * 2, teye_instance.height / screen_to_buffer);

  // Loop over the frame buffer, two rows at a time (since we render 2 rows per
  // character)
  for (ushort i = 0; i < rows - 1; i += 2) {

    for (ushort j = 0; j < cols; j++) {
      // get into the frame buffer's coordinate
      ushort x = clamp(j * screen_to_buffer, 0, teye_instance.width - 1),
             y1 = clamp(i * screen_to_buffer, 0, teye_instance.height - 1),
             y2 =
                 clamp((i + 1) * screen_to_buffer, 0, teye_instance.height - 1);

      // Combine two rows to form one character
      uint8_t upper_half = get_buffer_pixel(teye_instance, x, y1);
      uint8_t lower_half = get_buffer_pixel(teye_instance, x, y2);
      uint8_t prev_upper_half = back_frame_buffer[x + teye_instance.width * y1];
      uint8_t prev_lower_half = back_frame_buffer[x + teye_instance.width * y2];

      if (prev_upper_half == upper_half && prev_lower_half == lower_half && !size_changed) {
        // no change, move forward and print the next pixel
        char buf[32];
        snprintf(buf, sizeof(buf), "\x1b[%d;%dH", i / 2 + 1, j + 2);
        CharBuffer_append_text(&char_buffer, buf, strlen(buf));
        continue;
      }

      // Based on the character_value, pick the color
      if (upper_half == lower_half) {
        char *color = get_color_from_number(upper_half);
        CharBuffer_append_text(&char_buffer, ANSI_COLOR_RESET BACKGROUND,
                               strlen(ANSI_COLOR_RESET BACKGROUND));
        CharBuffer_append_text(&char_buffer, color, strlen(color));
        CharBuffer_append_text(&char_buffer, " ", 1);
      } else {
        char *color1 = get_color_from_number(upper_half);
        char *color2 = get_color_from_number(lower_half);
        CharBuffer_append_text(&char_buffer, FORGROUND, strlen(FORGROUND));
        CharBuffer_append_text(&char_buffer, color1, strlen(color1));
        CharBuffer_append_text(&char_buffer, BACKGROUND, strlen(BACKGROUND));
        CharBuffer_append_text(&char_buffer, color2, strlen(color2));
        CharBuffer_append_text(&char_buffer, "▀", 4);
      }
    }

    // End line with color reset and newline
    CharBuffer_append_text(&char_buffer, ANSI_COLOR_RESET "\n",
                           strlen(ANSI_COLOR_RESET) + 1);
  }
  write(STDOUT_FILENO, char_buffer.buf, char_buffer.len);

  clear_screen();

  // Swaps the back and front buffers
  uint8_t *temp = back_frame_buffer;
  back_frame_buffer = teye_instance.frame_buffer;
  teye_instance.frame_buffer = temp;

  // Cleans the haracter buffer to make it usable for the next iteration
  char_buffer.len = 0;
  char_buffer.buf[0] = '\0';

  return teye_instance;
}

// Function to render the frame buffer, represent two pixels with a single
// character
/*void TEYE_render_frame_mode_1() {
  move_cursor_top_left();
  fflush(stdout); // flush printf buffer if needed

  // Buffer for printing
  size_t buf_size = pixelcount(teye_instance) * 35;
  char *string_buffer = malloc(buf_size);
  int offset = 0;

  // Loop over the frame buffer, two rows at a time (since we render 2 rows per
  // character)
  for (ushort i = 0; i < teye_instance.height - 1; i += 2) {

    for (ushort j = 0; j < teye_instance.width; j++) {
      // Combine two rows to form one character
      uint8_t upper_half = teye_instance.frame_buffer[i][j];
      uint8_t lower_half = teye_instance.frame_buffer[i + 1][j];

      // Based on the character_value, pick the color
      if (upper_half == lower_half) {
        offset += snprintf(&string_buffer[offset], buf_size - offset,
                           ANSI_COLOR_RESET BACKGROUND "%s ",
                           get_color_from_number(upper_half));
      } else {
        offset += snprintf(&string_buffer[offset], buf_size - offset,
                           FORGROUND "%s" BACKGROUND "%s▀",
                           get_color_from_number(upper_half),
                           get_color_from_number(lower_half));
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

/** Restore the terminal and clean the memory*/
void TEYE_free() {
  show_cursor();

  free(teye_instance.frame_buffer);
  free(back_frame_buffer);

  CharBuffer_free(&char_buffer);
  printf("\033[?1049l"); // Return to primary buffer
}

void panic(const char *s) {

  TEYE_free();

  perror(s);
  exit(1);
}
