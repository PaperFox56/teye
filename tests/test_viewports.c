/**
 * bounce.c
 *
 * This file is part of the test suite for the TEYE library API.
 *
 * This test is used to test the viewport management of Teye.
 */

#include "timer.h"
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <teye/teye.h>

#if (TEYE_VERSION_MAJOR > 0) || (TEYE_VERSION_MINOR > 4)
#error                                                                         \
    "This test was made for Teye 0.4, it is probably deprecated. If the test is still compatible with the current version of the library, please update the condition above."
#endif

#define BLACK 232
#define WHITE 231

volatile atomic_int running = 1;

static int w = 20;
static int h = 20;

static void signal_handler() { running = 0; }

int main() {

  // Handles the interrupt signal from the terminal in orther to break the main
  // loop.
  signal(SIGINT, signal_handler);

  // The buffer is initialized to zero to prevent undefined behaviour from the
  // allocation function
  TEYE_Buffer buffer = {0};

  // Initialize the library
  if (TEYE_init(0) != 0) {
    perror("Couldn't initialize Teye");
    return -1;
  }

  if (TEYE_allocate_buffer(&buffer, w, h) != 0) {
    perror("Couldn't allocate a buffer");
    TEYE_free();
    exit(1);
  }

  /*Since TEYE was just initialised, the current size of the frame_buffer is the
   * same as the dimensions of the terminal*/
  int term_w = TEYE_get_framebuffer(0).width;
  int term_h = TEYE_get_framebuffer(0).height;

  int pos_x = (term_w - w) / 2;
  int pos_y = 0;

  /*Here we precompute a circle "sprite" that will be blit to the screen
   * every frame*/

  for (int i = 0; i < h; i++) {
    int x0 = i * w;
    for (int j = 0; j < w; j++) {

      int x = j - w / 2;
      int y = i - h / 2;

      buffer.buffer[j + x0] = (x * x + y * y < 100) ? WHITE : BLACK;
    }
  }

  float v = 1;

  while (running) {
    pos_y += v;

    // Update the vertical velocity
    if (pos_y >= term_h - h)
      v = -1;
    else if (pos_y <= 0)
      v = 1;

    // Clear the screen
    printf("\x1b[H");
    printf("\x1b[J");
    TEYE_clip_rendering_viewport(pos_x, pos_y, w, h);

    TEYE_blit(buffer, FitBest, 0, 0);
    TEYE_render_frame();

    // Basic FPS capping
    sleep_ms(1000 / 30);
  }

  // Don't forget to clean behind us
  TEYE_free_buffer(&buffer);
  TEYE_free();
}
