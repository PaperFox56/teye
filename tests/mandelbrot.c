/**
 * mandelbrot.c
 *
 * This file is part of the test suite for the TEYE library API.
 *
 * This program should display an animation of the mandelbrot set with a color
 * gradient indicating how fast a point diverges
 */

#include "timer.h"
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <teye/teye.h>

#define BLACK 232
#define WHITE 231

#if (TEYE_VERSION_MAJOR > 0) || (TEYE_VERSION_MINOR > 3)
#error                                                                         \
    "This test was made for Teye 0.3, it is probably deprecated. If the test is still compatible with the current version of the library, please update the condition above."
#endif

volatile atomic_int running = 1;

// Our buffer's resolution
static int w = 300;
static int h = 300;

static void signal_handler() { running = 0; }

int main() {

  // Handles the interrupt signal from the terminal in orther to break the main
  // loop.
  signal(SIGINT, signal_handler);

  // The buffer is initialized to zero to prevent undefined behaviour from the
  // allocation function
  TEYE_Buffer buffer = {0};

  // Initialize the library
  TEYE_init();

  if (TEYE_allocate_buffer(&buffer, w, h) != 0) {
    perror("Couldn't allocate a buffer");
    TEYE_free();
    exit(1);
  }

  int pos_x = w / 2;
  int pos_y = h / 2;

  int v = 2;

  while (running) {
    pos_y += v;

    if (pos_y > w - 150)
      v = -2;
    else if (pos_y < 50)
      v = 2;

    // This loop could be optimized but it is not needed
    for (int i = 0; i < h; i++) {
      int x0 = i * w;
      for (int j = 0; j < w; j++) {

        int x = j - pos_x;
        int y = i - pos_y;

        buffer.buffer[j + x0] = (x * x + y * y < 1000) ? WHITE : BLACK;
      }
    }

    TEYE_blit(buffer, FitWidth, 0, 0);
    TEYE_render_frame();

    // Basic FPS capping
    sleep_ms(1000 / 60);
  }

  // Don't forget to clean behind us
  TEYE_free_buffer(&buffer);
  TEYE_free();
}
