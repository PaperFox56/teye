/**
 * teye_core.c

Copyright (c) 2026 PaperFox56

This file is part of the teye library source code.
See the LICENCE section for details.

 */

#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h> // for terminal size

#include <teye/char_buffer.h>
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

// Nothing to see there, just a classic lookup table
static char colors[][5] = {
    "0m",   "1m",   "2m",   "3m",   "4m",   "5m",   "6m",   "7m",   "8m",
    "9m",   "10m",  "11m",  "12m",  "13m",  "14m",  "15m",  "16m",  "17m",
    "18m",  "19m",  "20m",  "21m",  "22m",  "23m",  "24m",  "25m",  "26m",
    "27m",  "28m",  "29m",  "30m",  "31m",  "32m",  "33m",  "34m",  "35m",
    "36m",  "37m",  "38m",  "39m",  "40m",  "41m",  "42m",  "43m",  "44m",
    "45m",  "46m",  "47m",  "48m",  "49m",  "50m",  "51m",  "52m",  "53m",
    "54m",  "55m",  "56m",  "57m",  "58m",  "59m",  "60m",  "61m",  "62m",
    "63m",  "64m",  "65m",  "66m",  "67m",  "68m",  "69m",  "70m",  "71m",
    "72m",  "73m",  "74m",  "75m",  "76m",  "77m",  "78m",  "79m",  "80m",
    "81m",  "82m",  "83m",  "84m",  "85m",  "86m",  "87m",  "88m",  "89m",
    "90m",  "91m",  "92m",  "93m",  "94m",  "95m",  "96m",  "97m",  "98m",
    "99m",  "100m", "101m", "102m", "103m", "104m", "105m", "106m", "107m",
    "108m", "109m", "110m", "111m", "112m", "113m", "114m", "115m", "116m",
    "117m", "118m", "119m", "120m", "121m", "122m", "123m", "124m", "125m",
    "126m", "127m", "128m", "129m", "130m", "131m", "132m", "133m", "134m",
    "135m", "136m", "137m", "138m", "139m", "140m", "141m", "142m", "143m",
    "144m", "145m", "146m", "147m", "148m", "149m", "150m", "151m", "152m",
    "153m", "154m", "155m", "156m", "157m", "158m", "159m", "160m", "161m",
    "162m", "163m", "164m", "165m", "166m", "167m", "168m", "169m", "170m",
    "171m", "172m", "173m", "174m", "175m", "176m", "177m", "178m", "179m",
    "180m", "181m", "182m", "183m", "184m", "185m", "186m", "187m", "188m",
    "189m", "190m", "191m", "192m", "193m", "194m", "195m", "196m", "197m",
    "198m", "199m", "200m", "201m", "202m", "203m", "204m", "205m", "206m",
    "207m", "208m", "209m", "210m", "211m", "212m", "213m", "214m", "215m",
    "216m", "217m", "218m", "219m", "220m", "221m", "222m", "223m", "224m",
    "225m", "226m", "227m", "228m", "229m", "230m", "231m", "232m", "233m",
    "234m", "235m", "236m", "237m", "238m", "239m", "240m", "241m", "242m",
    "243m", "244m", "245m", "246m", "247m", "248m", "249m", "250m", "251m",
    "252m", "253m", "254m", "255m"};

// tracks whether the terminal size changed since the last frame
static volatile sig_atomic_t screen_resized = 0;

static TEYE_Buffer front_framebuffer;

// This buffer is swaped with the main one after each frame.
static TEYE_Buffer back_framebuffer;

// Stores the frame's bytes before sending to the terminal
static struct CharBuffer char_buffer;

static TEYE_ResizeCallback resize_callback = NULL;

/****************
 Functions
*****************/

static void signalHandler() {
  screen_resized = 1;
  // char buf[32] = "screen resized\n";
  // write(STDERR_FILENO, buf, strlen(buf));
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

  // Since the buffer is going to grow anyway, we might as well do that now
  // We use an estimate of the number of character expected to render a frame
  CharBuffer_grow(&char_buffer, pixelcount(front_framebuffer) * 10);
}

void TEYE_set_resize_callback(TEYE_ResizeCallback callback) {
  resize_callback = callback;
}

TEYE_Buffer TEYE_get_framebuffer(int index) {
  switch (index) {
  case 0:
    return front_framebuffer;
  case 1:
    return back_framebuffer;
  default: {
    TEYE_Buffer invalid = {0};
    return invalid;
  }
  }
}

void TEYE_blit(TEYE_Buffer src, ScalingMode mode, int x, int y) {
  TEYE_blit_custom_scale_to(front_framebuffer, src, mode, x, y);
}

void TEYE_render_frame() {

  signal(SIGWINCH, signalHandler);

  // if (size_changed) {
  //   // the back buffer is now misleading, clean it
  //   TEYE_clear_buffer(buffer, 0)
  // }

  fflush(stdout);

  // Loop over the frame buffer, two rows at a time (since we render 2 rows
  // per character)

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

    // TODO: take into account the return value of the function
    if (resize_callback != NULL) {
      int code =
          resize_callback(front_framebuffer.width, front_framebuffer.height);
    }

    move_cursor_top_left();
    clear_screen();
  }

  write(STDOUT_FILENO, char_buffer.buf, char_buffer.len);

  // print the number of bytes written for profiling purposes

  // Swaps the back and front buffers
  uint8_t *temp = back_framebuffer.buffer;
  back_framebuffer.buffer = front_framebuffer.buffer;
  front_framebuffer.buffer = temp;

  TEYE_clear_buffer(front_framebuffer, 0);

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
