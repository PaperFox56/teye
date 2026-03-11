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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h> // for terminal size

#include "char_buffer.h"
#include <teye/teye.h>
#include <unistd.h>

/****************
 macros
 ****************/

// ANSI escape codes for colors
#define ANSI_COLOR_RESET "\x1b[0m"
#define clear_screen() printf("\x1b[J")
#define move_cursor_top_left() printf("\x1b[H")
#define hide_cursor() printf("\x1b[?25l")
#define show_cursor() printf("\x1b[?25h")

#define FORGROUND "\x1b[38;5;"
#define BACKGROUND "\x1b[48;5;"

#define get_color_from_number(x) colors[x]

#define clamp(x, min, max) (x < min ? min : (x > max ? max : x))

/****************
 Global variables
 ****************/
static char colors[][32] = {
    "0m",   // void
    "231m", // White
    "188m", // Light gray
    "102m", // Dark gray
    "16m",  // Black
    "87m",
};

// tracks whether the terminal size changed since the last frame
static volatile sig_atomic_t screen_resized = 0;

static TEYE_Buffer front_framebuffer;

// This buffer is swaped with the main one after each frame.
static TEYE_Buffer back_framebuffer;

// Stores the frame's bytes before sending to the terminal
static struct CharBuffer char_buffer;

/****************
 Functions
*****************/

static void signalHandler() {
  screen_resized = 1;

  char buf[32] = "screen resized\n";
  write(STDERR_FILENO, buf, strlen(buf));
}

/**
 * Fast integer to string conversion for terminal coordinates.
 */
static void CharBuffer_append_int(struct CharBuffer *cb, int n) {
  if (n == 0) {
    CharBuffer_append_text(cb, "0", 1);
    return;
  }

  char tmp[12]; // Enough for a 32-bit int
  int i = 10;
  tmp[11] = '\0';

  while (n > 0 && i >= 0) {
    tmp[i--] = (n % 10) + '0';
    n /= 10;
  }

  // i+1 is the start of our string
  CharBuffer_append_text(cb, &tmp[i + 1], 10 - i);
}

/**
 * Fast Cursor Move: "\x1b[row;colH"
 */
static void CharBuffer_append_cursor_move(struct CharBuffer *cb, int row,
                                          int col) {
  CharBuffer_append_text(cb, "\x1b[", 2);
  CharBuffer_append_int(cb, row);
  CharBuffer_append_text(cb, ";", 1);
  CharBuffer_append_int(cb, col);
  CharBuffer_append_text(cb, "H", 1);
}

static void reset_frame_buffers() {

  // get the screen size
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  int rows = w.ws_row * 2, cols = w.ws_col;

  // Allocate the framebuffers
  if (TEYE_allocate_buffer(&front_framebuffer, cols, rows) < 0 ||
      TEYE_allocate_buffer(&back_framebuffer, cols, rows) < 0)
    panic("teye: Failed to reallocate frame buffers");

  TEYE_clear_buffer(front_framebuffer, 0);
  TEYE_clear_buffer(back_framebuffer, 0);
}


void TEYE_init() {

  printf("\x1b[?1049h"); // Switch to alternate buffer

  hide_cursor();
  setlocale(LC_ALL, "");

  reset_frame_buffers();

  if (CharBuffer_init(&char_buffer) < 0)
    panic("teye: Couldn't initialize a character buffer");
}

void TEYE_blit(TEYE_Buffer src, DrawingMode mode, int dest_x, int dest_y,
               float scale_x, float scale_y) {

  if (mode == FitWidth) {
    scale_x = (float)front_framebuffer.width / src.width;
    scale_y = scale_x;
  } else if (mode == FitHeight) {
    scale_y = (float)front_framebuffer.height / src.height;
    scale_x = scale_y;
  } else if (mode == Stretch) {
    scale_x = (float)front_framebuffer.width / src.width;
    scale_y = (float)front_framebuffer.height / src.height;
  }

  // Calculate the target dimensions in the internal buffer
  int target_w = (int)(src.width * scale_x);
  int target_h = (int)(src.height * scale_y);

  // Iterate over the destination buffer
  for (int dy = 0; dy < target_h; dy++) {
    // Boundary check for y
    int actual_y = dest_y + dy;
    if (actual_y < 0 || actual_y >= front_framebuffer.height)
      continue;

    // Pre-calculate the source Y coordinate
    int sy = (int)(dy / scale_y);
    // Safety clamp for source Y
    if (sy >= src.height)
      sy = src.height - 1;

    int src_row_offset = sy * src.width;
    int dest_row_offset = actual_y * front_framebuffer.width;

    for (int dx = 0; dx < target_w; dx++) {
      // Boundary check for X
      int actual_x = dest_x + dx;
      if (actual_x < 0 || actual_x >= front_framebuffer.width)
        continue;

      // Calculate source X coordinate
      int sx = (int)(dx / scale_x);
      // Safety clamp for source X
      if (sx >= src.width)
        sx = src.width - 1;

      // Copy the pixel
      uint8_t pixel = src.buffer[src_row_offset + sx];
      if (pixel != 0) { // Transparency
        front_framebuffer.buffer[dest_row_offset + actual_x] = pixel;
      }
    }
  }
}

void TEYE_render_frame() {

  signal(SIGWINCH, signalHandler);

  // if (size_changed) {
  //   // the back buffer is now misleading, clean it
  //   TEYE_clear_buffer(buffer, 0)
  // }

  fflush(stdout);

  // Loop over the frame buffer, two rows at a time (since we render 2 rows per
  // character)

  // cursor position on the screen
  int i = 0;
  int j = -1;

  // pixel position buffer
  int x = 0, y = 0;
  // track the starting index of each line
  int x0 = 0, x1 = front_framebuffer.width;

  // We gotta save so bytes by not sending the same color code twice
  int prev_foreground = -1;
  int prev_background = -1;

  int to_ignore = 0;

  // Go the the start of the line
  CharBuffer_append_cursor_move(&char_buffer, 1, 1);

  do {

    if (++j >= front_framebuffer.width) {
      j = 0;
      i++;
      if (i >= front_framebuffer.height / 2)
        break;

      y = i * 2;
      x0 = y * front_framebuffer.width;
      x1 = x0 + front_framebuffer.width;

      prev_foreground = -1;
      prev_background = -1;

      CharBuffer_append_text(&char_buffer, ANSI_COLOR_RESET,
                             strlen(ANSI_COLOR_RESET));

      // Go the the start of the line
      CharBuffer_append_cursor_move(&char_buffer, i + 1, j + 1);
    }

    x = j;

    // Combine two rows to form one character
    uint8_t upper_half = front_framebuffer.buffer[x0 + x];
    uint8_t lower_half = front_framebuffer.buffer[x1 + x];
    uint8_t cahed_upper_half = back_framebuffer.buffer[x0 + x];
    uint8_t cached_lower_half = back_framebuffer.buffer[x1 + x];

    // Check if this pixel is different from the cache
    if (cahed_upper_half == upper_half && cached_lower_half == lower_half) {
      to_ignore++;
      continue;
    }

    // jump to the right coordinate
    if (to_ignore > 0) {
      CharBuffer_append_cursor_move(&char_buffer, i + 1, j + 1);
      to_ignore = 0;
    }

    // check the previous background color
    if (lower_half != prev_background) {
      char *color = get_color_from_number(lower_half);
      CharBuffer_append_text(&char_buffer, BACKGROUND, strlen(BACKGROUND));
      CharBuffer_append_text(&char_buffer, color, strlen(color));

      // update
      prev_background = lower_half;
    }

    // Based on the character_value, pick the color
    if (upper_half == lower_half) {
      CharBuffer_append_text(&char_buffer, " ", 1);
    } else {
      if (upper_half != prev_foreground) {
        char *color1 = get_color_from_number(upper_half);
        CharBuffer_append_text(&char_buffer, FORGROUND, strlen(FORGROUND));
        CharBuffer_append_text(&char_buffer, color1, strlen(color1));

        prev_foreground = upper_half;
      }
      CharBuffer_append_text(&char_buffer, "▀", strlen("▀"));
    }
  } while (1);

  if (screen_resized == 1) {
    // The screen was resized, we need to reallocate the framebuffers
    reset_frame_buffers();
    screen_resized = 0;

    move_cursor_top_left();
    clear_screen();
  }

  write(STDOUT_FILENO, char_buffer.buf, char_buffer.len);

  // print the number of bytes written for profiling purposes
  char buf[32];
  snprintf(buf, 32, "written: %ld\n", char_buffer.len);
  // write(STDERR_FILENO, buf, strlen(buf));

  // Swaps the back and front buffers
  uint8_t *temp = back_framebuffer.buffer;
  back_framebuffer.buffer = front_framebuffer.buffer;
  front_framebuffer.buffer = temp;

  // Cleans the character buffer to make it usable for the next iteration
  char_buffer.len = 0;
  char_buffer.buf[0] = '\0';
}

/** Restore the terminal and clean the memory*/
void TEYE_free() {
  show_cursor();

  TEYE_free_buffer(&front_framebuffer);
  TEYE_free_buffer(&back_framebuffer);

  CharBuffer_free(&char_buffer);
  printf("\x1b[?1049l"); // Return to primary buffer
}

void panic(const char *s) {

  TEYE_free();

  perror(s);
  exit(1);
}
