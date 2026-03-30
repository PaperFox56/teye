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
static int side = 400;

static void signal_handler() { running = 0; }

/**
 * Check if the complex number z = x + iy diverges under the mandelbrot set's
 * relation.
 * @return -1 if there is no divergence, the number of iterations otherwise.
 */
int check_divergence(float c_x, float c_y);

int main() {

  // Handles the interrupt signal from the terminal in orther to break the main
  // loop.
  signal(SIGINT, signal_handler);

  // The buffer is initialized to zero to prevent undefined behaviour from the
  // allocation function
  TEYE_Buffer buffer = {0};

  // Initialize the library
  if (TEYE_init() != 0) {
    perror("Couldn:t initialize Teye");
    return -1;
  }

  if (TEYE_allocate_buffer(&buffer, side, side) != 0) {
    perror("Couldn't allocate a buffer");
    TEYE_free();
    exit(1);
  }

  // How many units are represented by the with of the screen
  float units = 3.0;
  float scale = units / side;

  // origin
  float o_y = units / 2.0 + 1.;

  while (running) {

    // This loop could be optimized but it is not needed
    for (int i = 0; i < side; i++) {
      int x0 = i * side;
      for (int j = 0; j < side; j++) {

        float y = i * scale - units / 2.0;
        float x = j * scale - o_y;

        int diverges = check_divergence(x, y);
        buffer.buffer[j + x0] = (diverges == -1) ? BLACK : diverges;
      }
    }

    TEYE_blit(buffer, FitBest, 0, 0);
    TEYE_render_frame();

    units *= .99;
    scale = units / side;

    o_y -= units * .0035;

    // Basic FPS capping
    sleep_ms(1000 / 20);
  }

  // Don't forget to clean behind us
  TEYE_free_buffer(&buffer);
  TEYE_free();
}

int check_divergence(float c_x, float c_y) {
#define MAX_ITERATIONS 20
  float x = c_x;
  float y = c_y;
  for (int i = 0; i < MAX_ITERATIONS; i++) {
    float new_x = x * x - y * y + c_x;
    float new_y = 2 * x * y + c_y;

    x = new_x;
    y = new_y;

    // Calculate the magnitude
    if (x * x + y * y >= 4.0) {
      return i;
    }
  }
  return -1;
}
